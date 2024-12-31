#include "switchchatcontainer.h"
#include "../ChatContent/chatcontent.h"
#include <QtSql/QSqlDatabase>
#include <QLabel>
#include <QMessageBox>

SwitchChatContainer::SwitchChatContainer(int userId, QString userFirstname, QString userLastname, QWebSocket* m_client, QWidget *parent)
    : userId(userId), userFirstname(userFirstname), userLastname(userLastname), m_socket(m_client), QWidget{parent} {

    connect(m_socket, &QWebSocket::textMessageReceived, this, &SwitchChatContainer::updateButtons);

    switchLayout = new QVBoxLayout(this);
    switchLayout->setContentsMargins(0,0,0,0);
    switchLayout->setAlignment(Qt::AlignTop);
    switchLayout->setSpacing(0);

    setStyleSheet("QWidget {border-right: 1px solid #444}");
    toggleButton = new QPushButton("Show/Hide menu");
    toggleButton->setFont(QFont("Helvetica", 11));
    toggleButton->setStyleSheet("QPushButton { border: 0px; background-color: #222; "
                                "color: white; border-right: 1px solid #333; }"
                                "QPushButton::hover { background-color: #444; }");

    switchChats = new QStackedWidget(this);

    QWidget* startChat = new QWidget(this);
    QVBoxLayout* layoutStartChat = new QVBoxLayout(startChat);
    layoutStartChat->setAlignment(Qt::AlignCenter);
    startChat->setContentsMargins(0,0,0,0);

    QLabel* startChatContent = new QLabel("Select chat or start a conversation with a new contact!");
    layoutStartChat->addWidget(startChatContent);

    switchChats->addWidget(startChat);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "border: none;"
        "}"
        "QScrollBar:vertical {"
        "background-color: #222;"
        "width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "background-color: #333;"
        "min-height: 10px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "height: 10px;"
        "background-color: #222;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "height: 10px;"
        "background-color: #222;"
        "}"
    );

    // Создаём контейнер для кнопок
    buttonContainer = new QWidget(scrollArea);
    buttonLayout = new QVBoxLayout(buttonContainer);
    buttonLayout->setAlignment(Qt::AlignTop);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->addWidget(toggleButton);

    // Связываем контейнер с QScrollArea
    buttonContainer->setLayout(buttonLayout);
    scrollArea->setWidget(buttonContainer);

    // Добавляем QScrollArea в switchLayout
    switchLayout->addWidget(scrollArea);

    addSwitchButtons();
    setLayout(switchLayout);
}

void SwitchChatContainer::addSwitchButtons() {
    QSqlQuery query = executeQuery(
        "SELECT chats.id, users.firstname, users.lastname, "
        "(SELECT messages.body FROM messages "
        "WHERE messages.chat_id = chats.id "
        "ORDER BY messages.created_at DESC LIMIT 1) AS last_message "
        "FROM chats "
        "INNER JOIN users ON (users.id = chats.user1_id OR users.id = chats.user2_id) "
        "WHERE (chats.user1_id = :user_id OR chats.user2_id = :user_id) AND users.id != :user_id "
        "ORDER BY chats.created_at ASC;",
        {{":user_id", userId}}
    );

    while (query.next()) {
        int index = switchChats->count();
        int chatId = query.value("id").toInt();
        QString chatName = "[Chat] with " + query.value("firstname").toString() + " " + query.value("lastname").toString();
        QString lastMessage = query.value("last_message").toString();

        if (!switchButtons.contains(chatId)) {
            auto button = new SwitchChatButton(chatName, switchButtons.size() + 1, chatId, this);
            switchButtons[chatId] = button;

            button->setText(chatName + "\n\n" + lastMessage);

            buttonLayout->addWidget(button); // Добавляем кнопку в buttonLayout

            auto chatContent = new ChatContent(chatName, chatId, userId, m_socket, this);
            switchChats->addWidget(chatContent);

            connect(button, &QPushButton::clicked, this, [this, button, index, chatId]() {
                switchChats->setCurrentIndex(index);
                updateStatus(chatId);
            });
        }
    }
}

// QString SwitchChatContainer::formatButtonText(const QString& chatName, const QString& lastMessage) {
//     return QString(
//                "<div style='text-align: left;'>"
//                "<span style='font-weight: bold;'>%1</span><br>"
//                "<span style='background-color: lightgray; padding: 2px;'>%2</span>"
//                "</div>"
//                ).arg(chatName, lastMessage);
// }

void SwitchChatContainer::clearLayout(QLayout *layout) {
    if (!layout)
        return;

    // Удаляем все виджеты из макета
    while (QLayoutItem *item = layout->takeAt(1)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
    }
}

void SwitchChatContainer::updateButtons() {
    QSqlQuery query = executeQuery(
        "SELECT chats.id, "
        "(SELECT messages.body FROM messages "
        "WHERE messages.chat_id = chats.id "
        "ORDER BY messages.created_at DESC LIMIT 1) AS last_message "
        "FROM chats "
        "WHERE chats.user1_id = :user_id OR chats.user2_id = :user_id;",
        {{":user_id", userId}}
    );

    QSqlQuery squery;
    while(query.next()) {
        int chat_id = query.value("id").toInt();
        QString lastMessage = query.value("last_message").toString();

        squery = executeQuery("SELECT COUNT(*) AS total_count FROM message_read_status "
                              "INNER JOIN messages ON messages.id = message_read_status.message_id "
                              "WHERE message_read_status.user_id = :userid "
                              "AND message_read_status.is_read = false AND messages.chat_id = :chatid",
                              {
                                  {":chatid", chat_id},
                                  {":userid", userId}
                              });
        squery.next();
        int unreadCount = squery.value("total_count").toInt();

        // Обновляем текст кнопки
        if (switchButtons.contains(chat_id)) {
            SwitchChatButton* button = switchButtons.value(chat_id);
            if (switchChats->currentIndex() == button->switchIndex) { // Текущий активный чат

                button->setTextButton(0, lastMessage);
                updateStatus(chat_id); // Обновляем статус сообщений
            } else {
                button->setTextButton(unreadCount, lastMessage); // Обновляем количество непрочитанных сообщений
            }
        }
    }
}

void SwitchChatContainer::updateStatus(int index) {
    QSqlQuery squery = executeQuery("SELECT message_read_status.id FROM message_read_status "
                          "INNER JOIN messages ON messages.id = message_read_status.message_id "
                          "WHERE message_read_status.user_id = :userid "
                          "AND message_read_status.is_read = false AND messages.chat_id = :chatid",
                          {
                           {":chatid", index},
                           {":userid", userId}
                          });

    QSqlQuery updateQuery;

    while(squery.next()) {
        updateQuery = executeQuery("UPDATE message_read_status SET is_read = true WHERE id = :id",
                              {{":id", squery.value("id").toInt()}});
    }
}

QSqlQuery SwitchChatContainer::executeQuery(const QString& queryStr, const QVariantMap& params) {
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(queryStr);
    for (auto it = params.begin(); it != params.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        QMessageBox::critical(nullptr, "Ошибка", query.lastError().text());
    }
    return query;
}

void SwitchChatContainer::showChat() {
    QObject* senderObj = sender();
    SwitchChatButton* button = qobject_cast<SwitchChatButton*>(senderObj);

    switchChats->setCurrentIndex(button->switchIndex);
}
