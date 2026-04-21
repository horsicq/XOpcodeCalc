#include "xupdate.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XUpdate w;
    w.show();
    return a.exec();
}
