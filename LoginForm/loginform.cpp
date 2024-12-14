#include "loginform.h"
#include <QLabel>

LoginForm::LoginForm(QWidget* parent) : QWidget{parent} {

    layoutLogin = new QVBoxLayout(this);
    layoutLogin->setAlignment(Qt::AlignCenter);
    layoutLogin->setSpacing(6);

    containedLayout = new QVBoxLayout(this);

    mailLine = new QLineEdit();
    passLine = new QLineEdit();
    mailLine->setFixedSize(300, 25);
    passLine->setFixedSize(300, 25);

    QLabel* mailLabel = new QLabel("Email or login:");
    QLabel* passLabel = new QLabel("Password:");

    btnToChat = new LoginButton("Log in");
    connect(btnToChat, SIGNAL(clicked()), SLOT(removeLogupForm()));

    btn2ToChat = new LoginButton("Log up");

    layoutLogin->addLayout(containedLayout);
    layoutLogin->addWidget(mailLabel);
    layoutLogin->addWidget(mailLine);

    layoutLogin->addWidget(passLabel);
    layoutLogin->addWidget(passLine);
    layoutLogin->addSpacing(15);
    layoutLogin->addWidget(btnToChat);
    layoutLogin->addWidget(btn2ToChat);

    setAttribute(Qt::WA_DeleteOnClose, true);
    setLayout(layoutLogin);

    connectToBaseData();
}

void LoginForm::connectToBaseData() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("SimpleChat");
    db.setUserName("postgres");
    db.setPassword("11281215");

    if(!db.open()) {
        qDebug() << "Error connectiong database: " << db.lastError().text();
    } else {
        qDebug() << "Connected to database!";
    }
}

void LoginForm::addLogupForm() {
    if(temporaryWidgets.empty()) {
        firstNameLine = new QLineEdit();
        lastNameLine = new QLineEdit();
        firstNameLine->setFixedSize(300,25);
        lastNameLine->setFixedSize(300,25);

        QLabel* firstNameLabel = new QLabel("Firstname:");
        QLabel* lastNameLabel = new QLabel("Lastname:");

        containedLayout->addWidget(firstNameLabel);
        containedLayout->addWidget(firstNameLine);

        containedLayout->addWidget(lastNameLabel);
        containedLayout->addWidget(lastNameLine);

        temporaryWidgets.push_back(firstNameLine);
        temporaryWidgets.push_back(lastNameLine);
        temporaryWidgets.push_back(firstNameLabel);
        temporaryWidgets.push_back(lastNameLabel);
    }
}

void LoginForm::removeLogupForm() {
    if(!temporaryWidgets.empty()) {
        for(QWidget* widget : temporaryWidgets) {
            containedLayout->removeWidget(widget);
            widget->deleteLater();
        }
        temporaryWidgets.clear();
    }
}

void LoginForm::login() {
}

LoginForm::~LoginForm() {
}
