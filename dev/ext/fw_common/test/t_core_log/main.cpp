#include <QCoreApplication>
#include "TestCore.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestCore core;

    return core.exec(argc, argv);
}
