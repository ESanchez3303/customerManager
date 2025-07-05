#include "customermanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    customerManager w;
    w.show();
    return a.exec();
}
