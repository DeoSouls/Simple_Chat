#include "chatcontent.h"
#include "infopanelchat.h"
#include "messageitemdelegate.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QFontMetrics>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QClipboard>
#include <QDebug>
#include <QListView>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QBuffer>

ChatContent::ChatContent(const QString& chatName, int index, int id, QWebSocket* m_client, QWidget *parent) : QWidget{parent}, chatIndex(index), userId(id), m_socket(m_client) {
    chatContentLayout = new QVBoxLayout(this);
    chatContentLayout->setContentsMargins(0,0,0,0);
    chatContentLayout->setAlignment(Qt::AlignTop);
    chatContentLayout->setSpacing(0);
    setStyleSheet("QWidget { background-color: #444; }");

    messageModel = new ChatMessageModel(this);
    loadMessagesFromDatabase();

    // WebSocket
    connect(m_socket, &QWebSocket::textMessageReceived, this, &ChatContent::addMessageToChat);

    font1 = QFont("Segoe UI", 12);
    font3 = QFont("Arial", 11, QFont::Expanded);

    // инфо о чате (верхняя панель)
    InfoPanelChat* infoPanelChat = new InfoPanelChat(index, chatName);

    // поле для ввода сообщения и его отправки
    inputField = new QWidget(this);
    inputFieldLayout = new QHBoxLayout(inputField);
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


    QPushButton* btnToAttachImage = new QPushButton("Image");
    btnToAttachImage->setStyleSheet("QPushButton {margin: 0px; margin-left: 5px;}");
    connect(btnToAttachImage, &QPushButton::clicked, this, &ChatContent::onAttachImageButtonClicked);

    m_imagePreviewLabel = new QLabel();
    m_imagePreviewLabel->setFixedSize(100, 100);
    m_imagePreviewLabel->setStyleSheet("border: 1px solid gray;");
    m_imagePreviewLabel->setScaledContents(true);

    inputMessage = new InputMessage();
    inputMessage->setFont(font3);
    inputMessage->setFixedHeight(40);
    inputMessage->setPlaceholderText("Введите текст...");
    connect(inputMessage, &InputMessage::textChanged, this, &ChatContent::resizeInputField);

    QPushButton* btnToSendMessage = new QPushButton("Send >>");
    btnToSendMessage->setFont(font1);
    connect(btnToSendMessage, SIGNAL(clicked()), SLOT(sendMessage()));

    inputFieldLayout->addWidget(btnToAttachImage);
    inputFieldLayout->addWidget(inputMessage);
    inputFieldLayout->addWidget(btnToSendMessage);

    // Представление для сообщений
    messageView = new QListView(this);
    messageView->setModel(messageModel);
    messageView->setItemDelegate(new MessageItemDelegate(this));

    messageView->setStyleSheet(
        "QListView {"
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

void ChatContent::loadMessagesFromDatabase() {
    messageModel->clearMessage();

    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT users.firstname, messages.user_id, messages.body, messages.created_at, messages.id, messages.image_data "
                        "FROM users "
                        "INNER JOIN messages ON users.id = messages.user_id "
                        "WHERE messages.chat_id = :chatid "
                        "ORDER BY messages.created_at ASC;");
    selectQuery.bindValue(":chatid", chatIndex);

    if (!selectQuery.exec()) {
        qDebug() << "Ошибка выборки сообщений:" << selectQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить сообщения.");
        return;
    }

    while(selectQuery.next()) {
        int id = selectQuery.value("id").toInt();
        QString firstname = selectQuery.value("firstname").toString();
        int user_id = selectQuery.value("user_id").toInt();
        QString body = selectQuery.value("body").toString();
        QDateTime created_at = selectQuery.value("created_at").toDateTime();
        QByteArray image_data = selectQuery.value("image_data").toByteArray();

        ChatMessage msg;
        msg.id = id;
        msg.username = firstname;
        msg.message = body;
        msg.isMine = (user_id == userId);
        msg.timestamp = created_at;
        msg.hasImage = !image_data.isEmpty();

        if(msg.hasImage) {
            msg.imageData = image_data;
        }

        messageModel->addMessage(msg);
    }

}

void ChatContent::addMessageToChat(const QString &message) {
    Q_UNUSED(message);

    loadMessagesFromDatabase();
    messageView->scrollToBottom();
}

void ChatContent::sendMessage() {
    QString text = inputMessage->toPlainText().trimmed();

    if(text.isEmpty()) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if(!db.transaction()) {
        qDebug() << "Не удалось начать транзакцию:" << db.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить сообщение.");
        return;
    }

    QByteArray pixmapBytes;
    if(!m_attachedImage.isNull()) {
        QBuffer buffer(&pixmapBytes);
        buffer.open(QIODevice::WriteOnly);

        // Сохраняем pixmap в формате PNG (можно выбрать другой формат, напр. JPG)
        m_attachedImage.save(&buffer, "PNG", 90);

        // Закрываем buffer, хотя после выхода из области видимости он сам закроется
        buffer.close();
    }


    QSqlQuery query;
    query.prepare("INSERT INTO messages (user_id, body, chat_id, created_at, image_data) "
                  "VALUES (:user_id, :body, :chat_id, :created_at, :image_data) RETURNING id");
    query.bindValue(":user_id", userId);
    query.bindValue(":body", text);
    query.bindValue(":chat_id", chatIndex);
    query.bindValue(":created_at", QDateTime::currentDateTime());
    query.bindValue(":image_data", pixmapBytes);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления сообщения:" << query.lastError().text();
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить сообщение.");
        return;
    }

    query.next();
    int message_id = query.value("id").toInt();

    query.prepare("SELECT CASE WHEN user1_id != :userid THEN user1_id ELSE user2_id "
                  "END AS result FROM chats WHERE (user1_id != :userid OR user2_id != :userid)");
    query.bindValue(":userid", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления сообщения:" << query.lastError().text();
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить сообщение.");
        return;
    }

    query.next();
    int user2_id = query.value("result").toInt();

    query.prepare("INSERT INTO message_read_status (message_id, user_id, is_read) "
                  "VALUES (:message_id, :user_id, :is_read)");
    query.bindValue(":message_id", message_id);
    query.bindValue(":user_id", user2_id);
    query.bindValue(":is_read", false);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления сообщения:" << query.lastError().text();
        db.rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось отправить сообщение.");
        return;
    }

    db.commit();

    ChatMessage msg;
    msg.id = message_id;
    msg.username = "Me";
    msg.message = text;
    msg.isMine = true;
    msg.timestamp = QDateTime::currentDateTime();
    msg.hasImage = !pixmapBytes.isEmpty();

    if(msg.hasImage) {
        msg.imageData = pixmapBytes;
    }

    messageModel->addMessage(msg);

    m_socket->sendTextMessage(text);
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
