#include "loginbutton.h"

LoginButton::LoginButton(const QString text, QWidget* parent) : QPushButton{parent} {
    setText(text);
    setFixedSize(300, 28);
}

LoginButton::~LoginButton() {
}
