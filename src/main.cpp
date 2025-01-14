#include <QApplication>
#include "../MainWindow/mainwindow.h"
#include "chatserver.h"

using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    quint16 port = 12345;

    ChatServer server(port);

    Mainwindow mainWin1(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin1.show();

    Mainwindow mainWin2(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin2.show();

    return app.exec();
}
