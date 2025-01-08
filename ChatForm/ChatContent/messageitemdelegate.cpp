#include "messageitemdelegate.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QApplication>
#include <QTextLayout>
#include <QTextLine>

const int MIN_BUBBLE_WIDTH = 100;  // Минимальная ширина пузыря в пикселях

MessageItemDelegate::MessageItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {
    messageFont = QFont("Segoe UI", 11); // Укажите нужный шрифт и размер
    messageFont.setWeight(QFont::Normal);
}

void MessageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    // Получаем данные из модели
    QVariant data = index.data(ChatMessageModel::ChatMessageRole);
    if (!data.isValid()) {
        qDebug() << "Данные невалидны для индекса:" << index.row();
        return;
    }

    ChatMessage msg = data.value<ChatMessage>();
    if (msg.message.isEmpty()) {
        qDebug() << "Сообщение пусто для индекса:" << index.row();
    }

    painter->save();

    // Устанавливаем антиалиасинг для более гладкого текста
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor bubbleColor = msg.isMine ? QColor("#DCF8C6") : QColor("#FFFFFF");
    QColor textColor = QColor("#000000");
    QColor timeColor = QColor("#808080");

    // Размеры пузыря сообщения
    const int padding = 2;
    const int bubbleRadius = 10;
    int imageMaxWidth = 400;
    int imageMaxHeight = 400;

    QSize bubbleSize = sizeHint(option, index);
    QRect bubbleRect = option.rect.adjusted(padding, padding, -padding, -padding);

    // Выравнивание пузыря
    if (msg.isMine) {
        // Выравнивание вправо
        bubbleRect = QRect(option.rect.right() - bubbleSize.width() - padding,
                           option.rect.top(),
                           bubbleSize.width(),
                           bubbleSize.height());
    } else {
        // Выравнивание влево
        bubbleRect = QRect(option.rect.left() + padding,
                           option.rect.top(),
                           bubbleSize.width(),
                           bubbleSize.height());
    }
    // Рисуем пузырь сообщения
    QPainterPath path;
    path.addRoundedRect(bubbleRect, bubbleRadius, bubbleRadius);

    // Добавление тени
    QPainterPath shadowPath = path;
    shadowPath.translate(2, 2); // Смещаем путь для тени

    QColor shadowColor = QColor(0, 0, 0, 50); // Полупрозрачный черный цвет
    painter->setBrush(shadowColor);
    painter->setPen(Qt::NoPen);
    painter->drawPath(shadowPath);

    painter->setBrush(bubbleColor);
    painter->setPen(Qt::NoPen);
    painter->drawPath(path);

    painter->setFont(messageFont);
    painter->setPen(textColor);


    // Рисование изображения, если есть
    QPixmap pixmap;
    int imageOffsetY = 0; // Насколько сдвинуть текст вниз

    if (msg.hasImage && !msg.imageData.isEmpty()) {
        if (pixmap.loadFromData(msg.imageData, "PNG")) {
            // Успешно загрузили pixmap
            QPixmap scaledImage = pixmap.scaled(imageMaxWidth,
                                                imageMaxHeight,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
            // Считаем, на какой высоте нарисовать изображение
            int xImg = bubbleRect.left() + 10;
            int yImg = bubbleRect.top()  + 10;
            int wImg = scaledImage.width();
            int hImg = scaledImage.height();

            // Формируем imageRect в верхней части пузыря
            QRect imageRect(xImg, yImg, wImg, hImg);

            // Рисуем
            painter->drawPixmap(imageRect, scaledImage);

            // На сколько сдвинем текст ниже картинки
            imageOffsetY = hImg + 6;
        } else {
            qWarning() << "Failed to load pixmap from QByteArray in delegate.";
        }
    }

    // Рисуем текст сообщения
    QRect textRect = bubbleRect.adjusted(10, 10 + imageOffsetY, -10, -10);
    painter->drawText(textRect, Qt::TextWrapAnywhere, msg.message);

    // Отрисовка времени
    QFont timeFont = messageFont;
    timeFont.setPointSize(messageFont.pointSize() - 2); // Уменьшаем размер шрифта для времени
    painter->setFont(timeFont);
    painter->setPen(timeColor);
    QString timeString = msg.timestamp.toString("hh:mm");
    QRect timeRect = QRect(bubbleRect.left() + padding - 3,
                           bubbleRect.bottom() - padding - 15,
                           bubbleRect.width() - 2 * padding,
                           15);
    painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, timeString);

    painter->restore();
}


QSize MessageItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QVariant data = index.data(ChatMessageModel::ChatMessageRole);
    if (!data.isValid()) {
        return QSize(0, 0);
    }

    ChatMessage msg = data.value<ChatMessage>();

    // Рассчитываем размер текста
    QFontMetrics fm(messageFont);

    int maxBubbleWidth = option.rect.width() * 0.7;
    int padding = 12;
    int imageMaxWidth = 400;
    int imageMaxHeight = 400;

    QPixmap pixmap;
    int scaledImageX = 0;
    int scaledImageY = 0;

    if (msg.hasImage && !msg.imageData.isEmpty()) {
        if (pixmap.loadFromData(msg.imageData)) {
            QPixmap scaledImage = pixmap.scaled(imageMaxWidth,
                                                imageMaxHeight,
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
            scaledImageX = scaledImage.width();
            scaledImageY = scaledImage.height();
        } else {
            qWarning() << "Failed to load pixmap from QByteArray in delegate.";
        }
    }

    // Вычисление bounding rectangle для текста с учётом переноса слов
    QRect boundingRect = fm.boundingRect(QRect(0, 0, maxBubbleWidth - 2 * padding, 1000),
                                         Qt::TextWrapAnywhere,
                                         msg.message);

    // Общая высота пузыря (текст + отступы сверху и снизу)
    int bubbleHeight = boundingRect.height() + scaledImageY + 2 + 2 * padding;

    // Общая ширина пузыря (текст + отступы слева и справа)
    int bubbleWidth = 0;
    if(msg.hasImage) {
        bubbleWidth = scaledImageX + 2 * padding;
    } else {
        bubbleWidth = boundingRect.width() + scaledImageX + 2 * padding;
    }
    // Ограничение ширины пузыря до максимальной
    if (bubbleWidth > maxBubbleWidth + scaledImageX) {
        bubbleWidth = maxBubbleWidth;
    }
    // Ограничение ширины пузыря от минимальной
    if(bubbleWidth < MIN_BUBBLE_WIDTH) {
        bubbleWidth = MIN_BUBBLE_WIDTH;
    }

    // Общий размер элемента
    return QSize(bubbleWidth, bubbleHeight);
}
