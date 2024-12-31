#include "messagecontainer.h"

MessageContainer::MessageContainer(QFont font, QString firstname, QString message, QWidget *parent) : font(font), message(message), firstname(firstname), QWidget{parent} {
    containerMsgLayout = new QVBoxLayout(this);
    setStyleSheet("QWidget { border: 0px; color: white}"
                                    "QLabel { border: 1px solid #444; "
                                    "border-radius: 8px; padding: 5px}");
    containerMsgLayout->setSpacing(5);
    containerMsgLayout->setContentsMargins(15,0,0,4);

    QString wrappedText = message;
    if (message.length() > 35) {
        wrappedText.clear();
        int maxWidth = 35;

        for (int i = 0; i < message.length(); i += maxWidth) {
            wrappedText += message.mid(i, maxWidth) + "\n";
            if((i + 35) > message.length()) {
                wrappedText.chop(1);
            }
        }
    }

    nameLabel = new QLabel(firstname);
    if(firstname != "1") {
        nameLabel->setStyleSheet("border: 0px; padding: 0px");
        nameLabel->setAlignment(containerMsgLayout->alignment());
        containerMsgLayout->addWidget(nameLabel);
    }

    QLabel* messageLabel = new QLabel(wrappedText);
    messageLabel->setFont(font);

    containerMsgLayout->addWidget(messageLabel);
}

void MessageContainer::setAlignment(Qt::Alignment alignment) {
    containerMsgLayout->setAlignment(alignment);
    nameLabel->setAlignment(alignment);
}

