#include <QApplication>
#include "DemoWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DemoWidget w;
    w.resize(800, 600);
    w.show();

    return app.exec();
}
