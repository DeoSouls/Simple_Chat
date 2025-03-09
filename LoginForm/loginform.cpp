#include "loginform.h"
#include <QLabel>

LoginForm::LoginForm(QWidget* parent) : QWidget{parent} {
    // Выравнивание всего содержимого по вертикали и его центрирование
    layoutLogin = new QVBoxLayout(this);
    layoutLogin->setAlignment(Qt::AlignCenter);
    layoutLogin->setSpacing(6);

    // Контейнер для динамических компонентов
    containedLayout = new QVBoxLayout(this);
    font = QFont("Segoe UI", 11, 300);

    // Поля для ввода
    mailLine = new QLineEdit();
    mailLine->setFont(font);
    mailLine->setFixedSize(300, 25);
    passLine = new QLineEdit();
    passLine->setFont(font);
    passLine->setFixedSize(300, 25);

    // Заголовок макета
    headerLogin = new QLabel("Sign in");
    headerLogin->setFont(QFont("Segoe UI", 18, 800));
    headerLogin->setStyleSheet("QLabel { margin-bottom: 25px; }");
    headerLogin->setAlignment(Qt::AlignCenter);

    // Заголовки полей ввода
    QLabel* mailLabel = new QLabel("Email or login:");
    mailLabel->setFont(font);
    QLabel* passLabel = new QLabel("Password:");
    passLabel->setFont(font);

    // Кастомная кнопка перехода между страницами/формами
    btnToChat = new LoginButton("Log in");
    btnToChat->setFont(font);
    connect(btnToChat, &LoginButton::clicked,this, &LoginForm::removeLogupForm);
    btn2ToChat = new LoginButton("Log up");
    btn2ToChat->setFont(font);

    // Добавление виджетов в основной макет авторизации/регистрации
    layoutLogin->addWidget(headerLogin);
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
}

// Динамическое добавление полей регистрации
void LoginForm::addLogupForm() {
    if(temporaryWidgets.empty()) {
        firstNameLine = new QLineEdit();
        lastNameLine = new QLineEdit();
        firstNameLine->setFixedSize(300,25);
        lastNameLine->setFixedSize(300,25);

        QLabel* firstNameLabel = new QLabel("Firstname:");
        firstNameLabel->setFont(font);
        QLabel* lastNameLabel = new QLabel("Lastname:");
        lastNameLabel->setFont(font);

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

// Динамическое удаление полей регистрации
void LoginForm::removeLogupForm() {
    if(!temporaryWidgets.empty()) {
        for(QWidget* widget : temporaryWidgets) {
            containedLayout->removeWidget(widget);
            widget->deleteLater();
        }
        temporaryWidgets.clear();
    }
}
