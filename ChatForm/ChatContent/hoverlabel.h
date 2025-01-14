#ifndef HOVERLABEL_H
#define HOVERLABEL_H

#include "infopopup.h"
#include <QWidget>
#include <QLabel>

class InfoPopup;

class HoverLabel : public QLabel {
        Q_OBJECT
    public:
        explicit HoverLabel(bool status, const QString& email, QWidget* parent = nullptr);
    protected:
        void enterEvent(QEnterEvent* event) override;
        void leaveEvent(QEvent* event) override;
    private:
        InfoPopup* popup;
};

#endif // HOVERLABEL_H
