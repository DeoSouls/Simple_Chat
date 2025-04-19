#include "mainwindow.h"
#include "../ChatForm/chatform.h"
#include "../LoginForm/loginform.h"
#include "../ChatForm/UserChoiceContainer/userchoice.h"
#include <QRandomGenerator>
#include <QByteArray>
#include <QPushButton>
#include <QWebSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

Mainwindow::Mainwindow(const QUrl& url, QWidget* parent) : QWidget{parent}, url(url) {
    // Подключение к серверу
    connectToServer(url);

    connect(m_socket, &QWebSocket::textMessageReceived, this, &Mainwindow::handlerLogin);
    LoginForm* login = new LoginForm();

    // Добавляем функцию страницы к чату
    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(login);

    // Выравнивание основного окна по вертикали
    layoutMain = new QVBoxLayout(this);
    layoutMain->addWidget(stackedWidget);
    layoutMain->setContentsMargins(0,0,0,0);

    // Переключатель на авторизацию
    connect(login->btnToChat, &QPushButton::clicked, [this, login]() {
        onLoginClicked(login);
    });

    // Переключатель на регистрацию
    connect(login->btn2ToChat, &QPushButton::clicked, [this, login]() {
        onLogupClicked(login);
    });

    setMinimumWidth(640);
    setMinimumHeight(480);
    setWindowTitle("SimpleChat");
    resize(1050,550);
}

// Обработчик ответов из сервера
void Mainwindow::handlerLogin(const QString& message) {
    QJsonDocument response_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response_obj = response_doc.object();
    QString type = response_obj.value("type").toString();

    // Обработчик ошибок от сервера
    if(type.contains("error_login")) {
        showError(response_obj.value("message").toString());
        return;
    }

    // Успешный ответ от сервера устанавливает интерфейс чата
    if(type.contains("login") || type.contains("register")) {
        setupChatInterface(response_obj);
    }
}

// Создание криптографического пароля SHA256
// Будет хранится в базе данных
QString Mainwindow::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// Переключатель страниц для перехода на не текущую страницу
void Mainwindow::switchToWidget(QWidget* widget) {
    if (stackedWidget->currentWidget() != widget) {
        stackedWidget->setCurrentWidget(widget);
    }
}

// Отправка данных неавторизованных пользователей
// Логика переключения формы на авторизацию
void Mainwindow::onLoginClicked(LoginForm* login) {
    if(!login->temporaryWidgets.isEmpty()) {
        login->removeLogupForm();
        return;
    }

    if (login->mailLine->text().isEmpty() || login->passLine->text().isEmpty()) {
        showError("Пожалуйста, заполните все поля.");
        login->headerLogin->setText("Sign in");
        return;
    }

    QString password = hashPassword(login->passLine->text());

    QJsonObject messageObj;
    messageObj["action"] = "login";
    messageObj["email"] = login->mailLine->text();
    messageObj["password"] = password;

    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);

    login->mailLine->clear();
    login->passLine->clear();
}

// Подключение к серверу с приостановкой цикла событий до завершения подключения
void Mainwindow::connectToServer(const QUrl& url) {
    QEventLoop eloop;
    m_socket = new QWebSocket();
    connect(m_socket, &QWebSocket::connected, &eloop, [&eloop]() {
        qDebug() << "Подключено к серверу.";
        eloop.quit();
    });
    connect(m_socket, &QWebSocket::disconnected, &eloop, [&eloop]() {
        qDebug() << "Соединение разорвано.";
        eloop.quit();
    });
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, [this](QAbstractSocket::SocketError error) {
        QString errorMsg = "Ошибка WebSocket: " + m_socket->errorString();
        qDebug() << errorMsg;
        showError(errorMsg);
    });

    m_socket->open(url);

    eloop.exec();
}

// Отправка данных незарегистрированных пользователей
// Логика переключения формы на регистрацию
void Mainwindow::onLogupClicked(LoginForm* login) {
    if(login->temporaryWidgets.isEmpty()) {
        login->addLogupForm();
        login->headerLogin->setText("Sign up");
        return;
    }

    if (login->mailLine->text().isEmpty() || login->passLine->text().isEmpty() ||
        login->firstNameLine->text().isEmpty() || login->lastNameLine->text().isEmpty()) {
        showError("Пожалуйста, заполните все поля.");
        return;
    }

    QString hashPass = hashPassword(login->passLine->text());

    QJsonObject messageObj;
    messageObj["action"] = "register";
    messageObj["firstname"] = login->firstNameLine->text();
    messageObj["lastname"] = login->lastNameLine->text();
    messageObj["email"] = login->mailLine->text();
    messageObj["password"] = hashPass;

    QJsonDocument mesDoc(messageObj);
    QString jsonString = mesDoc.toJson(QJsonDocument::Compact);

    m_socket->sendTextMessage(jsonString);

    login->firstNameLine->clear();
    login->lastNameLine->clear();
    login->mailLine->clear();
    login->passLine->clear();
}

// Установка интерфейса чата
void Mainwindow::setupChatInterface(const QJsonObject& message) {
    int userid = message.value("userid").toInt();
    QString firstname = message.value("firstname").toString();
    QString lastname = message.value("lastname").toString();

    ChatForm* chat = new ChatForm(userid, firstname, lastname, m_socket, stackedWidget);
    stackedWidget->addWidget(chat);

    UserChoice* choice = new UserChoice(userid, chat, m_socket, stackedWidget);
    stackedWidget->addWidget(choice);

    // Ежеминутная проверка авторизованность пользователя
    QTimer* timer = new QTimer(this);
    timer->setInterval(10000);
    connect(timer, &QTimer::timeout, this, [this, chat, userid]() {
        QJsonObject reqObj;
        reqObj["action"] = "update_user";
        reqObj["userid"] = userid;

        QJsonDocument resDoc(reqObj);
        QString jsonString = resDoc.toJson(QJsonDocument::Compact);
        m_socket->sendTextMessage(jsonString);
    });
    timer->start();

    // Авторизация пользователя и вход в чат
    connect(chat->startChatting, &QPushButton::clicked, [this, choice]() {
        switchToWidget(choice);
        choice->addMenuUser();
    });

    // Выход из чата
    connect(chat->exitFromChat, &QPushButton::clicked, [this, chat, choice]() {
        switchToWidget(stackedWidget->widget(0));
        stackedWidget->removeWidget(chat);
        stackedWidget->removeWidget(choice);
        chat->deleteLater();

        m_socket->close();
        m_socket->open(url);
    });

    switchToWidget(chat);
}

// Обработчик ошибок
void Mainwindow::showError(const QString& message) {
    QMessageBox::critical(this, "Ошибка", message);
}
