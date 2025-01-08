#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include "LoginButton.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QLabel>

class LoginForm : public QWidget {
        Q_OBJECT
    public:
        explicit LoginForm(QWidget* parent = nullptr);
        QList<QWidget*> temporaryWidgets;
        LoginButton* btn2ToChat;
        LoginButton* btnToChat;

        QLineEdit* firstNameLine;
        QLineEdit* lastNameLine;
        QLineEdit* mailLine;
        QLineEdit* passLine;

        QLabel* headerLogin;
        void removeLogupForm();

        ~LoginForm();
    public slots:
        void addLogupForm();
    private slots:
        void login();
    private:
        QVBoxLayout* layoutLogin;
        QVBoxLayout* containedLayout;
        QFont font;
};

#endif // LOGINFORM_H
