#include "chatcontent.h"
#include "messageitemdelegate.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QJsonValue>
#include <QScrollBar>
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

    connect(m_socket, &QWebSocket::textMessageReceived, this, &ChatContent::addMessageToChat);

    font1 = QFont("Segoe UI", 12);

    QJsonObject aboutChat;
    aboutChat["action"] = "chat_messages";
    aboutChat["chatid"] = chatIndex;
    QJsonDocument doc(aboutChat);
    // Отправка запроса на получение данных чата
    m_socket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    // инфо о чате (верхняя панель)
    infoPanelChat = new InfoPanelChat(index, email, status, chatName);
    connect(infoPanelChat->searchingButton, &QPushButton::clicked, this, &ChatContent::searchingMessage);

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

    // Представление фото
    QPushButton* btnToAttachImage = new QPushButton(this);
    btnToAttachImage->setStyleSheet("QPushButton {margin: 0px; margin-left: 15px;}");
    btnToAttachImage->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconPhotoAp.png"));
    btnToAttachImage->setIconSize(QSize(25,25));
    connect(btnToAttachImage, &QPushButton::clicked, this, &ChatContent::onAttachImageButtonClicked);

    // Виджет для фотографий
    imagePrev = new QWidget();
    imagePrev->setStyleSheet("QWidget { background-color: #222; border-left: 1px solid #444}"
                             "QLabel { border: 1px solid #444; } "
                             "QPushButton { border: 1px solid #222; border-radius: 20px;}"
                             "QPushButton:hover { background-color: #444; }");
    QHBoxLayout* layoutImage = new QHBoxLayout(imagePrev);
    layoutImage->setContentsMargins(10,10,0,10);
    layoutImage->setAlignment(Qt::AlignCenter);
    layoutImage->setSpacing(5);
    m_imagePreviewLabel = new QLabel();
    m_imagePreviewLabel->setFixedSize(400, 400);
    m_imagePreviewLabel->setScaledContents(false);
    QPushButton* button = new QPushButton();
    button->topLevelWidget();
    button->setFixedSize(40,40);
    button->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconClose.png"));
    button->setIconSize(QSize(25,25));
    connect(button, QPushButton::clicked, [this]() {
        imagePrev->hide();
    });
    layoutImage->addWidget(m_imagePreviewLabel);
    layoutImage->addWidget(button);
    imagePrev->hide();

    // Ввод сообщения
    inputMessage = new InputMessage();
    inputMessage->setFont(font1);
    inputMessage->setFixedHeight(40);
    inputMessage->setPlaceholderText("Введите текст...");
    connect(inputMessage, &InputMessage::textChanged, this, &ChatContent::resizeInputField);

    // Отправка сообщения
    QPushButton* btnToSendMessage = new QPushButton(this);
    btnToSendMessage->setFont(font1);
    btnToSendMessage->setIcon(QIcon("C://cpp//projectsQt//SimpleChat//resources//iconSend.png"));
    btnToSendMessage->setIconSize(QSize(20,20));
    connect(btnToSendMessage, &QPushButton::clicked, this, &ChatContent::sendMessage);

    inputFieldLayout->addWidget(btnToAttachImage);
    inputFieldLayout->addWidget(inputMessage);
    inputFieldLayout->addWidget(btnToSendMessage);

    // Представление для сообщений
    messageView = new QListView(this);
    messageView->setModel(messageModel);
    messageDelegate = new MessageItemDelegate(this);
    messageView->setItemDelegate(messageDelegate);

    messageView->setStyleSheet(
        "QListView {"
        "background-image: url(C://cpp//projectsQt//SimpleChat//resources//phoneZoom.jpg);"
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
    connect(verticalScrollBar, QScrollBar::valueChanged, this, &ChatContent::scrollOnTop);
    messageView->scrollToBottom();
    messageView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    messageView->setContentsMargins(0, 0, 0, 0);

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
    chatContentLayout->addWidget(imagePrev);

    setLayout(chatContentLayout);
}

