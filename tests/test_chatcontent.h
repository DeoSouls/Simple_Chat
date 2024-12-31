#ifndef TEST_CHATCONTENT_H
#define TEST_CHATCONTENT_H

#include <QObject>

class TestChatContent : public QObject {
        Q_OBJECT
    private slots:
        void initTestCase();
        void cleanupTestCase();

        void testInitialState();
        void testAddMessage();
        void testClearMessages();
        void testSendMessage();
};

#endif // TEST_CHATCONTENT_H
