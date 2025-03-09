#include "chatserver.h"
#include <QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

ChatServer::ChatServer(quint16 port, QObject *parent) : QObject{parent},
    m_server(new QWebSocketServer(QStringLiteral("Chat Server"), QWebSocketServer::NonSecureMode, this)) {

    if(m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Chat server listening on port: " << port;
        connect(m_server, &QWebSocketServer::newConnection, this, &ChatServer::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &ChatServer::closed);

        // Вызов функции по подключению к базе данных
        connectToDatabase();
    } else {
        qCritical() << "Failed to start server:" << m_server->errorString();
    }
}

void ChatServer::connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("SimpleChat");
    db.setUserName("postgres");
    db.setPassword("11281215");

    if(!db.open()) {
        qDebug() << "Error connectiong database: " << db.lastError().text();
    } else {
        qDebug() << "Connected to database!";
    }
}

ChatServer::~ChatServer() {
    m_server->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void ChatServer::onNewConnection() {
    QWebSocket *client = m_server->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &ChatServer::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &ChatServer::onClientDisconnected);

    m_clients.insert(client);
    qDebug() << "New client connected.";
}

void ChatServer::onTextMessageReceived(const QString &message) {
    QWebSocket *senderClient = qobject_cast<QWebSocket *>(sender());
    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    if (senderClient && message_doc.isObject()) {
        QJsonObject message_obj = message_doc.object();
        QString action = message_obj.value("action").toString();

        if(action == "register") {
            handleRegistration(senderClient, message_obj);
        } else if(action == "login") {
            handleLogin(senderClient, message_obj);
        } else if (action == "send_message") {
            handleSendMessage(senderClient, message_obj);
        } else if (action == "chat_messages") {
            handleChatMessages(senderClient, message_obj);
        } else if (action == "add_chats") {
            handleAddChats(senderClient, message_obj);
        } else if (action == "update_chats") {
            handleUpdateChats(senderClient, message_obj);
        } else if (action == "update_stat") {
            handleUpdateStatusMessages(senderClient, message_obj);
        } else if (action == "update_user") {
            handleUpdateStatusUser(senderClient, message_obj);
        } else if (action == "add_users") {
            handleAddUsers(senderClient, message_obj);
        } else if (action == "create_chat") {
            handleCreateChat(senderClient, message_obj);
        } else if (action == "searching") {
            handleSearching(senderClient, message_obj);
        }
    }
}

