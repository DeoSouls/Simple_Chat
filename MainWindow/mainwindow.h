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
        explicit Mainwindow(const QUrl &url, QWidget* parent = nullptr);

        QSqlQuery executeQuery(const QString& queryStr, const QVariantMap& params = {});
    private slots:
        void onLoginClicked(LoginForm* login);
        void onLogupClicked(LoginForm* log);

    private:
        QStackedWidget* stackedWidget;
        QVBoxLayout* layoutMain;
        QWebSocket* m_socket;

        void setupChatInterface(int userid, const QString& firstname, const QString& lastname);
        QString hashPassword(const QString& password);
        void switchToWidget(QWidget* widget);
        void showError(const QString& message);
};

#endif // MAINWINDOW_H
