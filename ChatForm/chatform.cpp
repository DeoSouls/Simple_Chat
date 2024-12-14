#include "chatform.h"
#include "SwitchChatContainer/switchchatbutton.h"
#include "SwitchChatContainer/switchchatcontainer.h"
#include <QPropertyAnimation>
#include <QSplitter>
#include <QHBoxLayout>

ChatForm::ChatForm(int userId, QString userFirstname, QString userLastname, QWidget *parent): QWidget{parent} {
    layoutChat = new QHBoxLayout(this);
    layoutChat->setContentsMargins(0,0,0,0);
    layoutChat->setSpacing(0);

    sContainer = new SwitchChatContainer(userId, userFirstname, userLastname);

    layoutChat->addWidget(sContainer);
    layoutChat->addWidget(sContainer->switchChats);

    menu = new QFrame(this);
    menu->setStyleSheet("background-color: #222; color: white; border-right: 1px solid #555");
    menu->setFixedWidth(200);
    menu->setGeometry(-200, 0, 200, height());
    menu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout* menuLayout = new QVBoxLayout(menu);
    menuLayout->addWidget(new QPushButton("Button 1"));
    menuLayout->addWidget(new QPushButton("Button 2"));
    exitFromChat = new QPushButton("Exit");
    menuLayout->addWidget(exitFromChat);
    menuLayout->setAlignment(Qt::AlignCenter);

    animation = new QPropertyAnimation(menu, "geometry", this);
    animation->setDuration(200);

    connect(sContainer->toggleButton, SIGNAL(clicked()), SLOT(toggleMenu()));

    layoutChat->setStretch(0,1);
    layoutChat->setStretch(1,3);

    setLayout(layoutChat);
}

void ChatForm::toggleMenu() {
    if (animation->state() == QAbstractAnimation::Running) {
        return;
    }

    QRect startGeometry = menu->geometry();
    QRect endGeometry;

    if (menu->geometry().x() < 0) {
        endGeometry = QRect(0, 0, menu->width(), height());
    } else {
        endGeometry = QRect(-menu->width(), 0, menu->width(), height());
    }

    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    animation->start();
}

void ChatForm::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    menu->setGeometry(menu->x(), 0, menu->width(), height());
}

ChatForm::~ChatForm() {
}
