#include "useritemdelegate.h"
#include <QPainter>

UserItemDelegate::UserItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void UserItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    // Получаем данные из модели
    QVariant data = index.data(UserChoiceModel::UserChatRole);
    if (!data.isValid()) {
        qDebug() << "Данные невалидны для индекса:" << index.row();
        return;
    }

    Users user = data.value<Users>();

    if (user.firstname.isEmpty()) {
        qDebug() << "Пользователя нет для индекса:" << index.row();
    }

    painter->save();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Проверяем, наведён ли курсор
    if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, QColor("#333"));
    } else if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor("#222"));
    }

    // Устанавливаем антиалиасинг для более гладкого текста
    painter->setRenderHint(QPainter::Antialiasing, true);

    QString initials = user.firstname.left(1).toUpper() + user.lastname.left(1).toUpper();

    // Рисуем круг
    QRect circleRect(option.rect.width()/2 - 110, option.rect.y() + 2, 45, 45);
    painter->setPen(Qt::lightGray);
    painter->drawEllipse(circleRect);

    // Добавляем инициалы
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 11));
    painter->drawText(circleRect, Qt::AlignCenter, initials);

    // Рисование имени и почты
    QString fullname = user.firstname + " " + user.lastname;
    QRect textRect(option.rect.width()/2 - 35, option.rect.y() - 8, option.rect.width() - 60, option.rect.height());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, fullname);

    QRect emailRect(option.rect.width()/2 - 35, option.rect.y() + 10, option.rect.width() - 60, option.rect.height());
    painter->setPen(Qt::gray);
    painter->drawText(emailRect, Qt::AlignLeft | Qt::AlignVCenter, user.email);

    painter->restore();
}


QSize UserItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {

    // Общий размер элемента
    return QSize(option.rect.width(), 50); // Добавляем отступы
}
