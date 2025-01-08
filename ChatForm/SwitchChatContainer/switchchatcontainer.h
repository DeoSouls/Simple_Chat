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
    private:
        QVBoxLayout* switchLayout;
        QMap<int, SwitchChatButton*> switchButtons;
        QScrollArea* scrollArea;
        QWidget* buttonContainer;
        QVBoxLayout* buttonLayout;

        QSqlQuery executeQuery(const QString& queryStr, const QVariantMap& params = {});
    public slots:
        void addSwitchButtons(const QJsonObject& message);
    private slots:
        void updateHandler(const QString& message);
        void updateButtons(const QJsonObject& message);
        void showChat();
};

#endif // SWITCHCHATCONTAINER_H
