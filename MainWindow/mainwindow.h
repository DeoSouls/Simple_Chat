#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>


class Mainwindow : public QWidget {
        Q_OBJECT
    public:
        explicit Mainwindow(QWidget* parent = nullptr);

        ~Mainwindow();
    private:
        QStackedWidget* stackedWidget;
        QVBoxLayout* layoutMain;
};

#endif // MAINWINDOW_H
