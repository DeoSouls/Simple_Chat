#include "inputmessage.h"

InputMessage::InputMessage(QWidget* parent) : QTextEdit{parent} {
    connect(this, &QTextEdit::textChanged, this, &InputMessage::adjustHeight);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void InputMessage::adjustHeight() {
    int docHeight = document()->size().toSize().height();
    int minHeight = 40;
    int maxHeight = 300;
    int newHeight = qBound(minHeight, docHeight + 5, maxHeight);
    setFixedHeight(newHeight);
}
