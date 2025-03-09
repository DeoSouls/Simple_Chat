#include "switchchatcontainer.h"
#include "../ChatContent/chatcontent.h"
#include <QtSql/QSqlDatabase>
#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

SwitchChatContainer::SwitchChatContainer(int userId, QString userFirstname, QString userLastname, QWebSocket* m_client, QWidget *parent)
    : userId(userId), userFirstname(userFirstname), userLastname(userLastname), m_socket(m_client), QWidget{parent} {

    // Подключаем сокет к обработчику входящих сообщений
    connect(m_socket, &QWebSocket::textMessageReceived, this, &SwitchChatContainer::updateHandler);

    // Отправка сообщения для инициализации существующих чатов для пользователя
    QJsonObject messageObj;
    messageObj["action"] = "add_chats";
    messageObj["userid"] = userId;
    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);

    // Основной вертикальный layout
    switchLayout = new QVBoxLayout(this);
    switchLayout->setContentsMargins(0,0,0,0);
    switchLayout->setAlignment(Qt::AlignTop);
    switchLayout->setSpacing(0);

    // Верхняя панель для поиска и меню
    QWidget* topPanel = new QWidget(this);
    QHBoxLayout* panelLayout = new QHBoxLayout(topPanel);
    topPanel->setFixedHeight(40);
    topPanel->setStyleSheet("QWidget { background-color: #222; border: none;"
                            "border-bottom: 1px solid #333; }"
                            "QPushButton { border: 1px solid #444; background-color: #222; "
                            "color: white; border-radius: 8px;}"
                            "QPushButton::hover { background-color: #444; }"
                            "QLineEdit { background-color: #444; border-radius: 10px; "
                            "margin-left: 5px; color: white; padding-left: 5px;}");
    panelLayout->setAlignment(Qt::AlignRight);
    panelLayout->setContentsMargins(0,0,8,0);

    // Поле поиска
    searchLine = new QLineEdit();
    searchLine->setFixedHeight(30);
    searchLine->setFont(QFont("Segoe UI", 12, 300));
    searchLine->setPlaceholderText("Введите текст...");
    panelLayout->addWidget(searchLine,1);

    // Кнопка поиска
    searchOnTopPanel = new QPushButton();
    searchOnTopPanel->setFixedSize(30,30);
    searchOnTopPanel->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconSearch.png"));
    searchOnTopPanel->setIconSize(QSize(17,17));
    panelLayout->addWidget(searchOnTopPanel);
    connect(searchOnTopPanel, &QPushButton::clicked, this, &SwitchChatContainer::openSearch);

    // Кнопка меню
    toggleButton = new QPushButton();
    toggleButton->setFixedSize(30,30);
    toggleButton->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconsMenu.png"));
    toggleButton->setIconSize(QSize(17,17));
    panelLayout->addWidget(toggleButton);

    // Контейнер экранов чата
    switchChats = new QStackedWidget(this);

    // Начальный экран выбора чата
    QWidget* startChat = new QWidget(this);
    QVBoxLayout* layoutStartChat = new QVBoxLayout(startChat);
    layoutStartChat->setAlignment(Qt::AlignCenter);
    startChat->setContentsMargins(0,0,0,0);

    QLabel* startChatContent = new QLabel("Select chat or start a conversation with a new contact! ✨");
    startChatContent->setFont(QFont("Segoe UI", 12, 300));
    layoutStartChat->addWidget(startChatContent);

    switchChats->addWidget(startChat);

    // Область прокрутки для списка чатов
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
    buttonLayout->addWidget(topPanel);

    // Связываем контейнер с QScrollArea
    buttonContainer->setLayout(buttonLayout);
    scrollArea->setWidget(buttonContainer);

    // Добавляем QScrollArea в switchLayout
    switchLayout->addWidget(scrollArea);
    setLayout(switchLayout);
}

// Обработчик принятого сообщения от сервера
void SwitchChatContainer::updateHandler(const QString& message) {
    // Парсим JSON-ответ от сервера и
    // вызываем соответствующие методы обновления кнопок и статусов
    QJsonDocument response_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response_obj = response_doc.object();
    QString type = response_obj.value("type").toString();

    if(type.contains("error_chat")) {
        QMessageBox::critical(this, "Ошибка: ", response_obj.value("message").toString());
        return;
    }

    if(type.contains("add_chats")) {
        addSwitchButtons(response_obj);
    } else if (type.contains("update_chats")) {
        updateButtons(response_obj);
    } else if (type.contains("update_status")) {
        updateStatus(response_obj);
    }
}

