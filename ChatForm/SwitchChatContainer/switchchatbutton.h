#ifndef SWITCHCHATBUTTON_H
#define SWITCHCHATBUTTON_H

#include <QPushButton>

class SwitchChatButton : public QPushButton {
        Q_OBJECT
    public:
        explicit SwitchChatButton(const QString text, int index, int chatId, QWidget *parent = nullptr);

        int switchIndex;
        int chatId;
        const QString text;

        void setTextButton(int unreadCount, const QString& lastMessage);
};

#endif // SWITCHCHATBUTTON_H
