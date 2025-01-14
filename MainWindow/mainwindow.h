#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QWebSocket>
#include "../LoginForm/loginform.h"

class Mainwindow : public QWidget {
        Q_OBJECT
    public:
        explicit Mainwindow(const QUrl& url, QWidget* parent = nullptr);
    private slots:
        void onLoginClicked(LoginForm* login);
        void onLogupClicked(LoginForm* log);
        void connectToServer(const QUrl& url);
        void handlerLogin(const QString& message);
    private:
        QStackedWidget* stackedWidget;
        QVBoxLayout* layoutMain;
        QWebSocket* m_socket;
        QUrl url;

        void setupChatInterface(const QJsonObject& message);
        QString hashPassword(const QString& password);
        void switchToWidget(QWidget* widget);
        void showError(const QString& message);
};

#endif // MAINWINDOW_H
