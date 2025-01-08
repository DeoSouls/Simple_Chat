#include "chatcontent.h"
#include "infopanelchat.h"
#include "messageitemdelegate.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QJsonValue>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QClipboard>
#include <QDebug>
#include <QListView>
#include <QFileDialog>
#include <QApplication>
#include <QBuffer>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

ChatContent::ChatContent(const QString& chatName, const QString& email, bool status, int index, int id, QWebSocket* m_client, QWidget *parent) : QWidget{parent}, chatIndex(index), userId(id), m_socket(m_client) {
    chatContentLayout = new QVBoxLayout(this);
    chatContentLayout->setContentsMargins(0,0,0,0);
    chatContentLayout->setAlignment(Qt::AlignTop);
    chatContentLayout->setSpacing(0);
    setStyleSheet("QWidget { background-color: #444; }");

    messageModel = new ChatMessageModel(this);
    // loadMessagesFromDatabase();

    // WebSocket
    connect(m_socket, &QWebSocket::textMessageReceived, this, &ChatContent::addMessageToChat);

    font1 = QFont("Segoe UI", 12);

    QJsonObject aboutChat;
    aboutChat["action"] = "chat_messages";
    aboutChat["chatid"] = chatIndex;
    QJsonDocument doc(aboutChat);

    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    // инфо о чате (верхняя панель)
    InfoPanelChat* infoPanelChat = new InfoPanelChat(index, email, status, chatName);

    // поле для ввода сообщения и его отправки
    inputField = new QWidget(this);
    inputFieldLayout = new QHBoxLayout(inputField);
    inputFieldLayout->setContentsMargins(0,0,0,0);
    inputFieldLayout->setAlignment(Qt::AlignLeft);
    inputField->setFixedHeight(50);
    inputField->setStyleSheet("QWidget {border-top: 1px solid #444; "
                              "border-left: 1px solid #444; background-color: #222; }"
                              "QPushButton { width: 40px; height: 40px; border: 1px solid #222; "
                              "border-radius: 20px; font-family: Arial; "
                              "font-size: 12px; margin-right: 15px }"
                              "QPushButton::hover { background-color: #555; }"
                              "QTextEdit { border: 0px; border-radius: 8px; "
                              "margin-left: 15px; padding-top:5px;}");


    QPushButton* btnToAttachImage = new QPushButton(this);
    btnToAttachImage->setStyleSheet("QPushButton {margin: 0px; margin-left: 15px;}");
    btnToAttachImage->setIcon(QIcon("C://Users//Purik//Downloads//iconPhotoAp.png"));
    btnToAttachImage->setIconSize(QSize(25,25));
    connect(btnToAttachImage, &QPushButton::clicked, this, &ChatContent::onAttachImageButtonClicked);

    m_imagePreviewLabel = new QLabel();
    m_imagePreviewLabel->setFixedSize(100, 100);
    m_imagePreviewLabel->setStyleSheet("border: 1px solid gray;");
    m_imagePreviewLabel->setScaledContents(true);

    inputMessage = new InputMessage();
    inputMessage->setFont(font1);
    inputMessage->setFixedHeight(40);
    inputMessage->setPlaceholderText("Введите текст...");
    connect(inputMessage, &InputMessage::textChanged, this, &ChatContent::resizeInputField);

    QPushButton* btnToSendMessage = new QPushButton(this);
    btnToSendMessage->setFont(font1);
    btnToSendMessage->setIcon(QIcon("C://Users//Purik//Downloads//iconSend.png"));
    btnToSendMessage->setIconSize(QSize(20,20));
    connect(btnToSendMessage, &QPushButton::clicked, this, &ChatContent::sendMessage);

    inputFieldLayout->addWidget(btnToAttachImage);
    inputFieldLayout->addWidget(inputMessage);
    inputFieldLayout->addWidget(btnToSendMessage);

    // Представление для сообщений
    messageView = new QListView(this);
    messageView->setModel(messageModel);
    messageView->setItemDelegate(new MessageItemDelegate(this));

    messageView->setStyleSheet(
        "QListView {"
        "background-image: url(C://Users//Purik//Downloads//phoneZoom.jpg);"
        "background-color: #222;"
        "color: #252424;"
        "border: none;"
        "padding: 5px;"
        "border-left: 1px solid #333"
        "}"
        "QScrollBar:vertical {"
        "background-color: #222;"
        "width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "background-color: #333;"
        "min-height: 10px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "height: 10px;"
        "background-color: #222;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "height: 10px;"
        "background-color: #222;"
        "}"
        );
    messageView->setSpacing(5);
    messageView->setUniformItemSizes(false);
    messageView->setResizeMode(QListView::Adjust);
    messageView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    messageView->setWordWrap(true);
    messageView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    messageView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    QScrollBar *verticalScrollBar = messageView->verticalScrollBar();
    verticalScrollBar->setSingleStep(20);
    messageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageView->setContentsMargins(0, 0, 0, 0);
    messageView->scrollToBottom();

    connect(messageView, &QListView::activated, [&](const QModelIndex &index) {
        // Копируем текст в буфер обмена, если элемент выделен
        if (index.isValid()) {
            QVariant data = index.data(ChatMessageModel::ChatMessageRole);

            ChatMessage msg = data.value<ChatMessage>();
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(msg.message);

            qDebug() << "Copied to clipboard:" << msg.message;
        }
    });

    chatContentLayout->addWidget(infoPanelChat);
    chatContentLayout->addWidget(messageView);
    chatContentLayout->addWidget(inputField);

    setLayout(chatContentLayout);
}

