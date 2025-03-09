#ifndef CHATCONTENT_H
#define CHATCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QList>
#include <QListView>
#include <QWebSocket>
#include "InputMessage/inputmessage.h"
#include "chatmessagemodel.h"
#include "infopanelchat.h"

class ChatContent : public QWidget {
        Q_OBJECT
    public:
        explicit ChatContent(const QString& chatName, const QString& selfName, const QString& email, bool status, int index, int id, int user2id, QWebSocket* m_client, QWidget *parent = nullptr);

        int user2Id;
        InfoPanelChat* infoPanelChat;
    private:
        void loadMessagesFromDatabase(const QString &message);
        void handleMessagesToChat(const QJsonArray& messagesArray, bool hasPag);
        template <typename T> QVector<QVector<T>> splitArray(const QVector<T>& vector, int chunkSize);

        int chatIndex;
        int userId;
        QString selfName;
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
