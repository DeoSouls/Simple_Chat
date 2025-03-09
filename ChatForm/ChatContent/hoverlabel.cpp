#include "hoverlabel.h"

// Конструктор класса HoverLabel, который наследуется от QLabel
// Принимает статус пользователя (онлайн/оффлайн), email и родительский виджет
HoverLabel::HoverLabel(bool status, const QString& email, QWidget* parent) : QLabel(parent), h_status(status) {
    // Создаем всплывающее окно с информацией о пользователе
    // Передаем пустую строку для имени (будет заполнено позже), статус и email
    popup = new InfoPopup("", status, email, this);
    // Скрываем всплывающее окно при инициализации
    popup->hide();
}

// Переопределение метода enterEvent, который вызывается при наведении курсора на метку
void HoverLabel::enterEvent(QEnterEvent* event) {
    // Позиционируем всплывающее окно по центру относительно метки, со смещением вверх
    popup->move(mapToGlobal(QPoint(width()/2 - 175, height())));
    popup->infoName = text();
    popup->show();
    popup->animateShow();
    // Вызываем базовую реализацию метода
    QLabel::enterEvent(event);
}

// Переопределение метода leaveEvent, который вызывается при выходе курсора из области метки
void HoverLabel::leaveEvent(QEvent* event) {
    popup->hide();
    // Вызываем базовую реализацию метода
    QLabel::leaveEvent(event);
}
