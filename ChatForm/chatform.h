#ifndef CHATFORM_H
#define CHATFORM_H

#include "SwitchChatContainer/switchchatcontainer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QWebSocket>
#include <QPropertyAnimation>

class ChatForm : public QWidget {
        Q_OBJECT
    public:
        explicit ChatForm(int userId, QString userFirstname, QString userLastname, QWebSocket* m_client, QWidget *parent = nullptr);

        int userId;
        QString userFirstname;
        QString userLastname;
        QWebSocket* m_socket;
        SwitchChatContainer* sContainer;
        QPushButton* exitFromChat;
        QPushButton* startChatting;

        void clearLayout(QLayout *layout);
        QHBoxLayout* layoutChat;
    private slots:
        void toggleMenu();
    private:
        QPropertyAnimation* animation;
        QFrame* menu;

    protected:
        void resizeEvent(QResizeEvent* event) override;
};

#endif // CHATFORM_H
