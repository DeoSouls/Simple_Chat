#ifndef CHATMESSAGEMODEL_H
#define CHATMESSAGEMODEL_H

#include "messageitemdelegate.h"
#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include <QDateTime>

struct ChatMessage {
    QString username;
    QString message;
    bool isMine;
    QDateTime timestamp;
    bool hasImage;
    QByteArray imageData;

    ChatMessage() : isMine(false), hasImage(false) {}
};

class ChatMessageModel : public QAbstractListModel {
        Q_OBJECT
    public:
        explicit ChatMessageModel(QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;

        // Роли для пользовательских данных
        enum Roles {
            ChatMessageRole = Qt::UserRole + 1,
            UsernameRole,
            IsMineRole,
            TimestampRole,
            HasImageRole,
            ImageDataRole
        };

        // Метод для добавления сообщения
        void addMessage(const ChatMessage& msg);

        // Метод для загрузки сообщений из базы данных
        void loadMessages(const QVector<ChatMessage>& msgs);

        // Метод для очищения данных модели
        void clearMessage();

    protected:
        QHash<int, QByteArray> roleNames() const override;

    private:
        QVector<ChatMessage> messages;
};

#endif // CHATMESSAGEMODEL_H
