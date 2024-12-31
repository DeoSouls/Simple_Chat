#ifndef SWITCHCHATCONTAINER_H
#define SWITCHCHATCONTAINER_H

#include "switchchatbutton.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QWebSocket>
#include <QScrollArea>

class SwitchChatContainer : public QWidget {
    Q_OBJECT
    public:
        explicit SwitchChatContainer(int userId, QString userFirstname, QString userLastname, QWebSocket* m_client, QWidget *parent = nullptr);
        QStackedWidget* switchChats;
        QPushButton* toggleButton;

        int userId;
        QString userFirstname;
        QString userLastname;
        QWebSocket* m_socket;
        int chatIndex;
        void clearLayout(QLayout *layout);
    private:
        QVBoxLayout* switchLayout;
        QMap<int, SwitchChatButton*> switchButtons;
        QScrollArea* scrollArea;
        QWidget* buttonContainer;
        QVBoxLayout* buttonLayout;

        QSqlQuery executeQuery(const QString& queryStr, const QVariantMap& params = {});
        // QString formatButtonText(const QString& chatName, const QString& lastMessage);
    public slots:
        void addSwitchButtons();
    private slots:
        void updateStatus(int index);
        void updateButtons();
        void showChat();
};

#endif // SWITCHCHATCONTAINER_H