void ChatContent::loadMessagesFromDatabase(const QString &message) {
    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response = message_doc.object();
    QString type = response.value("type").toString();

    QJsonArray messagesArray;
    QVector<ChatMessage> messagesPag;
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

            messagesPag.push_back(msg);
            // Реализация без пагинации
            // messageModel->addMessage(msg);
        }

        pagination = splitArray(messagesPag, 10);

        emit messageView->verticalScrollBar()->valueChanged(0);

        // if (!pagination.isEmpty()) {
        //     QVector<ChatMessage> lastPage = pagination.last();

        //     for(const auto& msg : lastPage) {
        //         messageModel->addMessage(msg);
        //     }
        //     pagination.pop_back();
        // }

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
    } else if (type.contains("searching")) {
        allMessages = messageModel->getMessages();
        messageModel->clearMessage();

        messagesArray = response.value("data").toArray();
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
    }
}

void ChatContent::addMessageToChat(const QString &message) {
    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject message_obj = message_doc.object();

    QString typeMessage = message_obj.value("type").toString();
    int chatidFromResponse = message_obj.value("chatid").toInt();
    if((typeMessage.contains("chat_info") || typeMessage.contains("send") || typeMessage.contains("searching")) && chatidFromResponse == chatIndex) {
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
    m_attachedImage = QPixmap();
    imagePrev->hide();
}

void ChatContent::searchingMessage() {
    QString searchText = infoPanelChat->searchLine->toPlainText();
    if(!searchText.isEmpty()) {
        QJsonObject reqObject;
        reqObject["action"] = "searching";
        reqObject["chat_id"] = chatIndex;
        reqObject["search"] = searchText;
        QJsonDocument reqDoc(reqObject);

        QString jsonString = reqDoc.toJson(QJsonDocument::Compact);

        m_socket->sendTextMessage(jsonString);
    } else {
        if(allMessages.isEmpty()) {
            return;
        }

        messageModel->loadMessages(allMessages);
        messageView->scrollToBottom();
    }
}

void ChatContent::resizeInputField() {
    inputField->setFixedHeight(inputMessage->height() + 10);
}

template <typename T>
QVector<QVector<T>> ChatContent::splitArray(const QVector<T>& vector, int chunkSize) {
    QVector<QVector<T>> result;

    if (chunkSize <= 0) {
        QMessageBox::critical(nullptr, "Ошибка", "Размер группы должен быть больше 0");
        return {};
    }

    for (int i = 0; i < vector.size(); i += chunkSize) {
        // Используем mid() для извлечения подмассива
        result.append(vector.mid(i, chunkSize));
    }

    return result;
}

void ChatContent::scrollOnTop(int value) {
    QObject* obj = sender();
    QScrollBar* scroll = qobject_cast<QScrollBar*>(obj);
    int currentScrollValue = scroll->value();
    int previousMaximum = scroll->maximum();
    if(value == 0) {
        qDebug() << "Скрол наверху";
        if (!pagination.isEmpty()) {
            QVector<ChatMessage> lastPage = pagination.last();

            messageModel->prependMessages(lastPage);
            pagination.pop_back();

             QCoreApplication::processEvents();

            // Восстанавливаем позицию скроллбара
            int newMaximum = scroll->maximum();
            scroll->setValue(currentScrollValue + (newMaximum - previousMaximum));
        }
    }
}

void ChatContent::onAttachImageButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        imagePrev->show();
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            m_attachedImage = pixmap.scaled(m_imagePreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_imagePreviewLabel->setPixmap(m_attachedImage);
            m_imagePreviewLabel->setFixedSize(m_attachedImage.width()+1, m_attachedImage.height()+1);
        } else {
            QMessageBox::warning(this, "Invalid Image", "The selected file is not a valid image.");
        }
    }
}
