#include <QApplication>
#include <QStyleFactory>
#include "graphicsview.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //QApplication::setStyle(QStyleFactory::create("fusion"));
    GraphicsView view;
    view.show();
    return app.exec();
}
