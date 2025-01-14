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
#include "infopanelchat.h"

class ChatContent : public QWidget {
        Q_OBJECT
    public:
        ChatContent(const QString& chatName, const QString& email, bool status, int index, int id, QWebSocket* m_client, QWidget *parent = nullptr);

    private:
        void loadMessagesFromDatabase(const QString &message);
        template <typename T> QVector<QVector<T>> splitArray(const QVector<T>& vector, int chunkSize);

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
        InfoPanelChat* infoPanelChat;
        QFont font3;
        QFont font1;

        QWidget* imagePrev;
        ChatMessageModel* messageModel;
        MessageItemDelegate* messageDelegate;
        QVector<ChatMessage> allMessages;
        QListView* messageView;
        QPixmap m_sendImage;
        QPixmap m_attachedImage;
        QLabel* m_imagePreviewLabel;
        QHBoxLayout* inputFieldLayout;
        // Пагинация
        QVector<QVector<ChatMessage>> pagination;

    public slots:
        void sendMessage();
        void resizeInputField();
    private slots:
        void searchingMessage();
        void addMessageToChat(const QString &message);
        void onAttachImageButtonClicked();
        void scrollOnTop(int value);
};

#endif // CHATCONTENT_H
