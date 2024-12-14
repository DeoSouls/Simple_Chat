#include "mainwindow.h"
#include "../ChatForm/chatform.h"
#include "../LoginForm/loginform.h"
#include <QPushButton>
#include <QDebug>

Mainwindow::Mainwindow(QWidget* parent) : QWidget{parent} {

    LoginForm* login = new LoginForm();

    stackedWidget = new QStackedWidget();

    stackedWidget->addWidget(login);

    layoutMain = new QVBoxLayout(this);
    layoutMain->addWidget(stackedWidget);
    layoutMain->setContentsMargins(0,0,0,0);

    connect(login->btnToChat, &QPushButton::clicked, [this, login]() {
        if(login->temporaryWidgets.isEmpty()) {
            QSqlDatabase::database().transaction();
            QSqlQuery query;

            query.prepare("SELECT id, firstname, lastname, email, password FROM users WHERE email = :email");

            QString password = login->passLine->text();
            query.bindValue(":email", login->mailLine->text());

            if (!query.exec()) {
                qDebug() << "Ошибка авторизации:" << query.lastError().text();
                QSqlDatabase::database().rollback(); // Откат транзакции
            } else {
                if(query.next()) {
                    int userid = query.value("id").toInt();
                    QString userFirstname = query.value("firstname").toString();
                    QString userLastname = query.value("lastname").toString();
                    QString userPassword = query.value("password").toString();
                    qDebug() << "Пользователь найден! ID:" << userid
                             << "Имя:" << userFirstname
                             << "Фамилия:" << userLastname;
                    if(userPassword == password) {
                        qDebug() << "Пароли совпадают!";
                        ChatForm* chat = new ChatForm(userid, userFirstname, userLastname);
                        stackedWidget->addWidget(chat);
                        connect(chat->exitFromChat, &QPushButton::clicked, [this, chat]() {
                            stackedWidget->setCurrentIndex(0);
                            stackedWidget->removeWidget(chat);
                            chat->deleteLater();
                        });
                    }
                }
            }
            QSqlDatabase::database().commit();
            login->mailLine->clear();
            login->passLine->clear();
            stackedWidget->setCurrentIndex(1);
        } else {
            login->removeLogupForm();
        }
    });

    connect(login->btn2ToChat, &QPushButton::clicked, [this, login]() {
        if(!login->temporaryWidgets.isEmpty()) {
            QSqlDatabase::database().transaction();
            QSqlQuery query;

            // Подготовка SQL-запроса
            query.prepare("INSERT INTO users (firstname, lastname, email, password) "
                          "VALUES (:firstname, :lastname, :email, :password) RETURNING id, firstname, lastname");

            query.bindValue(":firstname", login->firstNameLine->text());
            query.bindValue(":lastname", login->lastNameLine->text());
            query.bindValue(":email", login->mailLine->text());
            query.bindValue(":password", login->passLine->text());

            if (!query.exec()) {
                qDebug() << "Ошибка регистрации:" << query.lastError().text();
                QSqlDatabase::database().rollback(); // Откат транзакции
            } else {
                if (query.next()) {
                    int userid = query.value("id").toInt();
                    QString userFirstname = query.value("firstname").toString();
                    QString userLastname = query.value("lastname").toString();
                    ChatForm* chat = new ChatForm(userid, userFirstname, userLastname);

                    qDebug() << "Пользователь добавлен! ID:" << userid
                             << "Имя:" << userFirstname
                             << "Фамилия:" << userLastname;
                    stackedWidget->addWidget(chat);
                    connect(chat->exitFromChat, &QPushButton::clicked, [this, chat]() {
                        stackedWidget->setCurrentIndex(0);
                        stackedWidget->removeWidget(chat);
                        chat->deleteLater();
                    });
                }
                QSqlDatabase::database().commit(); // Подтверждение транзакции
                login->firstNameLine->clear();
                login->lastNameLine->clear();
                login->mailLine->clear();
                login->passLine->clear();
                stackedWidget->setCurrentIndex(1);
            }
        } else {
            login->addLogupForm();
        }
    });

    setMinimumWidth(600);
    setWindowTitle("SimpleChat");
    resize(1050,550);
}


Mainwindow::~Mainwindow() {
}
