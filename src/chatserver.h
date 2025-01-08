#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWebSocket>
#include <QWebSocketServer>
#include <QObject>
#include <QSet>
#include <QSqlQuery>

class ChatServer : public QObject {
    Q_OBJECT
    public:
        explicit ChatServer(quint16 port, QObject *parent = nullptr);
        ~ChatServer();
    signals:
        void closed();
    private slots:
        void onNewConnection();
        void onTextMessageReceived(const QString &message);
        void onClientDisconnected();
        void handleRegistration(QWebSocket* senderClient, const QJsonObject &message);
        void handleLogin(QWebSocket* senderClient, const QJsonObject &message);
        void handleSendMessage(QWebSocket* senderClient, const QJsonObject &message);
        void handleChatMessages(QWebSocket* senderClient, const QJsonObject &message);
        void handleAddChats(QWebSocket* senderClient, const QJsonObject &message);
        void handleUpdateChats(QWebSocket* senderClient, const QJsonObject &message);
        void handleUpdateStatusMessages(QWebSocket* senderClient, const QJsonObject &message);
        void handleAddUsers(QWebSocket* senderClient, const QJsonObject &message);
        void handleCreateChat(QWebSocket* senderClient, const QJsonObject &message);
        QSqlQuery executeQuery(const QString& command, const QVariantMap& params, const QString& errorMessage, QWebSocket* senderClient);
    private:
        QWebSocketServer* m_server;
        QSet<QWebSocket*> m_clients;
        QMap<int, QWebSocket*> m_autorizedClients;

};

#endif // CHATSERVER_H
