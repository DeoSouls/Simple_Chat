#include "inputmessage.h"
// Конструктор InputMessage, подключает сигнал изменения текста к слоту adjustHeight()
InputMessage::InputMessage(QWidget* parent) : QTextEdit{parent} {
    connect(this, &QTextEdit::textChanged, this, &InputMessage::adjustHeight);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем вертикальный скроллбар для удобства
}

// Слот adjustHeight динамически регулирует высоту поля ввода
// в зависимости от размера введенного текста
void InputMessage::adjustHeight() {
    int docHeight = document()->size().toSize().height();
    int minHeight = 40;
    int maxHeight = 300;
    int newHeight = qBound(minHeight, docHeight + 5, maxHeight); // Ограничиваем высоту заданными пределами
    setFixedHeight(newHeight);
}
