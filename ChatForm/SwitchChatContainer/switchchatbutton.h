#ifndef SWITCHCHATBUTTON_H
#define SWITCHCHATBUTTON_H

#include <QPushButton>

class SwitchChatButton : public QWidget {
        Q_OBJECT
    public:
        explicit SwitchChatButton(const QString& text, const QString& username,int index, int chatId, QWidget *parent = nullptr);

        int switchIndex;
        int chatId;
        QString text;
        QString m_chatName;
        QString m_userName;
        QString m_lastMessage;
        int m_unreadCount;

        void setTextButton(int unreadCount, const QString& lastMessage);
        void setChatName(const QString& chatName);
        void setLastMessage(int unreadCount, const QString& lastMessage, const QString& userName);
    signals:
        void clicked(int chatid);
    protected:
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
};

#endif // SWITCHCHATBUTTON_H
