#include "mainwindow.h"
#include "chatform.h"
#include "loginform.h"
#include <QPushButton>

Mainwindow::Mainwindow(QWidget* parent) : QWidget{parent} {

    LoginForm* login = new LoginForm();
    ChatForm* chat = new ChatForm();

    stackedWidget = new QStackedWidget();

    stackedWidget->addWidget(login);
    stackedWidget->addWidget(chat);

    layoutMain = new QVBoxLayout(this);
    layoutMain->addWidget(stackedWidget);

    connect(login->btnToChat, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(1);
    });

    connect(chat->btnToLogin, &QPushButton::clicked, [this]() {
        stackedWidget->setCurrentIndex(0);
    });

    resize(500,400);
}

Mainwindow::~Mainwindow() {
}
