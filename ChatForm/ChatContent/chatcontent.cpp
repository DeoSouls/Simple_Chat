#include "chatcontent.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFontMetrics>
#include <QScrollArea>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDebug>

ChatContent::ChatContent(const QString case1, int index, int id, QWidget *parent) : QWidget{parent}, chatIndex(index), userId(id) {
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
    infoPanel->setStyleSheet("QWidget {"
                             "border-bottom: 1px solid #444; background-color: #222 }"
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
    inputField = new QWidget(this);
    QHBoxLayout* inputFieldLayout = new QHBoxLayout(inputField);
    inputFieldLayout->setContentsMargins(0,0,0,0);
    inputFieldLayout->setAlignment(Qt::AlignLeft);
    inputField->setFixedHeight(50);
    inputField->setStyleSheet("QWidget {border-top: 1px solid #444; "
                              "border-left: 1px solid #444; background-color: #222; }"
                              "QPushButton { width: 60px; height: 30px; border: 1px solid #333; "
                              "border-radius: 8px; font-family: Arial; "
                              "font-size: 12px; margin-right: 15px }"
                              "QPushButton::hover { background-color: #555; }"
                              "QTextEdit { border: 0px; border-radius: 8px; margin-left: 15px}");


    inputMessage = new InputMessage();
    inputMessage->setFont(font3);
    inputMessage->setFixedHeight(40);
    inputMessage->setPlaceholderText("Введите текст...");
    connect(inputMessage, &InputMessage::textChanged, this, &ChatContent::resizeInputField);

    QPushButton* btnToSendMessage = new QPushButton("Send >>");
    btnToSendMessage->setFont(font1);
    connect(btnToSendMessage, SIGNAL(clicked()), SLOT(sendMessage()));

    inputFieldLayout->addWidget(inputMessage);
    inputFieldLayout->addWidget(btnToSendMessage);

    // прокрутка для поля сообщений между пользователями
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: #222;"
        "    width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #333;"
        "    border-radius: 5px;"
        "    min-height: 20px;"
        "}"
    );

    // поле для сообщений между пользователями
    QWidget* outputField = new QWidget(this);
    outputFieldLayout = new QVBoxLayout(outputField);
    outputFieldLayout->setContentsMargins(0,0,0,0);
    outputFieldLayout->setAlignment(Qt::AlignBottom);
    outputFieldLayout->setSpacing(0);
    outputField->setStyleSheet("QWidget {border: 0px; border-left: 1px solid #444;background-color: #222}");

    scrollArea->setWidget(outputField);

    chatContentLayout->addWidget(infoPanel);
    chatContentLayout->addWidget(scrollArea);
    chatContentLayout->addWidget(inputField);

    setLayout(chatContentLayout);
    addMessageToChat();
}

void ChatContent::addMessageToChat() {
    QSqlDatabase::database().transaction();
    QSqlQuery selectQuery;

    selectQuery.prepare("SELECT body, user_id FROM messages WHERE chatid = :chatid");
    selectQuery.bindValue(":chatid", chatIndex);

    if (!selectQuery.exec()) {
        qDebug() << "Ошибка проверки email:" << selectQuery.lastError().text();
        QSqlDatabase::database().rollback();
    } else {
        while(selectQuery.next()) {
            if(selectQuery.value("user_id") == userId) {
                MessageContainer* msgContainer = new MessageContainer(font3,selectQuery.value("body").toString());
                msgContainer->setAlignment(Qt::AlignRight);
                outputFieldLayout->addWidget(msgContainer, 0, Qt::AlignRight | Qt::AlignBottom);
            } else {
                MessageContainer* msgContainer = new MessageContainer(font3,selectQuery.value("body").toString());
                msgContainer->setAlignment(Qt::AlignLeft);
                outputFieldLayout->addWidget(msgContainer, 0, Qt::AlignLeft | Qt::AlignBottom);
            }
        }
        qDebug() << "Количество найденных: " << selectQuery.size();
        QSqlDatabase::database().commit();
    }
}

void ChatContent::sendMessage() {
    QString text = inputMessage->toPlainText();
    if(text.length() > 0) {
        MessageContainer* msgContainer = new MessageContainer(font3,text);
        msgContainer->setAlignment(Qt::AlignRight);

        outputFieldLayout->setAlignment(Qt::AlignTop);
        outputFieldLayout->addWidget(msgContainer, 0);
        outputFieldLayout->setAlignment(Qt::AlignBottom);

        QSqlDatabase::database().transaction();
        QSqlQuery query;

        query.prepare("INSERT INTO messages (user_id, body, chatid) "
                      "VALUES (:user_id, :body, :chatid) RETURNING id");
        query.bindValue(":user_id", userId);
        query.bindValue(":body", text);
        query.bindValue(":chatid", chatIndex);

        if (!query.exec()) {
            qDebug() << "Ошибка добавления сообщения:" << query.lastError().text();
            QSqlDatabase::database().rollback(); // Откат транзакции
        } else {
            qDebug() << "Сообщение успешно добавлено!";
            QSqlDatabase::database().commit(); // Подтверждение транзакции
            inputMessage->clear();
        }
    }

    // -- проблема 1 - sroll должен про поялвлении нового сообщения спускаться вниз
    QScrollBar* scrollBar = scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
    // if (scrollBar->value() == scrollBar->maximum()) {
    //     QTimer::singleShot(0, this, [&]() {
    //         scrollBar->setValue(scrollBar->maximum());
    //     });
    // }
    // -- проблема 1 - sroll должен про поялвлении нового сообщения спускаться вниз
    qDebug() << text;
}

void ChatContent::resizeInputField() {
    inputField->setFixedHeight(inputMessage->height() + 10);
}

ChatContent::~ChatContent() {
}
