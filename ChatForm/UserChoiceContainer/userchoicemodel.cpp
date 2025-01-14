#include "userchoicemodel.h"

UserChoiceModel::UserChoiceModel(QObject* parent) : QAbstractListModel(parent) {}

int UserChoiceModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return users.size();
}

QVariant UserChoiceModel::data(const QModelIndex& index,int role) const {
    if(!index.isValid() || index.row() >= users.size())
        return QVariant();

    const Users& user = users.at(index.row());

    switch (role) {
    case UserChatRole:
        return QVariant::fromValue(user);
    case LastNameChatRole:
        return user.lastname;
    case MailChatRole:
        return user.email;
    case IdChatRole:
        return user.user_id;
    default:
        return QVariant();
    }
}

void UserChoiceModel::addUser(const Users& user) {
    beginInsertRows(QModelIndex(), users.size(), users.size());
    users.append(user);
    endInsertRows();
}

void UserChoiceModel::loadUsers(const QVector<Users>& usrs) {
    beginResetModel();
    users = usrs;
    endResetModel();
}
