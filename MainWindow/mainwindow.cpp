#include "mainwindow.h"
#include "../ChatForm/chatform.h"
#include "../LoginForm/loginform.h"
#include "../ChatForm/UserChoiceContainer/userchoice.h"
#include <QPushButton>
#include <QWebSocket>
#include <QMessageBox>
#include <QDebug>

Mainwindow::Mainwindow(const QUrl &url, QWidget* parent) : QWidget{parent} {
    m_socket = new QWebSocket();
    connect(m_socket, &QWebSocket::connected, this, []() {
        qDebug() << "Подключено к серверу.";
    });
    connect(m_socket, &QWebSocket::disconnected, this, []() {
        qDebug() << "Соединение разорвано.";
    });
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, [this](QAbstractSocket::SocketError error) {
        QString errorMsg = "Ошибка WebSocket: " + m_socket->errorString();
        qDebug() << errorMsg;
        showError(errorMsg);
    });

    LoginForm* login = new LoginForm();

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(login);

    layoutMain = new QVBoxLayout(this);
    layoutMain->addWidget(stackedWidget);
    layoutMain->setContentsMargins(0,0,0,0);

    if (!QSqlDatabase::database().isOpen()) {
        showError("Не удалось подключиться к базе данных.");
        return;
    }

    connect(login->btnToChat, &QPushButton::clicked, [this, login]() {
        onLoginClicked(login);
    });

    connect(login->btn2ToChat, &QPushButton::clicked, [this, login]() {
        onLogupClicked(login);
    });

    m_socket->open(url);
    setMinimumWidth(640);
    setMinimumHeight(480);
    setWindowTitle("SimpleChat");
    resize(1050,550);
}

QString Mainwindow::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

QSqlQuery Mainwindow::executeQuery(const QString& queryStr, const QVariantMap& params) {
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(queryStr);
    for (auto it = params.begin(); it != params.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        showError(query.lastError().text());
    }
    return query;
}

void Mainwindow::switchToWidget(QWidget* widget) {
    if (stackedWidget->currentWidget() != widget) {
        stackedWidget->setCurrentWidget(widget);
    }
}

void Mainwindow::onLoginClicked(LoginForm* login) {
    if(!login->temporaryWidgets.isEmpty()) {
        login->removeLogupForm();
        return;
    }

    if (login->mailLine->text().isEmpty() || login->passLine->text().isEmpty()) {
        showError("Пожалуйста, заполните все поля.");
        return;
    }

    QString password = login->passLine->text();

    QSqlQuery query = executeQuery("SELECT id, firstname, lastname, email, password FROM users WHERE email = :email",
                                   {{":email", login->mailLine->text()}});

    if(query.next()) {
        int userid = query.value("id").toInt();
        QString userFirstname = query.value("firstname").toString();
        QString userLastname = query.value("lastname").toString();
        QString userPassword = query.value("password").toString();

        if(userPassword == hashPassword(password)) {
            setupChatInterface(userid, userFirstname, userLastname);
        }
    }
    login->mailLine->clear();
    login->passLine->clear();
}

void Mainwindow::onLogupClicked(LoginForm* login) {
    if(login->temporaryWidgets.isEmpty()) {
        login->addLogupForm();
        return;
    }

    if (login->mailLine->text().isEmpty() || login->passLine->text().isEmpty() ||
        login->firstNameLine->text().isEmpty() || login->lastNameLine->text().isEmpty()) {
        showError("Пожалуйста, заполните все поля.");
        return;
    }

    QString hashPass = hashPassword(login->passLine->text());

    QSqlQuery query = executeQuery("INSERT INTO users (firstname, lastname, email, password) "
                                   "VALUES (:firstname, :lastname, :email, :password) RETURNING id, firstname, lastname",
                                   {
                                       {":firstname", login->firstNameLine->text()},
                                       {":lastname", login->lastNameLine->text()},
                                       {":email", login->mailLine->text()},
                                       {":password", hashPass}
                                   });
    if (query.next()) {
        int userid = query.value("id").toInt();
        QString userFirstname = query.value("firstname").toString();
        QString userLastname = query.value("lastname").toString();

        setupChatInterface(userid, userFirstname, userLastname);
    }

    login->firstNameLine->clear();
    login->lastNameLine->clear();
    login->mailLine->clear();
    login->passLine->clear();
}

void Mainwindow::setupChatInterface(int userid, const QString& firstname, const QString& lastname) {
    ChatForm* chat = new ChatForm(userid, firstname, lastname, m_socket, stackedWidget);
    stackedWidget->addWidget(chat);

    UserChoice* choice = new UserChoice(userid, chat, stackedWidget);
    stackedWidget->addWidget(choice);

    connect(chat->startChatting, &QPushButton::clicked, [this, choice]() {
        switchToWidget(choice);
        choice->addMenuUser();
    });

    connect(chat->exitFromChat, &QPushButton::clicked, [this, chat, choice]() {
        switchToWidget(stackedWidget->widget(0));
        stackedWidget->removeWidget(chat);
        stackedWidget->removeWidget(choice);
        chat->deleteLater();
    });

    switchToWidget(chat);
}

void Mainwindow::showError(const QString& message) {
    QMessageBox::critical(this, "Ошибка", message);
}
