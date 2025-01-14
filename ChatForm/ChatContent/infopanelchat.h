#ifndef INFOPANELCHAT_H
#define INFOPANELCHAT_H

#include "hoverlabel.h"
#include "chatmessagemodel.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QWebSocket>

class InfoPanelChat : public QWidget {
        Q_OBJECT
    public:
        explicit InfoPanelChat(int index, const QString& email, bool status, const QString& chatName, QWidget *parent = nullptr);

        QPushButton* searchingButton;
        QTextEdit* searchLine;
    private:
        QHBoxLayout* panelLayout;
        QHBoxLayout* searchLayout;
        QPushButton* searchSwitch;
        QPushButton* searchExit;

        bool searchOpen;
        int chatIndex;
    private slots:
        void openTextLineSearch();
        void closeTextLineSearch();
};

#endif // INFOPANELCHAT_H
