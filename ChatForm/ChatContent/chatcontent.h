#ifndef CHATCONTENT_H
#define CHATCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QList>
#include <QListView>
#include <QWebSocket>
#include "InputMessage/inputmessage.h"
#include "MessageContainer/messagecontainer.h"
#include "chatmessagemodel.h"

class ChatContent : public QWidget {
        Q_OBJECT
    public:
        ChatContent(const QString& chatName, int index, int id, QWebSocket* m_client, QWidget *parent = nullptr);

    private:
        void loadMessagesFromDatabase();

        int chatIndex;
        int userId;
        // под вопросом
        QList<QString> messages;
        QList<QString> currentUserMesssages;
        QList<QWidget*> messagesContainers;
        QVBoxLayout* outputFieldLayout;
        // под вопросом
        QVBoxLayout* chatContentLayout;
        QWidget* inputField;

        QWebSocket* m_socket;
        InputMessage* inputMessage;
        QFont font3;
        QFont font1;

        ChatMessageModel* messageModel;
        QListView* messageView;
        QPixmap m_attachedImage;
        QLabel* m_imagePreviewLabel;
        QHBoxLayout* inputFieldLayout;

    public slots:
        void sendMessage();
        void resizeInputField();
    private slots:
        void addMessageToChat(const QString &message);
        void onAttachImageButtonClicked();
};

#endif // CHATCONTENT_H
