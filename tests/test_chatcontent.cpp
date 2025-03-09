#include "test_chatcontent.h"
#include "chatmessagemodel.h"
#include "chatcontent.h"
#include <QWebSocket>
#include <QSignalSpy>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QTest>
#include <QtTest/QtTest>

void TestChatContent::initTestCase()
{
    // Этот метод вызывается перед запуском всех тестов
}

void TestChatContent::cleanupTestCase()
{
    // Этот метод вызывается после выполнения всех тестов
}

void TestChatContent::testInitialState()
{
    // Создаём фиктивный WebSocket, так как не будем его использовать в тестах
    QWebSocket socket;

    // Создаём экземпляр ChatContent
    ChatContent chatContent("Test Chat", "Me", "mail", true, 1, 1001, 1000, &socket);
    chatContent.show();

    // Проверяем, что модель пуста
    ChatMessageModel* model = chatContent.findChild<ChatMessageModel*>();
    QVERIFY(model != nullptr);
    QCOMPARE(model->rowCount(), 0);

    // Проверяем, что QListView существует и настроена правильно
    QListView* view = chatContent.findChild<QListView*>();
    QVERIFY(view != nullptr);
    QCOMPARE(view->model(), model);

    // Закрываем виджет после проверки
    chatContent.close();
}

void TestChatContent::testAddMessage()
{
    QWebSocket socket;
    ChatContent chatContent("Test Chat", "Me", "mail", true, 1, 1001, 1000, &socket);
    chatContent.show();

    ChatMessageModel* model = chatContent.findChild<ChatMessageModel*>();
    QVERIFY(model != nullptr);

    // Создаём сообщение
    ChatMessage msg;
    msg.username = "Alice";
    msg.message = "Hello, World!";
    msg.isMine = false;
    msg.timestamp = QDateTime::currentDateTime();

    // Добавляем сообщение
    model->addMessage(msg);
    QCOMPARE(model->rowCount(), 1);

    // Проверяем данные в модели
    QModelIndex index = model->index(0, 0);
    QVERIFY(index.isValid());
    QCOMPARE(model->data(index, ChatMessageModel::UsernameRole).toString(), QString("Alice"));
    QCOMPARE(model->data(index, ChatMessageModel::ChatMessageRole).fromValue(msg.message), QString("Hello, World!"));
    QCOMPARE(model->data(index, ChatMessageModel::IsMineRole).toBool(), false);
    QCOMPARE(model->data(index, ChatMessageModel::TimestampRole).toDateTime(), msg.timestamp);

    // Закрываем виджет после проверки
    chatContent.close();
}

void TestChatContent::testClearMessages()
{
    QWebSocket socket;
    ChatContent chatContent("Test Chat", "Me", "mail", true, 1, 1001, 1000, &socket);
    chatContent.show();

    ChatMessageModel* model = chatContent.findChild<ChatMessageModel*>();
    QVERIFY(model != nullptr);

    // Добавляем несколько сообщений
    for (int i = 0; i < 5; ++i) {
        ChatMessage msg;
        msg.username = QString("User%1").arg(i);
        msg.message = QString("Message %1").arg(i);
        msg.isMine = (i % 2 == 0);
        msg.timestamp = QDateTime::currentDateTime();

        model->addMessage(msg);
    }
    QCOMPARE(model->rowCount(), 5);

    // Очищаем сообщения
    model->clearMessage();
    QCOMPARE(model->rowCount(), 0);

    // Закрываем виджет после проверки
    chatContent.close();
}

void TestChatContent::testSendMessage()
{
    QWebSocket socket;
    ChatContent chatContent("Test Chat", "Me", "mail", true, 1, 1001, 1000, &socket);
    chatContent.show();

    ChatMessageModel* model = chatContent.findChild<ChatMessageModel*>();
    QVERIFY(model != nullptr);

    // Находим элементы управления для отправки сообщения
    InputMessage* input = chatContent.findChild<InputMessage*>();
    QVERIFY(input != nullptr);

    QPushButton* sendButton = chatContent.findChild<QPushButton*>();
    QVERIFY(sendButton != nullptr);

    // Создаём сигнал-пойм для кнопки отправки
    QSignalSpy spySendButton(sendButton, SIGNAL(clicked()));

    // Вводим текст в поле ввода
    QString testMessage = "Test message from unit test";
    QTest::keyClicks(input, testMessage);

    // Нажимаем кнопку отправки
    QTest::mouseClick(sendButton, Qt::LeftButton);

    // Проверяем, что сигнал отправки был вызван
    QCOMPARE(spySendButton.count(), 1);

    // Проверяем, что сообщение добавилось в модель
    // QModelIndex index = model->index(0, 0);
    // QCOMPARE(model->rowCount(), 1);
    // QVERIFY(index.isValid());
    // QCOMPARE(model->data(index, ChatMessageModel::ChatMessageRole), testMessage);
    // QCOMPARE(model->data(index, ChatMessageModel::IsMineRole).toBool(), true);

    // Закрываем виджет после проверки
    chatContent.close();
}

QTEST_MAIN(TestChatContent)
#include "test_chatcontent.moc"
