#include "mainwindow.h"
#include "../ChatForm/chatform.h"
#include "../LoginForm/loginform.h"
#include <QPushButton>
#include <QMainWindow>
#include <QDebug>

Mainwindow::Mainwindow(QWidget* parent) : QWidget{parent} {

    LoginForm* login = new LoginForm();
    ChatForm* chat = new ChatForm();

    stackedWidget = new QStackedWidget();

    stackedWidget->addWidget(login);
    stackedWidget->addWidget(chat);

    layoutMain = new QVBoxLayout(this);
    layoutMain->addWidget(stackedWidget);
    layoutMain->setContentsMargins(0,0,0,0);

    connect(login->btnToChat, &QPushButton::clicked, [this, login]() {
        if(login->temporaryWidgets.isEmpty()) {
            stackedWidget->setCurrentIndex(1);
        } else {
            login->removeLogupForm();
        }
    });

    connect(login->btn2ToChat, &QPushButton::clicked, [this, login]() {
        if(!login->temporaryWidgets.isEmpty()) {
            stackedWidget->setCurrentIndex(1);
        } else {
            login->addLogupForm();
        }
    });

    connect(chat->btnToLogin, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(0);
    });

    setMinimumWidth(600);
    setWindowTitle("SimpleChat");
    resize(1050,550);
}


Mainwindow::~Mainwindow() {
}
