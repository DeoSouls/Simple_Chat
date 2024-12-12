#ifndef CHATFORM_H
#define CHATFORM_H

#include "SwitchChatContainer/switchchatcontainer.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QPropertyAnimation>

class ChatForm : public QWidget {
        Q_OBJECT
    public:
        explicit ChatForm(QWidget *parent = nullptr);
        QPushButton* btnToLogin;

        ~ChatForm();
    private slots:
        void toggleMenu();
    private:
        SwitchChatContainer* sContainer;
        QHBoxLayout* layoutChat;
        QPropertyAnimation* animation;
        QFrame* menu;
    protected:
        void resizeEvent(QResizeEvent* event) override;
};

#endif // CHATFORM_H
