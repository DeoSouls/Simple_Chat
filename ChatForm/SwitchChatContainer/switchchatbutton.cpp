#include "switchchatbutton.h"
#include <QMouseEvent>
#include <QPainter>

SwitchChatButton::SwitchChatButton(const QString& text, const QString& username, int index, int chatId, QWidget *parent)
    : QWidget{parent}, m_chatName(text), m_userName(username), switchIndex(index), chatId(chatId) {
    m_unreadCount = 0;
    setFixedHeight(70);
    setFont(QFont("Helvetica", 11, 200));
}

void SwitchChatButton::setChatName(const QString& chatName) {
    m_chatName = chatName;
    update(); // Перерисовываем кнопку
}

void SwitchChatButton::setLastMessage(int unreadCount, const QString& lastMessage, const QString& userName) {
    m_userName = userName;
    m_lastMessage = lastMessage;
    m_unreadCount = unreadCount;
    update(); // Перерисовываем кнопку
}

void SwitchChatButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(chatId); // Испускаем сигнал при клике
    }
}

void SwitchChatButton::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем фон
    painter.setBrush(QColor("#222"));
    painter.setPen(QPen(QColor("#444"), 1));
    painter.drawRect(rect().adjusted(-1,-1,1,-1));

    // Рисуем круг
    QRect circleRect(rect().x() + 5, rect().y() + 5, 55, 55);
    painter.setPen(Qt::lightGray);
    painter.drawEllipse(circleRect);

    // Добавляем инициалы
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 14, 800));
    int posInit = m_chatName.indexOf(' ');
    QString initials = m_chatName.mid(0,1) + m_chatName.mid(posInit+1,1);
    painter.drawText(circleRect, Qt::AlignCenter, initials);

    // Имя пользователя
    painter.setPen(Qt::white);
    painter.setFont(QFont("Segoe UI", 14, 300));
    if(m_unreadCount > 0) {
        painter.drawText(QPoint(70, 25), m_chatName+" +"+QString::number(m_unreadCount));
    } else {
        painter.drawText(QPoint(70, 25), m_chatName);
    }

    QFontMetrics fm(QFont("Segoe UI", 11, 300));
    int maxRectWidth = rect().width() * 0.4;
    if(!m_lastMessage.isEmpty()) {
        // Прозрачный прямоугольник
        QString text = m_userName+": "+m_lastMessage+"...";
        QRect messageRect = QRect(70, 40, maxRectWidth, 20);
        QColor semiTransparentBlack(245, 245, 245, 40); // Черный цвет с прозрачностью
        painter.setBrush(semiTransparentBlack);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(messageRect, 7, 7);

        // Последнее сообщение
        QString elidedText = fm.elidedText(text, Qt::ElideRight, maxRectWidth);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 11, 300));
        painter.drawText(messageRect.adjusted(4,-1,-1,-1), Qt::TextSingleLine, elidedText);
    }

}

