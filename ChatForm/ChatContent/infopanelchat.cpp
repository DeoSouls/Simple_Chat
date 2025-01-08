#include "infopanelchat.h"
#include "HoverLabel.h"
#include <QLabel>
#include <QPushButton>

InfoPanelChat::InfoPanelChat(int index, const QString& email, bool status, const QString& chatName, QWidget *parent)
    : QWidget{parent}, chatIndex(index) {
    panelLayout = new QHBoxLayout(this);
    panelLayout->setContentsMargins(0,0,0,0);
    panelLayout->setSpacing(0);
    setFixedHeight(45);
    setStyleSheet("QWidget { background-color: #222 } "
                             "QLabel { border: 0px; border-bottom: 1px solid #444; "
                             "border-left: 1px solid #333; color: white;}"
                             "QPushButton { border: none; border-bottom: 1px solid #444; "
                             "border-left: 1px solid #333; color: white; }");

    QLabel* labelChatIndex = new QLabel("⚪ Был(а) недавно");
    if(status) {
        labelChatIndex->setText("🟢 В сети");
    }
    HoverLabel* nameChat = new HoverLabel(status, email);
    nameChat->setText(chatName);
    QPushButton* infoTime = new QPushButton();
    infoTime->setFixedHeight(45);
    infoTime->setIcon(QIcon("C://Users//Purik//Downloads//iconSearch.png"));
    infoTime->setIconSize(QSize(20,20));

    QFont font("Segoe UI", 13, 300);
    labelChatIndex->setFont(font);
    nameChat->setFont(font);
    infoTime->setFont(font);

    labelChatIndex->setAlignment(Qt::AlignCenter);
    nameChat->setAlignment(Qt::AlignCenter);

    panelLayout->addWidget(labelChatIndex, 2);
    panelLayout->addWidget(nameChat, 2);
    panelLayout->addWidget(infoTime, 3);
}
