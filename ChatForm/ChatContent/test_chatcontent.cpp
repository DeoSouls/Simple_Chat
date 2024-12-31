#include "test_chatcontent.h"
#include "chatmessagemodel.h"
#include <QWebSocket>
#include <QSignalSpy>
#include <QPushButton>
#include <QLineEdit>

TestChatContent::TestChatContent(QObject *parent) : QObject{parent} {}
