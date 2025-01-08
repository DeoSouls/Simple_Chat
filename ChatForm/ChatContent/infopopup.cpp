#include "infopopup.h"
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QLabel>
#include <QPainter>

InfoPopup::InfoPopup(const QString& infoName, bool status, const QString& email, QWidget *parent)
    : QWidget{parent}, infoName(infoName), m_status(status), m_email(email) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    setStyleSheet("QWidget { background-color: #222; border: 1px solid #444; }"
                  "QLabel { color: white; border: none; padding: 5px }");
    setFixedSize(350, 250);
    setWindowOpacity(0.0); // Начальная прозрачность
}

void InfoPopup::animateShow() {
    // Анимация прозрачности
    QPropertyAnimation* fadeIn = new QPropertyAnimation(this, "windowOpacity");
    fadeIn->setDuration(300);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    // Анимация развертывания
    QPropertyAnimation* scaleIn = new QPropertyAnimation(this, "geometry");
    QRect startRect = QRect(pos().x(), pos().y(), width(), 0);
    QRect endRect = QRect(pos().x(), pos().y(), width(), height());
    scaleIn->setDuration(300);
    scaleIn->setStartValue(startRect);
    scaleIn->setEndValue(endRect);
    scaleIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void InfoPopup::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание
    painter.setBrush(QColor("#222"));              // Устанавливаем цвет фона
    painter.setPen(QPen(QColor("#444"), 1));       // Без рамки (или задайте цвет, если рамка нужна)

    QRect rect = this->rect();                     // Получаем размеры виджета
    rect.adjust(1,1,-1,-1);
    painter.drawRoundedRect(rect, 10, 10);         // Рисуем скруглённый прямоугольник с радиусом 10

    // Рисуем круг
    QRect circleRect(rect.x() + 20, rect.y() + 20, 55, 55);
    painter.setPen(Qt::lightGray);
    painter.drawEllipse(circleRect);

    // Добавляем инициалы
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 14, 800));
    int posInit = infoName.indexOf(' ');
    QString initials = infoName.mid(0,1) + infoName.mid(posInit+1,1);
    painter.drawText(circleRect, Qt::AlignCenter, initials);

    // Добавляем имя пользователя, почту и статус
    painter.setPen(QColor("white"));
    painter.setFont(QFont("Segoe UI", 14, 300));

    int xOffset = 20;
    int yOffset = 56;

    QString statusString;
    if(m_status) {
        statusString = "в сети 🟢";
    } else {
        statusString = "был(а) недавно.";
    }

    painter.drawText(QPoint(xOffset + 90, yOffset), "Имя: " + infoName);
    painter.drawText(QPoint(xOffset, yOffset + 66), "Email: " + m_email);
    painter.drawText(QPoint(xOffset, yOffset + 132), "Статус: " + statusString);
}

void InfoPopup::enterEvent(QEnterEvent* event) {
    show();
    QWidget::enterEvent(event);
}

void InfoPopup::leaveEvent(QEvent* event) {
    if (!parentWidget()->underMouse()) {
        hide();
    }

    QWidget::leaveEvent(event);
}
