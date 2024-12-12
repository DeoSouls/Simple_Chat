#include "switchchatbutton.h"

SwitchChatButton::SwitchChatButton(const QString text, int index, QWidget *parent) : QPushButton{parent}, switchIndex(index) {
    setText(text);
    setFixedHeight(50);
    setStyleSheet("QPushButton {text-align: left;border-radius: 0; "
                  "border-top: 1px solid #444; background-color: #222; "
                  "color: white; padding-left: 35px}"
                  "QPushButton::hover {background-color: #444;}");
}
