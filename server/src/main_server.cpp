#include <QCoreApplication>
#include "chatserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Создаем сервер на порту 1234 (или любом другом)
    ChatServer server(1234);

    return app.exec();
}
