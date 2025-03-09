#include "../MainWindow/mainwindow.h"
#include <QApplication>
#include "chatserver.h"

using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);  // Инициализация главного приложения Qt

    quint16 port = 12345;  // Порт сервера (недостаток: жёстко задан)
    ChatServer server(port);  // Запуск сервера на указанном порту

    // Создание первого клиентского окна
    Mainwindow mainWin1(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin1.show();  // Отображение окна

    // Создание второго клиентского окна (для тестирования взаимодействия)
    Mainwindow mainWin2(QUrl(QStringLiteral("ws://localhost:%1").arg(port)));
    mainWin2.show();

    return app.exec();  // Запуск цикла обработки событий
}
