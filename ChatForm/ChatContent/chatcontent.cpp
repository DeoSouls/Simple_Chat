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
#include <QMediaPlayer>
#include <QVideoWidget>

// Конструктор класса ChatContent, создает основной интерфейс чата
ChatContent::ChatContent(const QString& chatName, const QString& selfName, const QString& email, bool status, int index, int id, int user2id, QWebSocket* m_client, QWidget *parent)
    : QWidget{parent}, chatIndex(index), selfName(selfName), userId(id), user2Id(user2id), m_socket(m_client) {

    // Создание основного вертикального макета для размещения элементов интерфейса
    chatContentLayout = new QVBoxLayout(this);
    chatContentLayout->setContentsMargins(0,0,0,0);
    chatContentLayout->setAlignment(Qt::AlignTop);
    chatContentLayout->setSpacing(0);

    // Установка стиля для основного виджета
    setStyleSheet("QWidget { background-color: #444; }");
    font1 = QFont("Segoe UI", 12);

    // Создание модели данных для сообщений
    messageModel = new ChatMessageModel(this);

    // Подключение обработчика входящих сообщений от сервера
    connect(m_socket, &QWebSocket::textMessageReceived, this, &ChatContent::addMessageToChat);

    // Запрос на получение сообщений чата с сервера
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

    // Добавление всех элементов в основной макет
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
    if(type.contains("chat_info")) {
        messagesArray = response["data"].toArray();
        // Обработка массива сообщений
        handleMessagesToChat(messagesArray, true);
        emit messageView->verticalScrollBar()->valueChanged(0);

        // Другой вариант пагинации
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
        msg.timestamp = QDateTime::fromString(created_at);
        qDebug() << msg.timestamp;
        msg.hasImage = !image_data.isEmpty();

        if(msg.hasImage) {
            QByteArray image = QByteArray::fromBase64(image_data.toUtf8());
            msg.imageData = image;
        }

        msg.hasVideo = false; ///////
        if(msg.hasVideo) {
            msg.source = "";
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
        // Обработка массива сообщений
        handleMessagesToChat(messagesArray, false);
    }
}

void ChatContent::handleMessagesToChat(const QJsonArray& messagesArray, bool hasPag) {
    QVector<ChatMessage> messagesPag;
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

        msg.hasVideo = false; ///////
        if(msg.hasVideo) {
            msg.source = "";
        }

        if(hasPag) {
            messagesPag.push_back(msg);
        } else {
            messageModel->addMessage(msg);
        }
    }
    if(hasPag) {
        pagination = splitArray(messagesPag, 10);
    }
}

void ChatContent::addMessageToChat(const QString &message) {
    QJsonDocument message_doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject message_obj = message_doc.object();

    QString typeMessage = message_obj.value("type").toString();
    int chatidFromResponse = message_obj.value("chatid").toInt();
    if((typeMessage.contains("chat_info") || typeMessage.contains("send") || typeMessage.contains("searching")) && chatidFromResponse == chatIndex) {
        loadMessagesFromDatabase(message);
        messageView->scrollToBottom();
    }
}

