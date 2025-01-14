#ifndef USERITEMDELEGATE_H
#define USERITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "userchoicemodel.h"

class UserItemDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        explicit UserItemDelegate(QObject* parent = nullptr);

        // Переопределяем метод paint для кастомного рисования
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        // Переопределяем размер элемента
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // USERITEMDELEGATE_H
