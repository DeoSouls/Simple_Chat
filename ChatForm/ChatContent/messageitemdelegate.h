#ifndef MESSAGEITEMDELEGATE_H
#define MESSAGEITEMDELEGATE_H

#include "chatmessagemodel.h"
#include <QStyledItemDelegate>
#include <QFont>

class MessageItemDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        explicit MessageItemDelegate(QObject* parent = nullptr);

        // Переопределяем метод paint для кастомного рисования
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        // Переопределяем размер элемента
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    private:
        QFont messageFont;
};

#endif // MESSAGEITEMDELEGATE_H
