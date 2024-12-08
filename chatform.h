#ifndef CHATFORM_H
#define CHATFORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class ChatForm : public QWidget {
        Q_OBJECT
    public:
        explicit ChatForm(QWidget *parent = nullptr);
        ~ChatForm();
        QPushButton* btnToLogin;
    private:
        QVBoxLayout* layoutChat;
};

#endif // CHATFORM_H
