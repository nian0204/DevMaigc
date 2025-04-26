#include "dockerwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DockerWidget w;
    w.show();
    return a.exec();
}
