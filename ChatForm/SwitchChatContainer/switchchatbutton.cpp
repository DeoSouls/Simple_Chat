#include "switchchatbutton.h"

SwitchChatButton::SwitchChatButton(const QString text, int index, int chatId, QWidget *parent)
    : QPushButton{parent}, text(text), switchIndex(index), chatId(chatId) {
    setText(text);
    setFixedHeight(70);
    setFont(QFont("Helvetica", 11, 200));
    setStyleSheet("QPushButton {text-align: left;border-radius: 0; "
                  "border-bottom: 1px solid #444; background-color: #222; "
                  "color: white; padding-left: 20px; padding-top: 3px;"
                  "border-right: 1px solid #333; line-height: 0.5;}"
                  "QPushButton::hover {background-color: #444;}");
}

void SwitchChatButton::setTextButton(int unreadCount, const QString& lastMessage) {
    QString textBut = text;
    if (unreadCount > 0) {
        textBut += QString(" (%1)").arg(unreadCount);
    }
    if (!lastMessage.isEmpty()) {
        textBut += "\n\n" + lastMessage;
    }
    this->setText(textBut);
}

