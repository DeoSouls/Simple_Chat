#include "infopanelchat.h"
#include <QLabel>

InfoPanelChat::InfoPanelChat(int index, const QString& chatName, QWidget *parent)
    : QWidget{parent}, chatIndex(index) {
    panelLayout = new QHBoxLayout(this);
    panelLayout->setContentsMargins(0,0,0,0);
    panelLayout->setSpacing(0);
    setFixedHeight(45);
    setStyleSheet("QWidget { background-color: #222 } "
                             "QLabel { border: 0px; border-bottom: 1px solid #444; "
                             "border-left: 1px solid #333; color: white;}");

    QLabel* labelChatIndex = new QLabel("Chat "+ QString::number(chatIndex));
    QLabel* nameChat = new QLabel(chatName);
    QLabel* infoTime = new QLabel("Time");
    labelChatIndex->setFont(QFont("Helvetica", 11));
    nameChat->setFont(QFont("Helvetica", 11));
    infoTime->setFont(QFont("Helvetica", 11));
    labelChatIndex->setAlignment(Qt::AlignCenter);
    infoTime->setAlignment(Qt::AlignCenter);
    nameChat->setAlignment(Qt::AlignCenter);

    panelLayout->addWidget(labelChatIndex, 1);
    panelLayout->addWidget(nameChat, 3);
    panelLayout->addWidget(infoTime, 1);
}
