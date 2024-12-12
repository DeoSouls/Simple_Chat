#ifndef SWITCHCHATCONTAINER_H
#define SWITCHCHATCONTAINER_H

#include "switchchatbutton.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>

class SwitchChatContainer : public QWidget {
    Q_OBJECT
    public:
        explicit SwitchChatContainer(QWidget *parent = nullptr);
        QStackedWidget* switchChats;
        QPushButton* toggleButton;
    private:
        QVBoxLayout* switchLayout;
        QList<SwitchChatButton*> switchButtons;

        void addSwitchButtons();
    private slots:
        void showChat();
};

#endif // SWITCHCHATCONTAINER_H
