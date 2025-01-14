#ifndef USERCHOICEMODEL_H
#define USERCHOICEMODEL_H

#include <QAbstractListModel>

struct Users {
    QString firstname;
    QString lastname;
    QString email;
    int user_id;
};

class UserChoiceModel : public QAbstractListModel {
    Q_OBJECT
    public:
        explicit UserChoiceModel(QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;

        // Роли для пользовательских данных
        enum Roles {
            UserChatRole = Qt::UserRole + 1,
            LastNameChatRole,
            MailChatRole,
            IdChatRole
        };

        // Метод для добавления пользователя
        void addUser(const Users& user);

        // Метод для загрузки пользователей из базы данных
        void loadUsers(const QVector<Users>& users);

    private:
        QVector<Users> users;
};

#endif // USERCHOICEMODEL_H
