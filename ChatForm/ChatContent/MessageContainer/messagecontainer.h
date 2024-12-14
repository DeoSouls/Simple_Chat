#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class MessageContainer : public QWidget {
        Q_OBJECT
    public:
        explicit MessageContainer(QFont font, QString message, QWidget *parent = nullptr);

        QString message;
        QFont font;
        void setAlignment(Qt::Alignment);

        ~MessageContainer();
    private:
        QVBoxLayout* containerMsgLayout;
};

#endif // MESSAGECONTAINER_H
