#include <QApplication>
#include "layout.h"
#include <QDesktopWidget>
#include <QRect>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainLayout window;

    window.setWindowTitle("Q Menu");
    window.show();

    return app.exec();
}
