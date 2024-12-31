#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWebSocket>
#include <QWebSocketServer>
#include <QObject>
#include <QSet>

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
    private:
        QWebSocketServer* m_server;
        QSet<QWebSocket*> m_clients;

};

#endif // CHATSERVER_H
