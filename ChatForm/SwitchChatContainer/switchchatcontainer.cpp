#include "switchchatcontainer.h"
#include "../ChatContent/chatcontent.h"
#include <QLabel>

SwitchChatContainer::SwitchChatContainer(int userId, QString userFirstname, QString userLastname, QWidget *parent)
    : userId(userId), userFirstname(userFirstname), userLastname(userLastname), QWidget{parent} {
    switchLayout = new QVBoxLayout(this);
    switchLayout->setContentsMargins(0,0,0,0);
    switchLayout->setAlignment(Qt::AlignTop);
    switchLayout->setSpacing(0);

    setStyleSheet("QWidget {border-right: 1px solid #444}");
    toggleButton = new QPushButton("Show/Hide menu");
    toggleButton->setStyleSheet("QPushButton { border: 0px; background-color: #222; "
                                "color: white; border-right: 1px solid #444; }"
                                "QPushButton::hover { background-color: #444; }");

    switchChats = new QStackedWidget(this);

    QWidget* startChat = new QWidget(this);
    QVBoxLayout* layoutStartChat = new QVBoxLayout(startChat);
    layoutStartChat->setAlignment(Qt::AlignCenter);

    startChat->setContentsMargins(0,0,0,0);

    QLabel* startChatContent = new QLabel("Select chat or start a conversation with a new contact!");
    layoutStartChat->addWidget(startChatContent);

    switchChats->addWidget(startChat);
    addSwitchButtons();
    setLayout(switchLayout);
}

void SwitchChatContainer::addSwitchButtons() {
    switchLayout->addWidget(toggleButton);
    for(int i = 1; i < 12; i++) {
        switchButtons.push_back(new SwitchChatButton("Switch Button " + QString::number(i),i, this));
        connect(switchButtons[i-1], SIGNAL(clicked()), SLOT(showChat()));

        switchLayout->addWidget(switchButtons[i-1]);
        switchChats->addWidget(new ChatContent("Maria", i, userId, this));
    }
}

void SwitchChatContainer::showChat() {
    QObject* senderObj = sender();
    SwitchChatButton* button = qobject_cast<SwitchChatButton*>(senderObj);

    switchChats->setCurrentIndex(button->switchIndex);
}

SwitchChatContainer::~SwitchChatContainer() {
}
