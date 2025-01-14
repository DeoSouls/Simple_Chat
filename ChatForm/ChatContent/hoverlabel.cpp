#include "hoverlabel.h"

HoverLabel::HoverLabel(bool status, const QString& email, QWidget* parent) : QLabel(parent) {
    popup = new InfoPopup("", status, email, this);
    popup->hide();
}

void HoverLabel::enterEvent(QEnterEvent* event) {
    popup->move(mapToGlobal(QPoint(width()/2 - 175, height())));
    popup->infoName = text();
    popup->show();
    popup->animateShow();
    QLabel::enterEvent(event);
}

void HoverLabel::leaveEvent(QEvent* event) {
    popup->hide();
    QLabel::leaveEvent(event);
}
