#ifndef INFOPOPUP_H
#define INFOPOPUP_H

#include <QWidget>

class InfoPopup : public QWidget {
        Q_OBJECT
    public:
        explicit InfoPopup(const QString& infoName, bool status, const QString& email, QWidget *parent = nullptr);

        QString infoName;
        bool m_status;
        QString m_email;
    public slots:
        void animateShow();
    protected:
        void paintEvent(QPaintEvent* event) override;
        void enterEvent(QEnterEvent* event) override;
        void leaveEvent(QEvent* event) override;
};

#endif // INFOPOPUP_H