void ChatServer::handleRegistration(QWebSocket* senderClient, const QJsonObject &message) {
    QString firstNameLine = message.value("firstname").toString();
    QString lastNameLine = message.value("lastname").toString();
    QString mailLine = message.value("email").toString();
    QString hashPass = message.value("password").toString();
    QSqlQuery query = executeQuery("INSERT INTO users (firstname, lastname, email, password) "
                                   "VALUES (:firstname, :lastname, :email, :password) RETURNING id, firstname, lastname",
                                   {
                                       {":firstname", firstNameLine},
                                       {":lastname", lastNameLine},
                                       {":email", mailLine},
                                       {":password", hashPass}
                                   }, "Не удалось добавить пользователя", "error_login", senderClient);
    QJsonObject messageObj;
    if (query.next()) {
        int userid = query.value("id").toInt();
        QString userFirstname = query.value("firstname").toString();
        QString userLastname = query.value("lastname").toString();

        // Добавляем авторизованного пользователя
        m_autorizedClients[userid] = senderClient;

        messageObj["userid"] = userid;
        messageObj["firstname"] = userFirstname;
        messageObj["lastname"] = userLastname;
    }

    messageObj["type"] = "register";
    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleLogin(QWebSocket* senderClient, const QJsonObject &message) {
    QString email = message.value("email").toString();
    QString password = message.value("password").toString();
    QSqlQuery query = executeQuery("SELECT id, firstname, lastname, email, password FROM users WHERE email = :email",
                                   {{":email", email}}, "Не удалось найти пользователя", "error_login", senderClient);
    QJsonObject messageObj;
    if(query.next()) {
        int userid = query.value("id").toInt();
        QString userFirstname = query.value("firstname").toString();
        QString userLastname = query.value("lastname").toString();
        QString userPassword = query.value("password").toString();

        // Добавляем авторизованного пользователя
        m_autorizedClients[userid] = senderClient;
        if(!password.contains(userPassword)) {
            return; ////
        }

        messageObj["userid"] = userid;
        messageObj["firstname"] = userFirstname;
        messageObj["lastname"] = userLastname;
    }

    messageObj["type"] = "login";

    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);
    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleSendMessage(QWebSocket* senderClient, const QJsonObject &message) {
    if (senderClient) {
        int user_id = message.value("user_id").toInt();
        QString body = message.value("body").toString();
        QString selfName = message.value("selfName").toString();
        int chat_id = message.value("chat_id").toInt();
        QDateTime created_at = QDateTime::currentDateTime();
        QByteArray imageData;
        if (message.contains("image_data")) {
            // Декодируем Base64 строку в QByteArray
            imageData = QByteArray::fromBase64(message.value("image_data").toString().toUtf8());
        }

        QSqlQuery query;
        query.prepare("INSERT INTO messages (user_id, body, chat_id, created_at, image_data) "
                      "VALUES (:user_id, :body, :chat_id, :created_at, :image_data) RETURNING id");
        query.bindValue(":user_id", user_id);
        query.bindValue(":body", body);
        query.bindValue(":chat_id", chat_id);
        query.bindValue(":created_at", created_at.toString(Qt::ISODateWithMs));
        query.bindValue(":image_data", imageData);

        if (!query.exec()) {
            qDebug() << "Ошибка добавления сообщения:" << query.lastError().text();
            QJsonObject errMsgObject;
            errMsgObject["type"] = "error_chat";
            errMsgObject["message"] = "Ошибка при добавлении сообщения";

            QJsonDocument errMsgDocument(errMsgObject);
            senderClient->sendTextMessage(errMsgDocument.toJson(QJsonDocument::Compact));
            return;
        }

        query.next();
        int message_id = query.value("id").toInt();

        query = executeQuery("SELECT CASE WHEN user1_id != :userid THEN user1_id ELSE user2_id "
                             "END AS result FROM chats WHERE (user1_id != :userid OR user2_id != :userid) AND chats.id = :chatid ",
                             {
                                 {":userid", user_id},
                                 {":chatid", chat_id}
                             }, "Ошибка при добавлении сообщения", "error_chat", senderClient);

        query.next();
        int user2_id = query.value("result").toInt();

        query = executeQuery("INSERT INTO message_read_status (message_id, user_id, is_read) "
                             "VALUES (:message_id, :user_id, :is_read)", {
                                 {":message_id", message_id},
                                 {":user_id", user2_id},
                                 {":is_read", false}
                             }, "Ошибка при добавлении сообщения", "error_chat", senderClient);
        qDebug() << user_id;
        qDebug() << user2_id;
        if(m_autorizedClients.contains(user2_id)) {
            QJsonObject sendMessage;
            sendMessage["type"] = "send";
            sendMessage["chatid"] = chat_id;
            // Первая часть для чата
            sendMessage["user_id"] = user_id;
            sendMessage["firstname"] = selfName;
            sendMessage["body"] = body;
            sendMessage["created_at"] = created_at.toString();
            sendMessage["image_data"] = QString::fromUtf8(imageData.toBase64());
            // Вторая для переключателя чатов
            sendMessage["unreadCount"] = 0;
            sendMessage["userLastMessage"] = selfName;
            sendMessage["lastMessage"] = body;
            QJsonDocument messageDoc(sendMessage);
            QString jsonString = messageDoc.toJson(QJsonDocument::Compact);

            m_autorizedClients[user2_id]->sendTextMessage(jsonString);

        }
    }
}

void ChatServer::handleChatMessages(QWebSocket* senderClient, const QJsonObject &message) {
    int chatid = message.value("chatid").toInt();
    QSqlQuery query = executeQuery("SELECT users.firstname, messages.user_id, messages.body, messages.created_at, messages.id, messages.image_data "
                                          "FROM users "
                                          "INNER JOIN messages ON users.id = messages.user_id "
                                          "WHERE messages.chat_id = :chatid "
                                          "ORDER BY messages.created_at ASC;",
                                   {{":chatid", chatid}}, "Не удалось найти данные чата", "error_chat", senderClient);
    QJsonArray userInfoArray;
    while(query.next()) {
        QString firstname = query.value("firstname").toString();
        int user_id = query.value("user_id").toInt();
        QString body = query.value("body").toString();
        QString created_at = query.value("created_at").toString();
        QByteArray image_data = query.value("image_data").toByteArray();

        QJsonObject userInfoObj;
        userInfoObj["user_id"] = user_id;
        userInfoObj["firstname"] = firstname;
        userInfoObj["body"] = body;
        userInfoObj["created_at"] = created_at;
        userInfoObj["image_data"] = QString::fromUtf8(image_data.toBase64());

        userInfoArray.append(userInfoObj);
    }

    QJsonObject response;
    response["type"] = "chat_info";
    response["chatid"] = chatid;
    response["data"] = userInfoArray;

    QJsonDocument resDoc(response);
    QString jsonString = QString::fromUtf8(resDoc.toJson(QJsonDocument::Compact));

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleAddChats(QWebSocket* senderClient, const QJsonObject &message) {
    int userid = message.value("userid").toInt();

    QSqlQuery query = executeQuery(
        "SELECT chats.id AS chat_id, "
        "users.firstname AS participant_firstname, "
        "users.lastname AS participant_lastname, "
        "users.email AS participant_email, "
        "users.id AS participant_user_id, "
        "last_message.body AS last_message_body, "
        "last_message.user_id AS last_message_user_id, "
        "sender.firstname AS sender_firstname, "
        "sender.lastname AS sender_lastname "
        "FROM chats "
        "INNER JOIN users ON (users.id = chats.user1_id OR users.id = chats.user2_id) "
        "LEFT JOIN ("
        "  SELECT m1.chat_id, m1.body, m1.user_id "
        "  FROM messages m1 "
        "  INNER JOIN ("
        "      SELECT chat_id, MAX(created_at) AS max_created_at "
        "      FROM messages "
        "      GROUP BY chat_id"
        "  ) m2 ON m1.chat_id = m2.chat_id AND m1.created_at = m2.max_created_at"
        ") AS last_message ON last_message.chat_id = chats.id "
        "LEFT JOIN users AS sender ON sender.id = last_message.user_id "
        "WHERE (chats.user1_id = :user_id OR chats.user2_id = :user_id) AND users.id != :user_id "
        "ORDER BY chats.created_at ASC;",
        {{":user_id", userid}},
        "Не удалось найти чаты",
        "error_chat",
        senderClient);

    QJsonArray messageArray;
    while(query.next()) {
        int chatId = query.value("chat_id").toInt();
        int user2id = query.value("participant_user_id").toInt();
        QString email = query.value("participant_email").toString();
        QString chatName = query.value("participant_firstname").toString() + " " + query.value("participant_lastname").toString();
        int posInit = chatName.indexOf(' ');
        QString userName = chatName.mid(0,1).toUpper() + chatName.mid(1,posInit) + chatName.mid(posInit+1, 1).toUpper() + chatName.mid(posInit+2, chatName.size());
        QString lastMessage = query.value("last_message_body").toString();
        QString userLastMessage = query.value("sender_firstname").toString();

        QJsonObject chatInfoObj;
        chatInfoObj["chatId"] = chatId;
        chatInfoObj["userName"] = userName;
        chatInfoObj["user2Id"] = user2id;
        chatInfoObj["userLastMessage"] = userLastMessage;
        chatInfoObj["lastMessage"] = lastMessage;
        chatInfoObj["email"] = email;
        if(m_autorizedClients.contains(user2id)) {
            chatInfoObj["status"] = true;
        } else {
            chatInfoObj["status"] = false;
        }
        messageArray.append(chatInfoObj);
    }

    // Ответ клиенту о содержимом чата
    QJsonObject response;
    response["type"] = "add_chats";
    response["data"] = messageArray;

    QJsonDocument resDoc(response);
    QString jsonString = resDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleUpdateChats(QWebSocket* senderClient, const QJsonObject &message) {
    int userid = message.value("userid").toInt();
    qDebug() << "Обновление переключателей с сервера";

    QSqlQuery query = executeQuery(
        "SELECT "
        "chats.id AS chat_id, "
        "last_message.body AS last_message_body, "
        "sender.firstname AS sender_firstname, "
        "sender.lastname AS sender_lastname "
        "    FROM chats "
        "        LEFT JOIN ( "
        "            SELECT "
        "                m.chat_id, "
        "                m.body, "
        "                m.user_id "
        "            FROM messages m "
        "            WHERE m.created_at = ( "
        "            SELECT MAX(created_at) "
        "                FROM messages "
        "                    WHERE chat_id = m.chat_id "
        "                ) "
        "            ) AS last_message ON last_message.chat_id = chats.id "
        "      LEFT JOIN users AS sender ON sender.id = last_message.user_id "
        "WHERE chats.user1_id = :user_id OR chats.user2_id = :user_id;",
        {{":user_id", userid}}, "Не удалось найти чаты", "error_chat", senderClient);

    QSqlQuery squery;
    QJsonArray messageArray;
    while(query.next()) {
        int chatid = query.value("chat_id").toInt();
        QString lastMessage = query.value("last_message_body").toString();
        QString userLastMessage = query.value("sender_firstname").toString();
        squery = executeQuery("SELECT COUNT(*) AS total_count FROM message_read_status "
                              "INNER JOIN messages ON messages.id = message_read_status.message_id "
                              "WHERE message_read_status.user_id = :userid "
                              "AND message_read_status.is_read = false AND messages.chat_id = :chatid",
                              {
                                  {":chatid", chatid},
                                  {":userid", userid}
                              }, "Не удалось получить данные о сообщениях", "error_chat", senderClient);

        squery.next();
        int unreadCount = squery.value("total_count").toInt();

        QJsonObject arrayObj;
        arrayObj["unreadCount"] = unreadCount;
        arrayObj["lastMessage"] = lastMessage;
        arrayObj["userLastMessage"] = userLastMessage;
        arrayObj["chatid"] = chatid;

        messageArray.append(arrayObj);
    }

    QJsonObject responseObj;
    responseObj["type"] = "update_chats";
    responseObj["data"] = messageArray;

    QJsonDocument resDoc(responseObj);
    QString jsonString = resDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleUpdateStatusMessages(QWebSocket* senderClient, const QJsonObject &message) {
    int chatid = message.value("chatid").toInt();
    int userid = message.value("userid").toInt();

    QSqlQuery squery = executeQuery("SELECT message_read_status.id FROM message_read_status "
                                    "INNER JOIN messages ON messages.id = message_read_status.message_id "
                                    "WHERE message_read_status.user_id = :userid "
                                    "AND message_read_status.is_read = false AND messages.chat_id = :chatid",
                                    {
                                        {":chatid", chatid},
                                        {":userid", userid}
                                    }, "Не удалось получить данные о сообщении", "error_chat", senderClient);

    QSqlQuery updateQuery;

    while(squery.next()) {
        updateQuery = executeQuery("UPDATE message_read_status SET is_read = true WHERE id = :id",
                                   {{":id", squery.value("id").toInt()}}, "Не удалось обновить данные о сообщении", "error_chat", senderClient);
    }
}

void ChatServer::handleUpdateStatusUser(QWebSocket* senderClient, const QJsonObject &message) {
    int userid = message.value("userid").toInt();
    QSqlQuery query = executeQuery("SELECT id, firstname, lastname, email FROM users WHERE users.id != :id",
                                   {{":id", userid}}, "Не удалось получить данные о пользователях", "error_chat", senderClient);

    QJsonArray messageArray;
    while(query.next()) {
        QJsonObject arrayObj;
        int userId = query.value("id").toInt();
        arrayObj["id"] = userId;
        arrayObj["status"] = m_autorizedClients[userId] ? true : false;
        messageArray.append(arrayObj);
    }

    QJsonObject responseObj;
    responseObj["type"] = "update_status";
    responseObj["data"] = messageArray;

    QJsonDocument resDoc(responseObj);
    QString jsonString = resDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleAddUsers(QWebSocket* senderClient, const QJsonObject &message) {
    int userid = message.value("userid").toInt();
    QSqlQuery query = executeQuery("SELECT id, firstname, lastname, email FROM users WHERE users.id != :id",
                                   {{":id", userid}}, "Не удалось получить данные о пользователях", "error_choice", senderClient);

    QJsonArray messageArray;
    while(query.next()) {
        QJsonObject arrayObj;
        arrayObj["firstname"] = query.value("firstname").toString();
        arrayObj["lastname"] = query.value("lastname").toString();
        arrayObj["email"] = query.value("email").toString();
        arrayObj["id"] = query.value("id").toInt();

        messageArray.append(arrayObj);
    }

    QJsonObject responseObj;
    responseObj["type"] = "add_users";
    responseObj["data"] = messageArray;

    QJsonDocument resDoc(responseObj);
    QString jsonString = resDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

void ChatServer::handleCreateChat(QWebSocket* senderClient, const QJsonObject &message) {
    int userid = message.value("userid").toInt();
    int user2id = message.value("user2id").toInt();

    QSqlQuery query = executeQuery("SELECT id FROM chats WHERE (user1_id=:user1_id AND user2_id=:user2_id) OR (user1_id=:user2_id AND user2_id=:user1_id)",
                                   {
                                    {":user1_id", userid},
                                    {":user2_id", user2id}
                                   }, "Не удалось найти чаты", "error_choice", senderClient);

    if(!query.next()) {
        query = executeQuery("INSERT INTO chats (user1_id, user2_id) VALUES (:user1_id, :user2_id)",
                             {
                                 {":user1_id", userid},
                                 {":user2_id", user2id}
                             }, "Не удалось создать чат", "error_choice", senderClient);
    }
}

void ChatServer::handleSearching(QWebSocket* senderClient, const QJsonObject &message) {
    int chat_id = message.value("chat_id").toInt();
    QString searchText = message.value("search").toString();

    QSqlQuery query = executeQuery("SELECT messages.user_id, messages.body, messages.created_at, messages.image_data "
                                   "FROM messages "
                                   "WHERE body LIKE '%' || :search || '%' AND messages.chat_id=:chat_id "
                                   "ORDER BY messages.created_at ASC;",
                                   {
                                    {":chat_id", chat_id},
                                    {":search", searchText}
                                   }, "Не удалось получить данные чата", "error_choice", senderClient);
    QJsonArray responseArray;
    while(query.next()) {
        int user_id = query.value("user_id").toInt();
        QString body = query.value("body").toString();
        QString created_at = query.value("created_at").toString();
        QByteArray image_data = query.value("image_data").toByteArray();

        qDebug() << body;
        QJsonObject messageObj;
        messageObj["user_id"] = user_id;
        messageObj["firstname"] = "search";
        messageObj["body"] = body;
        messageObj["created_at"] = created_at;
        messageObj["image_data"] = QString::fromUtf8(image_data.toBase64());

        responseArray.append(messageObj);
    }
    QJsonObject responseObj;
    responseObj["type"] = "searching";
    responseObj["chatid"] = chat_id;
    responseObj["data"] = responseArray;

    QJsonDocument resDoc(responseObj);
    QString jsonString = resDoc.toJson(QJsonDocument::Compact);

    senderClient->sendTextMessage(jsonString);
}

QSqlQuery ChatServer::executeQuery(const QString& command, const QVariantMap& params, const QString& errorMessage, const QString& error_type, QWebSocket* senderClient) {
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(command);

    for(auto it = params.begin(); it != params.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qDebug() << errorMessage << query.lastError().text();
        QJsonObject errMsgObject;
        errMsgObject["type"] = error_type;
        errMsgObject["message"] = errorMessage;

        QJsonDocument errMsgDocument(errMsgObject);
        senderClient->sendTextMessage(errMsgDocument.toJson(QJsonDocument::Compact));
    }
    return query;
}

void ChatServer::onClientDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.remove(client);

        for(auto it = m_autorizedClients.begin(); it != m_autorizedClients.end(); ++it) {
            if(it.value() == client) {
                m_autorizedClients.erase(it);
                break;
            }
        }

        client->deleteLater();
        qDebug() << "Client disconnected.";
    }
}

