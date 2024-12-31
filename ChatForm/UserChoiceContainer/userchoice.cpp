#include "userchoice.h"
#include "useritemdelegate.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QDebug>

UserChoice::UserChoice(int id, ChatForm* chatform, QStackedWidget* stackedWidget, QWidget *parent)
    : currentUserId(id), chatform(chatform), stackedWidget(stackedWidget), QWidget{parent} {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);
    setStyleSheet("QWidget { background-color: #222; }");

    setLayout(mainLayout);
}

void UserChoice::addMenuUser() {
    clearLayout(mainLayout);

    // Кнопка возвращения на главный экран
    QPushButton* returnMain = new QPushButton("<", this);
    returnMain->setStyleSheet("QPushButton { border: 0px; background-color: #222} "
                              "QPushButton::hover { background-color: #444}");
    returnMain->setFixedHeight(25);
    connect(returnMain, &QPushButton::clicked, this, &UserChoice::returnChat);

    userModel = new UserChoiceModel(this);

    // View для пользователей
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

    QSqlDatabase::database().transaction();
    QSqlQuery query;

    query.prepare("SELECT id, firstname, lastname, email FROM users WHERE users.id != :id");
    query.bindValue(":id", currentUserId);

    if(!query.exec()) {
        qDebug() << "Ошибка: "<< query.lastError().text();
        QSqlDatabase::database().rollback();
    }

    while(query.next()) {
        Users user;
        user.firstname = query.value("firstname").toString();
        user.lastname = query.value("lastname").toString();
        user.email = query.value("email").toString();
        user.user_id = query.value("id").toInt();

        userModel->addUser(user);
    }
    QSqlDatabase::database().commit();
}

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

void UserChoice::createChat(const QModelIndex &current) {
    if (!current.isValid()) return;

    QVariant data = current.data(UserChoiceModel::UserChatRole);
    Users user = data.value<Users>();

    QSqlQuery query;
    query.prepare("SELECT id FROM chats WHERE (user1_id=:user1_id AND user2_id=:user2_id) OR (user1_id=:user2_id AND user2_id=:user1_id)");
    query.bindValue(":user1_id", currentUserId);
    query.bindValue(":user2_id", user.user_id);

    if(!query.exec()) {
        qDebug() << "Ошибка создания чата: "<< query.lastError().text();
        QSqlDatabase::database().rollback();
    }

    if(!query.next()) {
        query.prepare("INSERT INTO chats (user1_id, user2_id) VALUES (:user1_id, :user2_id)");
        query.bindValue(":user1_id", currentUserId);
        query.bindValue(":user2_id", user.user_id);

        if(!query.exec()) {
            qDebug() << "Ошибка создания чата: "<< query.lastError().text();
            QSqlDatabase::database().rollback();
        }

        chatform->sContainer->addSwitchButtons();
    }
    stackedWidget->setCurrentIndex(1);
}

void UserChoice::returnChat() {
    stackedWidget->setCurrentIndex(1);
}
