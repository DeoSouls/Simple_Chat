#include "chatserver.h"
#include <QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

ChatServer::ChatServer(quint16 port, QObject *parent) : QObject{parent},
    m_server(new QWebSocketServer(QStringLiteral("Chat Server"), QWebSocketServer::NonSecureMode, this)) {

    if(m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Chat server listening on port: " << port;
        connect(m_server, &QWebSocketServer::newConnection, this, &ChatServer::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &ChatServer::closed);

    } else {
        qCritical() << "Failed to start server:" << m_server->errorString();
    }
}

ChatServer::~ChatServer() {
    m_server->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void ChatServer::onNewConnection() {
    QWebSocket *client = m_server->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &ChatServer::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &ChatServer::onClientDisconnected);

    m_clients.insert(client);
    qDebug() << "New client connected.";
}

void ChatServer::onTextMessageReceived(const QString &message) {
    QWebSocket *senderClient = qobject_cast<QWebSocket *>(sender());
    if (senderClient) {
        // Сохранение сообщения в базу данных можно добавить здесь

        for (QWebSocket *client : qAsConst(m_clients)) {
            client->sendTextMessage(message); // Отправка сообщения всем клиентам
        }
    }
}

void ChatServer::onClientDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.remove(client);
        client->deleteLater();
        qDebug() << "Client disconnected.";
    }
}

