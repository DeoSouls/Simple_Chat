#include "chatcontent.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFontMetrics>
#include <QScrollArea>
#include <QDebug>

ChatContent::ChatContent(const QString case1, int index, QWidget *parent) : QWidget{parent}, chatIndex(index) {
    chatContentLayout = new QVBoxLayout(this);
    chatContentLayout->setContentsMargins(0,0,0,0);
    chatContentLayout->setAlignment(Qt::AlignTop);
    chatContentLayout->setSpacing(0);

    font3 = QFont("Arial", 11, QFont::Expanded);
    QFont font1("Arial", 9, QFont::Expanded);

    // инфо о чате (верхняя панель)
    QWidget* infoPanel = new QWidget(this);
    QHBoxLayout* infoChatLayout = new QHBoxLayout(infoPanel);
    infoChatLayout->setContentsMargins(0,0,0,0);
    infoChatLayout->setSpacing(15);
    infoPanel->setFixedHeight(45);
    infoPanel->setStyleSheet("QWidget { border-left: 1px solid #555; "
                             "border-bottom: 1px solid #555; background-color: #222 }"
                             "QLabel { border: 0px; color: white; font-family: Arial;"
                             "font-size: 11px;}");

    QLabel* chatIndex = new QLabel("Chat "+ QString::number(index));
    QLabel* chatName = new QLabel(case1);
    QLabel* infoTime = new QLabel("Time...");
    chatIndex->setFixedHeight(30);
    chatIndex->setStyleSheet("margin-left: 10px");
    chatName->setFixedHeight(30);
    infoTime->setFixedSize(70, 30);
    chatName->setAlignment(Qt::AlignCenter);

    infoChatLayout->addWidget(chatIndex);
    infoChatLayout->addWidget(chatName);
    infoChatLayout->addWidget(infoTime);

    infoChatLayout->setStretch(0,1);
    infoChatLayout->setStretch(1,6);

    // поле для ввода сообщения и его отправки
    QWidget* inputField = new QWidget(this);
    QHBoxLayout* inputFieldLayout = new QHBoxLayout(inputField);
    inputFieldLayout->setContentsMargins(0,0,0,0);
    inputFieldLayout->setAlignment(Qt::AlignLeft);
    inputField->setFixedHeight(50);
    inputField->setStyleSheet("QWidget { border-top: 1px solid #555; "
                              "border-left: 1px solid #555; background-color: #222; }"
                              "QPushButton { width: 60px; height: 30px; border: 1px solid #333; "
                              "border-radius: 8px; font-family: Arial; "
                              "font-size: 12px; margin-right: 15px }"
                              "QPushButton::hover { background-color: #555; }"
                              "QTextEdit { border: 0px; border-radius: 8px; margin-left: 15px}");


    inputMessage = new QTextEdit();
    inputMessage->setFont(font3);
    inputMessage->setFixedHeight(30);
    inputMessage->setPlaceholderText("Введите текст...");

    QPushButton* btnToSendMessage = new QPushButton("Send >>");
    btnToSendMessage->setFont(font1);
    connect(btnToSendMessage, SIGNAL(clicked()), SLOT(sendMessage()));

    inputFieldLayout->addWidget(inputMessage);
    inputFieldLayout->addWidget(btnToSendMessage);

    // поле для сообщений между пользователями
    QWidget* outputField = new QWidget(this);
    outputFieldLayout = new QVBoxLayout(outputField);
    outputFieldLayout->setContentsMargins(0,0,0,0);
    outputFieldLayout->setAlignment(Qt::AlignBottom);
    outputField->setStyleSheet("QWidget {border-left: 1px solid #555; background-color: #222}");

    chatContentLayout->addWidget(infoPanel);
    chatContentLayout->addWidget(outputField);
    chatContentLayout->addWidget(inputField);

    chatContentLayout->setStretch(0,1);
    chatContentLayout->setStretch(1,2);

    setLayout(chatContentLayout);
}

void ChatContent::sendMessage() {
    QString text = inputMessage->toPlainText();
    if(text.length() > 0) {
        messages.push_back(text);

        QWidget* containerMessage = new QWidget(this);
        QVBoxLayout* containerMsgLayout = new QVBoxLayout(containerMessage);
        containerMessage->setStyleSheet("QWidget {border: 0px; color: white}"
                                        "QTextEdit { border: 1px solid #444; "
                                        "border-radius: 7px;}");
        containerMsgLayout->setSpacing(5);
        containerMsgLayout->setContentsMargins(0,0,15,4);
        containerMsgLayout->setAlignment(Qt::AlignRight);

        QTextEdit* message = new QTextEdit(text);
        message->setFixedWidth(350);
        message->setReadOnly(true);
        message->setFont(font3);

        QFontMetrics metrics(font3);
        QSize textSize = metrics.size(Qt::TextSingleLine, text);
        int newHeight = qMax(30, textSize.height() + 10);
        message->setFixedHeight(newHeight);

        containerMsgLayout->addWidget(message);

        if(messagesContainers.isEmpty()) {
            outputFieldLayout->addWidget(containerMessage, 0, Qt::AlignRight | Qt::AlignBottom);
            messagesContainers.push_back(containerMessage);
        } else {
            messagesContainers.push_back(containerMessage);
            for(auto msg : messagesContainers) {
                outputFieldLayout->addWidget(msg, 0, Qt::AlignRight | Qt::AlignBottom);
            }
        }
        inputMessage->clear();
    }
    qDebug() << text;
}
