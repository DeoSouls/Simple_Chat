#include "chatmessagemodel.h"

ChatMessageModel::ChatMessageModel(QObject* parent) : QAbstractListModel(parent) {}

// Переопределение метода для получения количества строк (сообщений) в модели
int ChatMessageModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return messages.size();
}

// Переопределение метода для получения данных по индексу и роли
QVariant ChatMessageModel::data(const QModelIndex& index,int role) const {
    if(!index.isValid() || index.row() >= messages.size())
        return QVariant();

    const ChatMessage& msg = messages.at(index.row());

    switch (role) {
        case ChatMessageRole: // Роль для получения всего объекта сообщения
            return QVariant::fromValue(msg);
        case UsernameRole: // Роль для получения имени пользователя
            return msg.username;
        case IsMineRole: // Роль для определения, принадлежит ли сообщение текущему пользователю
            return msg.isMine;
        case TimestampRole: // Роль для получения временной метки
            return msg.timestamp;
        case HasImageRole: // Роль для проверки наличия изображения
            return msg.hasImage;
        case ImageDataRole: // Роль для получения данных изображения
            return msg.imageData;
        default:
            return QVariant();
    }
}

// Переопределение метода для связывания ролей с именами для использования в QML
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

// Метод для добавления нового сообщения в конец списка
void ChatMessageModel::addMessage(const ChatMessage& msg) {
    beginInsertRows(QModelIndex(), messages.size(), messages.size());
    messages.append(msg);
    endInsertRows();
}

// Метод для добавления сообщений в начало списка (например, при загрузке истории)
void ChatMessageModel::prependMessages(const QVector<ChatMessage>& newMessages) {
    beginInsertRows(QModelIndex(), 0, newMessages.size() - 1);
    // Итерируемся по newMessages в обратном порядке
    for (int i = newMessages.size() - 1; i >= 0; --i) {
        messages.prepend(newMessages[i]);
    }
    endInsertRows();
}

// Метод для полной замены списка сообщений (например, при загрузке из базы данных)
void ChatMessageModel::loadMessages(const QVector<ChatMessage>& msgs) {
    beginResetModel();
    messages = msgs;
    endResetModel();
}

// Метод для получения всех сообщений
QVector<ChatMessage> ChatMessageModel::getMessages() const {
    return messages;
}

// Метод для очистки всех сообщений
void ChatMessageModel::clearMessage() {
    beginResetModel();
    messages.clear();
    endResetModel();
}
