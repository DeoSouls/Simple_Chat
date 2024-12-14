#ifndef INPUTMESSAGE_H
#define INPUTMESSAGE_H

#include <QWidget>
#include <QTextEdit>

class InputMessage : public QTextEdit {
        Q_OBJECT
    public:
        explicit InputMessage(QWidget* parent = nullptr);

        ~InputMessage();
    private slots:
        void adjustHeight();
};

#endif // INPUTMESSAGE_H
