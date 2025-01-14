#include "infopanelchat.h"
#include "HoverLabel.h"
#include <QLabel>

InfoPanelChat::InfoPanelChat(int index, const QString& email, bool status, const QString& chatName, QWidget *parent)
    : QWidget{parent}, chatIndex(index) {
    panelLayout = new QHBoxLayout(this);
    panelLayout->setContentsMargins(0,0,0,0);
    panelLayout->setSpacing(0);
    setFixedHeight(45);
    setStyleSheet("QWidget { background-color: #222 } "
                             "QLabel { border: 0px; border-bottom: 1px solid #444; "
                             "border-left: 1px solid #333; color: white;}");

    QLabel* labelChatIndex = new QLabel("⚪ Был(а) недавно");
    if(status) {
        labelChatIndex->setText("🟢 В сети");
    }
    HoverLabel* nameChat = new HoverLabel(status, email);
    nameChat->setText(chatName);

    // Панель поисковика
    QWidget* searchPanel = new QWidget(this);
    searchPanel->setStyleSheet("QWidget { border: none; border-bottom: 1px solid #444; "
                               "border-left: 1px solid #333; color: white; }"
                               "QTextEdit { background-color: #222; border: none; margin-left: 5px;} "
                               "QPushButton { background-color: #222; border: none; } "
                               "QPushButton:hover { background-color: #444; }");
    searchLayout = new QHBoxLayout(searchPanel);
    searchLayout->setContentsMargins(0,0,0,0);
    searchLayout->setSpacing(0);

    // Содержимое поисковика
    searchLine = new QTextEdit();
    searchLine->setFixedHeight(30);
    searchLine->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    searchLine->setFont(QFont("Segoe UI", 12, 300));
    searchLine->setPlaceholderText("Введите текст...");
    searchingButton = new QPushButton();
    searchingButton->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconSearch.png"));
    searchingButton->setIconSize(QSize(20,20));
    searchingButton->setFixedSize(45,44);
    searchSwitch = new QPushButton();
    searchSwitch->setFixedHeight(44);
    searchSwitch->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconSearch.png"));
    searchSwitch->setIconSize(QSize(20,20));
    connect(searchSwitch, &QPushButton::clicked, this, &InfoPanelChat::openTextLineSearch);
    searchExit = new QPushButton();
    searchExit->setFixedHeight(44);
    searchExit->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconArrow.png"));
    searchExit->setIconSize(QSize(20,20));
    connect(searchExit, &QPushButton::clicked, this, &InfoPanelChat::closeTextLineSearch);

    QFont font("Segoe UI", 13, 300);
    labelChatIndex->setFont(font);
    nameChat->setFont(font);

    searchSwitch->setFont(font);
    searchLayout->addWidget(searchSwitch);
    searchLayout->insertWidget(0, searchLine);
    searchLayout->insertWidget(1, searchingButton);
    searchLayout->insertWidget(2, searchExit);
    searchExit->hide();
    searchLine->hide();
    searchingButton->hide();

    labelChatIndex->setAlignment(Qt::AlignCenter);
    nameChat->setAlignment(Qt::AlignCenter);

    panelLayout->addWidget(labelChatIndex, 2);
    panelLayout->addWidget(nameChat, 2);
    panelLayout->addWidget(searchPanel, 3);
}

void InfoPanelChat::openTextLineSearch() {
    searchLayout->setStretch(0,0);
    searchSwitch->hide();
    searchLine->show();
    searchingButton->show();
    searchExit->show();
}

void InfoPanelChat::closeTextLineSearch() {
    searchSwitch->show();
    searchLine->hide();
    searchingButton->hide();
    searchExit->hide();
    searchLayout->setStretch(0,1);
}
