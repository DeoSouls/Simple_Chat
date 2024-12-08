#include "chatform.h"
#include <QPushButton>

ChatForm::ChatForm(QWidget *parent): QWidget{parent} {
    layoutChat = new QVBoxLayout(this);
    layoutChat->setAlignment(Qt::AlignCenter);

    btnToLogin = new QPushButton("Ok, get started");

    layoutChat->addWidget(btnToLogin);

    setLayout(layoutChat);
    setWindowTitle("SimpleChat");
}

ChatForm::~ChatForm() {
}
