#include <QApplication>
#include "mainwindow.h"

using namespace std;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Mainwindow mainWin;
    mainWin.show();

    return app.exec();
}
