#ifndef SWITCHCHATBUTTON_H
#define SWITCHCHATBUTTON_H

#include <QPushButton>

class SwitchChatButton : public QPushButton {
        Q_OBJECT
    public:
        explicit SwitchChatButton(const QString text, int index, QWidget *parent = nullptr);

        int switchIndex;
};

#endif // SWITCHCHATBUTTON_H
