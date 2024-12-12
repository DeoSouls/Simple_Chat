#ifndef CHATCONTENT_H
#define CHATCONTENT_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QList>

class ChatContent : public QWidget {
        Q_OBJECT
    public:
        explicit ChatContent(const QString case1, int index, QWidget *parent = nullptr);

        int chatIndex;
    private:
        QList<QString> messages;
        QList<QWidget*> messagesContainers;
        QVBoxLayout* chatContentLayout;
        QVBoxLayout* outputFieldLayout;
        QTextEdit* inputMessage;
        QFont font3;
    private slots:
        void sendMessage();
};

#endif // CHATCONTENT_H
