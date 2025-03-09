#include "loginbutton.h"

// Кастомная кнопка с жесткими границами
LoginButton::LoginButton(const QString text, QWidget* parent) : QPushButton{parent} {
    setText(text);
    setFixedSize(300, 28);
}
