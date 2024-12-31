#include <QApplication>
#include "../MainWindow/mainwindow.h"
#include "chatserver.h"
#include "../ChatForm/ChatContent/chatmessagemodel.h"

using namespace std;
Q_DECLARE_METATYPE(ChatMessage)

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Регистрация мета-типа
    qRegisterMetaType<ChatMessage>("ChatMessage");

    quint16 port = 12345;

    ChatServer server(port);

    Mainwindow mainWin1(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin1.show();

    Mainwindow mainWin2(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin2.show();

    return app.exec();
}
