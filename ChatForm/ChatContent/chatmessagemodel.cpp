#include "chatmessagemodel.h"

ChatMessageModel::ChatMessageModel(QObject* parent) : QAbstractListModel(parent) {}

int ChatMessageModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return messages.size();
}

QVariant ChatMessageModel::data(const QModelIndex& index,int role) const {
    if(!index.isValid() || index.row() >= messages.size())
        return QVariant();

    const ChatMessage& msg = messages.at(index.row());

    switch (role) {
        case ChatMessageRole:
            return QVariant::fromValue(msg);
        case UsernameRole:
            return msg.username;
        case IsMineRole:
            return msg.isMine;
        case TimestampRole:
            return msg.timestamp;
        case HasImageRole:
            return msg.hasImage;
        case ImageDataRole:
            return msg.imageData;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ChatMessageModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ChatMessageRole] = "chatMessage";
    roles[UsernameRole] = "username";
    roles[IsMineRole] = "isMine";
    roles[TimestampRole] = "timestamp";
    roles[HasImageRole] = "hasImage";
    roles[ImageDataRole] = "imageData";
    return roles;
}

void ChatMessageModel::addMessage(const ChatMessage& msg) {
    beginInsertRows(QModelIndex(), messages.size(), messages.size());
    messages.append(msg);
    endInsertRows();
}

void ChatMessageModel::prependMessages(const QVector<ChatMessage>& newMessages) {
    beginInsertRows(QModelIndex(), 0, newMessages.size() - 1);
    // Итерируемся по newMessages в обратном порядке
    for (int i = newMessages.size() - 1; i >= 0; --i) {
        messages.prepend(newMessages[i]);
    }
    endInsertRows();
}

void ChatMessageModel::loadMessages(const QVector<ChatMessage>& msgs) {
    beginResetModel();
    messages = msgs;
    endResetModel();
}

QVector<ChatMessage> ChatMessageModel::getMessages() const {
    return messages;
}

void ChatMessageModel::clearMessage() {
    beginResetModel();
    messages.clear();
    endResetModel();
}
