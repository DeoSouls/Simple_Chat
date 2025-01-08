#ifndef INFOPANELCHAT_H
#define INFOPANELCHAT_H

#include "hoverlabel.h"
#include <QWidget>
#include <QHBoxLayout>

class InfoPanelChat : public QWidget {
        Q_OBJECT
    public:
        explicit InfoPanelChat(int index, const QString& email, bool status, const QString& chatName, QWidget *parent = nullptr);
    private:
        QHBoxLayout* panelLayout;
        int chatIndex;
};

#endif // INFOPANELCHAT_H
