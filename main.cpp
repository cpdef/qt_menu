#include <QApplication>
#include "layout.h"
#include <QDesktopWidget>
#include <QRect>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDesktopWidget dw;
    QRect r = dw.availableGeometry();
    MainLayout window;

    window.setWindowTitle("Q Menu");
    window.setFixedSize(r.width()*0.9, r.height()*0.9);
    window.show();

    return app.exec();
}
