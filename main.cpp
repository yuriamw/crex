#include <QApplication>

#include "core/core.h"
#include "mw.h"

#include "widgets/exchartwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Core core;

    ExChartWidget cw;
    cw.view()->setWindowTitle("Chart");
    cw.view()->show();

//    MW mw(core.exchangeProtocol(), core.exchangeInfo());
//    mw.show();

    return a.exec();
}