// Метод для отправки сообщения на сервер
void ChatContent::sendMessage() {
    QString text = inputMessage->toPlainText().trimmed();

    if(text.isEmpty()) {
        return;
    }

    // Обработка прикрепленного изображения
    QByteArray pixmapBytes;
    if(!m_attachedImage.isNull()) {
        QBuffer buffer(&pixmapBytes);
        buffer.open(QIODevice::WriteOnly);

        // Сохраняем pixmap в формате PNG (можно выбрать другой формат, напр. JPG)
        m_attachedImage.save(&buffer, "PNG", 100);

        // Закрываем buffer, хотя после выхода из области видимости он сам закроется
        buffer.close();
    }

    // Создание объекта сообщения для добавления в модель
    ChatMessage msg;
    msg.username = selfName;
    msg.message = text;
    msg.isMine = true;
    msg.timestamp = QDateTime::currentDateTime();
    msg.hasImage = !pixmapBytes.isEmpty();
    if(msg.hasImage) {
        msg.imageData = pixmapBytes;
    }
    msg.hasVideo = !m_mediaSource.isEmpty();
    if(msg.hasVideo) {
        msg.source = m_mediaSource;

        QWidget *containerWidget = new QWidget(messageView);
        QVBoxLayout *layout = new QVBoxLayout(containerWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        // Создаем видеовиджет и плеер
        QVideoWidget *videoWidget = new QVideoWidget(containerWidget);
        QMediaPlayer *player = new QMediaPlayer(containerWidget);

        videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
        videoWidget->setMinimumSize(400, 400);

        player->setVideoOutput(videoWidget);
        player->setSource(QUrl::fromLocalFile(msg.source));

        // Создаем панель управления с кнопкой полноэкранного режима
        QHBoxLayout *controlLayout = new QHBoxLayout();
        controlLayout->setAlignment(Qt::AlignRight); // Выравнивание вправо

        QPushButton *fullScreenButton = new QPushButton(containerWidget);
        fullScreenButton->setIcon(QIcon::fromTheme("view-fullscreen")); // Используйте системную иконку или свою
        fullScreenButton->setFixedSize(32, 32); // Фиксированный размер кнопки
        fullScreenButton->setToolTip("Полноэкранный режим");

        controlLayout->addWidget(fullScreenButton);

        // Добавляем видео и панель управления в основной контейнер
        layout->addWidget(videoWidget);
        layout->addLayout(controlLayout);

        // Устанавливаем контейнер как виджет для индекса
        int newRowIndex = messageModel->rowCount() - 1;
        QModelIndex newIndex = messageModel->index(newRowIndex, 0);
        messageView->setIndexWidget(newIndex, containerWidget);

        // Запускаем воспроизведение
        player->play();

        // Подключаем сигналы и слоты для полноэкранного режима
        connect(fullScreenButton, &QPushButton::clicked, videoWidget, &QVideoWidget::setFullScreen);
        connect(videoWidget, &QVideoWidget::fullScreenChanged, fullScreenButton, &QPushButton::setChecked);
    }

    // Добавление сообщения в модель и отправка на сервер
    messageModel->addMessage(msg);

    // Формирование JSON-объекта для отправки на сервер
    QJsonObject messageObject;
    messageObject["action"] = "send_message";
    messageObject["user_id"] = userId;
    messageObject["selfName"] = selfName;
    messageObject["body"] = text;
    messageObject["chat_id"] = chatIndex;
    if (!pixmapBytes.isEmpty()) {
        messageObject["image_data"] = QString::fromUtf8(pixmapBytes.toBase64());
    }

    // Отправка сообщения на сервер
    QJsonDocument messageDoc(messageObject);
    QString jsonString = messageDoc.toJson();    
    //m_socket->sendTextMessage(jsonString);

    // Отправка запроса на обновление переключателей чатов
    QJsonObject messageObject2;
    messageObject2["action"] = "update_chats";
    messageObject2["userid"] = userId;
    QJsonDocument messageDoc2(messageObject2);
    QString jsonString2 = messageDoc2.toJson();
    //m_socket->sendTextMessage(jsonString2);

    messageView->scrollToBottom();

    // Очистка полей ввода и предпросмотра
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

// Метод для реализации пагинации сообщений при прокрутке вверх
void ChatContent::scrollOnTop(int value) {
    QObject* obj = sender();
    QScrollBar* scroll = qobject_cast<QScrollBar*>(obj);
    int currentScrollValue = scroll->value();
    int previousMaximum = scroll->maximum();

    // Если скролл достиг верха, загружаем следующую порцию сообщений
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

// Демо-представление изображения
void ChatContent::onAttachImageButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.mp4)");
    if (!fileName.isEmpty()) {

        // Обработка видеофайлов
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if(extension == "mp4") {
            m_mediaSource = fileInfo.filePath();
            QMessageBox::information(this, "Информация", "Вы выбрали видеофайл MP4!");
            return;
        }

        // Обработка изображений
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
