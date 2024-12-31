#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class MessageContainer : public QWidget {
        Q_OBJECT
    public:
        explicit MessageContainer(QFont font, QString firstname, QString message, QWidget *parent = nullptr);

        QString firstname;
        QString message;
        QFont font;
        QLabel* nameLabel;
        void setAlignment(Qt::Alignment);
    private:
        QVBoxLayout* containerMsgLayout;
};

#endif // MESSAGECONTAINER_H
