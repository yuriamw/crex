#include <QApplication>

#include "core/core.h"
#include "mw.h"

#include "widgets/exchartview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Core core;

    crex::ch::ExChartView cw("ExChart");
    cw.show();

//    MW mw(core.exchangeProtocol(), core.exchangeInfo());
//    mw.show();

    return a.exec();
}
