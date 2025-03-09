#include "userchoice.h"
#include "useritemdelegate.h"
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

UserChoice::UserChoice(int id, ChatForm* chatform, QWebSocket* m_client, QStackedWidget* stackedWidget, QWidget *parent)
    : currentUserId(id), chatform(chatform), m_socket(m_client), stackedWidget(stackedWidget), QWidget{parent} {

    // Создаем вертикальный layout для размещения виджетов
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);
    setStyleSheet("QWidget { background-color: #222; }");

    // Подключаем обработчик входящих сообщений от сервера
    connect(m_socket, &QWebSocket::textMessageReceived, this, &UserChoice::handlerCommand);

    setLayout(mainLayout);
}

// Обработчик команд от сервера
void UserChoice::handlerCommand(const QString& message) {
    // Парсим JSON-сообщение
    QJsonDocument response_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response_obj = response_doc.object();
    QString type = response_obj.value("type").toString();

    // Обработка ошибки выбора пользователя
    if(type.contains("error_choice")) {
        QMessageBox::critical(this, "Ошибка: ", response_obj.value("message").toString());
        return;
    }

    // Добавление пользователей в список
    if(type.contains("add_users")) {
        addUsers(response_obj);
    }
}

// Функция добавления меню выбора пользователей
void UserChoice::addMenuUser() {
    clearLayout(mainLayout);

    // Кнопка возвращения на главный экран
    QPushButton* returnMain = new QPushButton("<", this);
    returnMain->setStyleSheet("QPushButton { border: 0px; background-color: #222} "
                              "QPushButton::hover { background-color: #444}");
    returnMain->setFixedHeight(25);
    connect(returnMain, &QPushButton::clicked, this, &UserChoice::returnChat);

    // Создание модели и представления списка пользователей
    userModel = new UserChoiceModel(this);

    // Использование делегата для кастомного отображения элементов списка
    userView = new QListView(this);
    userView->setModel(userModel);
    userView->setItemDelegate(new UserItemDelegate(this));

    userView->setStyleSheet("QListView { border: 0px; background-color: #222; border-bottom: 1px solid #333;} "
                            "QListView::item:hover { background-color: #444;}");
    userView->setSpacing(5);
    userView->setUniformItemSizes(false);
    userView->setResizeMode(QListView::Adjust);
    userView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    userView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    userView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    userView->setContentsMargins(0, 0, 0, 0);
    userView->scrollToTop();
    connect(userView, &QListView::clicked, this, &UserChoice::createChat);

    mainLayout->addWidget(returnMain);
    mainLayout->addWidget(userView);

    // Запрос списка пользователей у сервера
    QJsonObject mesObj;
    mesObj["action"] = "add_users";
    mesObj["userid"] = currentUserId;
    QJsonDocument mesDoc(mesObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);
}

// Метод добавления пользователей в модель списка из JSON-ответа сервера
void UserChoice::addUsers(const QJsonObject& message) {
    QJsonArray messageArray = message["data"].toArray();

    for(const QJsonValue& value : messageArray) {
        QJsonObject mesObj = value.toObject();

        Users user;
        user.firstname = mesObj.value("firstname").toString();
        user.lastname = mesObj.value("lastname").toString();
        user.email = mesObj.value("email").toString();
        user.user_id = mesObj.value("id").toInt();

        userModel->addUser(user);
    }
}

// Очистка текущего макета перед добавлением новых виджетов
void UserChoice::clearLayout(QLayout *layout) {
    if (!layout)
        return;

    // Удаляем все виджеты из макета
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
    }
}

// Создание нового чата при выборе пользователя из списка
void UserChoice::createChat(const QModelIndex &current) {
    if (!current.isValid()) return;

    QVariant data = current.data(UserChoiceModel::UserChatRole);
    Users user = data.value<Users>();

    // Формируем JSON-запрос на сервер для создания нового чата
    QJsonObject messageObj;
    messageObj["action"] = "create_chat";
    messageObj["userid"] = currentUserId;
    messageObj["user2id"] = user.user_id;
    QJsonDocument mesDoc(messageObj);

    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);

    QJsonObject messageObj2;
    messageObj2["action"] = "add_chats";
    messageObj2["userid"] = currentUserId;
    QJsonDocument mesDoc2(messageObj2);

    QString jsonString2 = mesDoc2.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString2);

    stackedWidget->setCurrentIndex(1); // переключаем экран на чат после создания
}

// Возврат на основной экран чата
void UserChoice::returnChat() {
    stackedWidget->setCurrentIndex(1);
}