// Добавление кнопок переключения между чатами в интерфейс из JSON-ответа сервера
void SwitchChatContainer::addSwitchButtons(const QJsonObject& message) {
    // Добавляем кнопки чатов в интерфейс на основе данных от сервера
    QJsonArray responseArray;
    responseArray = message["data"].toArray();

    for(const QJsonValue& value : responseArray) {
        QJsonObject responseObj = value.toObject();
        int index = switchChats->count();
        int chatId = responseObj["chatId"].toInt();
        int user2Id = responseObj["user2Id"].toInt();
        QString email = responseObj["email"].toString();
        QString userName = responseObj["userName"].toString();
        QString lastMessage = responseObj["lastMessage"].toString();
        QString userLastMessage = responseObj["userLastMessage"].toString();
        bool status = responseObj["status"].toBool();

        if (!switchButtons.contains(chatId)) {
            auto button = new SwitchChatButton(userName, userLastMessage, switchButtons.size() + 1, chatId, this);
            switchButtons[chatId] = button;

            button->setLastMessage(0,lastMessage, userLastMessage);
            buttonLayout->addWidget(button); // Добавляем кнопку в buttonLayout

            auto chatContent = new ChatContent(userName, userFirstname, email, status, chatId, userId, user2Id, m_socket, this);
            switchChats->addWidget(chatContent);

            connect(button, &SwitchChatButton::clicked, this, [this, button, index, chatId]() {
                switchChats->setCurrentIndex(index);
                QJsonObject mesObj;
                mesObj["action"] = "update_stat";
                mesObj["userid"] = userId;
                mesObj["chatid"] = chatId;

                QJsonDocument messageDoc(mesObj);
                QString jsonString = messageDoc.toJson(QJsonDocument::Compact);
                m_socket->sendTextMessage(jsonString);
            });
        }
    }
    QJsonObject messageObj;
    messageObj["action"] = "update_chats";
    messageObj["userid"] = userId;

    QJsonDocument mesDoc(messageObj);
    QString jsonString2 = mesDoc.toJson(QJsonDocument::Compact);
    m_socket->sendTextMessage(jsonString2);
}

// Обновление состояния кнопок (непрочитанные сообщения)
void SwitchChatContainer::updateButtons(const QJsonObject& message) {
    // Обновляем информацию о последних сообщениях и
    // количестве непрочитанных сообщений в кнопках-чата
    QJsonArray responseArray = message["data"].toArray();
    for(const QJsonValue& value : responseArray) {
        QJsonObject messageObj = value.toObject();

        QString lastMessage = messageObj.value("lastMessage").toString();
        qDebug() << "Последнее сообщение: "<< lastMessage;
        QString userLastMessage = messageObj.value("userLastMessage").toString();
        int unreadCount = messageObj.value("unreadCount").toInt();
        int chatid = messageObj.value("chatid").toInt();

        if (switchButtons.contains(chatid)) {
            SwitchChatButton* button = switchButtons.value(chatid);
            if (switchChats->currentIndex() == button->switchIndex) { // Текущий активный чат
                button->setLastMessage(0, lastMessage, userLastMessage);
                QJsonObject mesObj;
                mesObj["action"] = "update_stat";
                mesObj["userid"] = userId;
                mesObj["chatid"] = chatid;

                QJsonDocument messageDoc(mesObj);
                QString jsonString = messageDoc.toJson(QJsonDocument::Compact);
                m_socket->sendTextMessage(jsonString);
            } else {
                button->setLastMessage(unreadCount, lastMessage, userLastMessage); // Обновляем количество непрочитанных сообщений
            }
        }
    }
}

// Обновление статуса пользователей (в сети/не в сети)
void SwitchChatContainer::updateStatus(const QJsonObject& message) {
    // Обновляем статус пользователей в интерфейсе чата (онлайн/оффлайн)
    std::unordered_map<int, bool> userStatusMap;
    QJsonArray responseArray = message["data"].toArray();
    userStatusMap.reserve(responseArray.size());
    for(const QJsonValue& elem : responseArray) {
        QJsonObject resObj = elem.toObject();
        const auto [userId, status] = std::tuple(
            resObj.value("id").toInt(-1),
            resObj.value("status").toBool()
        );

        if(userId != -1) {
            userStatusMap.emplace(userId, status);
        }
    }

    const int widgetCount = switchChats->count();
    for (int i = 0; i < widgetCount; ++i) {
        QWidget* widget = switchChats->widget(i);
        ChatContent* sender = qobject_cast<ChatContent*>(widget);
        if (sender) {
            // qDebug() << sender->user2Id << ": " << map[sender->user2Id];
            if(userStatusMap[sender->user2Id]) {
                sender->infoPanelChat->labelChatIndex->setText("🟢 В сети");
                sender->infoPanelChat->nameChat->popup->m_status = true;
            } else {
                sender->infoPanelChat->labelChatIndex->setText("⚪ Был(а) недавно");
                sender->infoPanelChat->nameChat->popup->m_status = false;
            }
        } else {
            // Виджет не является ChatContent. Обработка этого случая.
            qDebug() << "Виджет в индексе " << i << " не является ChatContent";
        }
    }
}

// Открытие поиска среди контактов/чатов
void SwitchChatContainer::openSearch() {
    // Фильтруем отображаемые кнопки по поисковой строке
    QString searchText = searchLine->text().toLower();
    for(int i = 0; i < buttonLayout->count(); ++i) {
        QWidget* widget = buttonLayout->itemAt(i)->widget();
        SwitchChatButton* button = qobject_cast<SwitchChatButton*>(widget);
        if(button) {
            QString btnText = button->m_chatName.toLower();
            if(!btnText.contains(searchText)) {
                button->hide();
                qDebug() << button->m_chatName;
            } else {
                button->show();
            }
        }
    }
}

// Открытие чата из контейнера кнопок
void SwitchChatContainer::showChat() {
    QObject* senderObj = sender();
    SwitchChatButton* button = qobject_cast<SwitchChatButton*>(senderObj);

    switchChats->setCurrentIndex(button->switchIndex);
}
