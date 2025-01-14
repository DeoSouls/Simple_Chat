#ifndef USERCHOICE_H
#define USERCHOICE_H

#include "../chatform.h"
#include "userchoicemodel.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QListView>

class UserChoice : public QWidget {
        Q_OBJECT
    public:
        explicit UserChoice(int id, ChatForm* chatform, QWebSocket* m_client, QStackedWidget* stackedWidget, QWidget *parent = nullptr);

    public slots:
        void addMenuUser();
        void returnChat();
        void createChat(const QModelIndex &current);
    private slots:
        void handlerCommand(const QString& message);
        void addUsers(const QJsonObject& message);
    private:
        QVBoxLayout* mainLayout;
        QListView* userView;
        UserChoiceModel* userModel;
        QStackedWidget* stackedWidget;
        ChatForm* chatform;
        QWebSocket* m_socket;

        int currentUserId;

        void clearLayout(QLayout *layout);
};

#endif // USERCHOICE_H
