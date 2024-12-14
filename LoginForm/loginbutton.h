#ifndef LOGINBUTTON_H
#define LOGINBUTTON_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class LoginButton : public QPushButton {
        Q_OBJECT
    public:
        explicit LoginButton(const QString text, QWidget* parent = nullptr);

        ~LoginButton();
};

#endif // LOGINBUTTON_H
