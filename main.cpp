#include <QApplication>

#include "core/core.h"
#include "mw.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Core core;

    MW mw(core.exchangeProtocol(), core.exchangeWssProtocol(), core.exchangeInfo());
    mw.show();

    return a.exec();
}
