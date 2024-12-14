#ifndef CHATCONTENT_H
#define CHATCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QList>
#include <QScrollArea>
#include "InputMessage/inputmessage.h"
#include "MessageContainer/messagecontainer.h"

class ChatContent : public QWidget {
        Q_OBJECT
    public:
        explicit ChatContent(const QString case1, int index, int id, QWidget *parent = nullptr);

        int userId;
        int chatIndex;

        void addMessageToChat();

        ~ChatContent();
    private:
        QList<QString> messages;
        QList<QString> currentUserMesssages;
        QList<QWidget*> messagesContainers;
        QVBoxLayout* chatContentLayout;
        QVBoxLayout* outputFieldLayout;
        QWidget* inputField;
        InputMessage* inputMessage;
        QScrollArea* scrollArea;
        QFont font3;
    private slots:
        void sendMessage();
        void resizeInputField();
};

#endif // CHATCONTENT_H
