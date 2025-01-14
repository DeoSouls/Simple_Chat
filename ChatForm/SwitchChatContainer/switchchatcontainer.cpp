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

    connect(m_socket, &QWebSocket::textMessageReceived, this, &SwitchChatContainer::updateHandler);

    QJsonObject messageObj;
    messageObj["action"] = "add_chats";
    messageObj["userid"] = userId;
    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);

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

    searchLine = new QLineEdit();
    searchLine->setFixedHeight(30);
    searchLine->setFont(QFont("Segoe UI", 12, 300));
    searchLine->setPlaceholderText("Введите текст...");
    panelLayout->addWidget(searchLine,1);

    searchOnTopPanel = new QPushButton();
    searchOnTopPanel->setFixedSize(30,30);
    searchOnTopPanel->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconSearch.png"));
    searchOnTopPanel->setIconSize(QSize(17,17));
    panelLayout->addWidget(searchOnTopPanel);
    connect(searchOnTopPanel, &QPushButton::clicked, this, &SwitchChatContainer::openSearch);

    toggleButton = new QPushButton();
    toggleButton->setFixedSize(30,30);
    toggleButton->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconsMenu.png"));
    toggleButton->setIconSize(QSize(17,17));
    panelLayout->addWidget(toggleButton);

    switchChats = new QStackedWidget(this);

    QWidget* startChat = new QWidget(this);
    QVBoxLayout* layoutStartChat = new QVBoxLayout(startChat);
    layoutStartChat->setAlignment(Qt::AlignCenter);
    startChat->setContentsMargins(0,0,0,0);

    QLabel* startChatContent = new QLabel("Select chat or start a conversation with a new contact! ✨");
    startChatContent->setFont(QFont("Segoe UI", 12, 300));
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
    buttonLayout->addWidget(topPanel);

    // Связываем контейнер с QScrollArea
    buttonContainer->setLayout(buttonLayout);
    scrollArea->setWidget(buttonContainer);

    // Добавляем QScrollArea в switchLayout
    switchLayout->addWidget(scrollArea);
    setLayout(switchLayout);
}

void SwitchChatContainer::updateHandler(const QString& message) {
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
    }
}

void SwitchChatContainer::addSwitchButtons(const QJsonObject& message) {
    QJsonArray responseArray;
    responseArray = message["data"].toArray();

    for(const QJsonValue& value : responseArray) {
        QJsonObject responseObj = value.toObject();
        int index = switchChats->count();
        int chatId = responseObj["chatId"].toInt();
        QString email = responseObj["email"].toString();
        QString userName = responseObj["userName"].toString();
        QString lastMessage = responseObj["lastMessage"].toString();
        bool status = responseObj["status"].toBool();

        if (!switchButtons.contains(chatId)) {
            auto button = new SwitchChatButton(userName, switchButtons.size() + 1, chatId, this);
            switchButtons[chatId] = button;

            button->setLastMessage(0,lastMessage);
            buttonLayout->addWidget(button); // Добавляем кнопку в buttonLayout

            auto chatContent = new ChatContent(userName, email, status, chatId, userId, m_socket, this);
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

void SwitchChatContainer::updateButtons(const QJsonObject& message) {
    QJsonArray responseArray = message["data"].toArray();
    for(const QJsonValue& value : responseArray) {
        QJsonObject messageObj = value.toObject();

        QString lastMessage = messageObj.value("lastMessage").toString();
        int unreadCount = messageObj.value("unreadCount").toInt();
        int chatid = messageObj.value("chatid").toInt();

        if (switchButtons.contains(chatid)) {
            SwitchChatButton* button = switchButtons.value(chatid);
            if (switchChats->currentIndex() == button->switchIndex) { // Текущий активный чат

                button->setLastMessage(0, lastMessage);
                QJsonObject mesObj;
                mesObj["action"] = "update_stat";
                mesObj["userid"] = userId;
                mesObj["chatid"] = chatid;

                QJsonDocument messageDoc(mesObj);
                QString jsonString = messageDoc.toJson(QJsonDocument::Compact);
                m_socket->sendTextMessage(jsonString);
            } else {
                button->setLastMessage(unreadCount, lastMessage); // Обновляем количество непрочитанных сообщений
            }
        }
    }
}

void SwitchChatContainer::openSearch() {
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

void SwitchChatContainer::showChat() {
    QObject* senderObj = sender();
    SwitchChatButton* button = qobject_cast<SwitchChatButton*>(senderObj);

    switchChats->setCurrentIndex(button->switchIndex);
}
