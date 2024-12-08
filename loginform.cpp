#include "loginform.h"

LoginForm::LoginForm(QWidget* parent) : QWidget(parent) {

    layoutLogin = new QVBoxLayout(this);
    layoutLogin->setAlignment(Qt::AlignCenter);
    layoutLogin->setSpacing(16);

    btnToChat = new QPushButton("Log in");
    connect(btnToChat, SIGNAL(clicked()), SLOT(login()));

    QPushButton* btn2 = new QPushButton("Log up");

    layoutLogin->addWidget(btnToChat);
    layoutLogin->addWidget(btn2);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setLayout(layoutLogin);
    setWindowTitle("Log in");
    resize(500,400);
}

void LoginForm::login() {
}

LoginForm::~LoginForm() {
}
