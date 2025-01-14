#include "userchoice.h"
#include <QSqlDatabase>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

userChoice::userChoice(int id, QWidget *parent)
    : currentUserId(id), QWidget{parent} {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setAlignment(Qt::AlignCenter);
    setStyleSheet("QWidget { background-color: #333; }");

    setLayout(mainLayout);
}

void userChoice::addMenuUser() {
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    query.prepare("SELECT firstname, lastname, email FROM users WHERE users.id != :id");
    query.bindValue(":id", currentUserId);

    if(!query.exec()) {
        qDebug() << "Ошибка: "<< query.lastError().text();
        QSqlDatabase::database().rollback();
    }

    while(query.next()) {
        QPushButton* userContainer = new QPushButton();
        userContainer->setFixedHeight(60);
        userContainer->setText(query.value("firstname").toString()+" "
                               +query.value("lastname").toString()+"\n"
                               +query.value("email").toString());
        mainLayout->addWidget(userContainer);
    }
    QSqlDatabase::database().commit();
}

void userChoice::createChat() {
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO chats (user_id) VALUES (:user_id)")
}

userChoice::~userChoice() {
}
