#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class LoginForm : public QWidget {
        Q_OBJECT
    public:
        explicit LoginForm(QWidget* parent = nullptr);
        ~LoginForm();
        QPushButton* btnToChat;
    private slots:
        void login();
    private:
        QVBoxLayout* layoutLogin;

};

#endif // LOGINFORM_H