void ChatContent::loadMessagesFromDatabase(const QString &message) {

    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response = message_doc.object();
    QString type = response.value("type").toString();

    QJsonArray messagesArray;
    if(type.contains("chat_info")) {
        messagesArray = response["data"].toArray();
        for(const QJsonValue& value : messagesArray) {
            QJsonObject messageObj = value.toObject();

            QString firstname = messageObj["firstname"].toString();
            int user_id = messageObj["user_id"].toInt();
            QString body = messageObj["body"].toString();
            QString created_at = messageObj["created_at"].toString();
            QString image_data = messageObj["image_data"].toString();

            ChatMessage msg;
            msg.username = firstname;
            msg.message = body;
            msg.isMine = (user_id == userId);
            msg.timestamp = QDateTime::fromString(created_at, Qt::ISODateWithMs);
            msg.hasImage = !image_data.isEmpty();

            if(msg.hasImage) {
                QByteArray image = QByteArray::fromBase64(image_data.toUtf8());
                msg.imageData = image;
            }

            messageModel->addMessage(msg);
        }
    } else if (type.contains("send")) {
        QString firstname = response["firstname"].toString();
        int user_id = response["user_id"].toInt();
        QString body = response["body"].toString();
        QString created_at = response["created_at"].toString();
        QString image_data = response["image_data"].toString();

        ChatMessage msg;
        msg.username = firstname;
        msg.message = body;
        msg.isMine = (user_id == userId);
        qDebug() << created_at;
        msg.timestamp = QDateTime::fromString(created_at, Qt::ISODateWithMs);
        msg.hasImage = !image_data.isEmpty();

        if(msg.hasImage) {
            QByteArray image = QByteArray::fromBase64(image_data.toUtf8());
            msg.imageData = image;
        }

        messageModel->addMessage(msg);

        QJsonObject messageObj;
        messageObj["action"] = "update_chats";
        messageObj["userid"] = userId;

        QJsonDocument mesDoc(messageObj);
        QString jsonString2 = mesDoc.toJson(QJsonDocument::Compact);

        m_socket->sendTextMessage(jsonString2);
    }
}

void ChatContent::addMessageToChat(const QString &message) {
    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject message_obj = message_doc.object();

    QString typeMessage = message_obj.value("type").toString();
    int chatidFromResponse = message_obj.value("chatid").toInt();
    if((typeMessage.contains("chat_info") || typeMessage.contains("send")) && chatidFromResponse == chatIndex) {
        loadMessagesFromDatabase(message);
    }
    messageView->scrollToBottom();
}

void ChatContent::sendMessage() {
    QString text = inputMessage->toPlainText().trimmed();

    if(text.isEmpty()) {
        return;
    }

    QByteArray pixmapBytes;
    if(!m_attachedImage.isNull()) {
        QBuffer buffer(&pixmapBytes);
        buffer.open(QIODevice::WriteOnly);

        // Сохраняем pixmap в формате PNG (можно выбрать другой формат, напр. JPG)
        m_attachedImage.save(&buffer, "PNG", 100);

        // Закрываем buffer, хотя после выхода из области видимости он сам закроется
        buffer.close();
    }

    ChatMessage msg;
    msg.username = "Me";
    msg.message = text;
    msg.isMine = true;
    msg.timestamp = QDateTime::currentDateTime();
    msg.hasImage = !pixmapBytes.isEmpty();
    if(msg.hasImage) {
        msg.imageData = pixmapBytes;
    }
    messageModel->addMessage(msg);

    QJsonObject messageObject;
    messageObject["action"] = "send_message";
    messageObject["user_id"] = userId;
    messageObject["body"] = text;
    messageObject["chat_id"] = chatIndex;
    if (!pixmapBytes.isEmpty()) {
        messageObject["image_data"] = QString::fromUtf8(pixmapBytes.toBase64());
    }
    QJsonDocument messageDoc(messageObject);
    QString jsonString = messageDoc.toJson();

    m_socket->sendTextMessage(jsonString);

    messageView->scrollToBottom();
    inputMessage->clear();

    m_imagePreviewLabel->clear();
    m_imagePreviewLabel->hide();
    m_attachedImage = QPixmap();
    chatContentLayout->removeWidget(m_imagePreviewLabel);
}

void ChatContent::resizeInputField() {
    inputField->setFixedHeight(inputMessage->height() + 10);
}

void ChatContent::onAttachImageButtonClicked() {
    chatContentLayout->addWidget(m_imagePreviewLabel);
    m_imagePreviewLabel->show();
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            m_attachedImage = pixmap.scaled(m_imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_imagePreviewLabel->setPixmap(m_attachedImage);
        } else {
            QMessageBox::warning(this, "Invalid Image", "The selected file is not a valid image.");
        }
    }
}
