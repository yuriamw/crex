#include "exchart.h"

#include <QStringList>
#include <QDateTime>

#include <QChart>
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QDateTimeAxis>
#include <QValueAxis>

#include "logger.h"

using namespace QtCharts;

namespace Defaults {
    struct candle_data {
        qreal o;
        qreal c;
        qreal h;
        qreal l;
    };

    static const struct candle_data candles[] = {
        {11.2, 11.7, 11.9,  10.0},
        {11.7, 12.9, 14.1,  11.5},
        {12.9, 10.3, 12.9,  10.2},
        {10.3,  9.5, 11.25,  9.1},
    };
};

ExChart::ExChart()
{
    setMinimumSize(800, 320);

    QCandlestickSeries *series = new QCandlestickSeries();
    series->setName("Candles");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    series->setBodyOutlineVisible(false);
    series->setMaximumColumnWidth(6);
    series->setMinimumColumnWidth(6);

    TRACE("") << QDateTime::fromSecsSinceEpoch(1000000 + 0);
    TRACE("") << QDateTime::fromSecsSinceEpoch(1000000 + 3600);
    TRACE("") << QDateTime::fromSecsSinceEpoch(1000000 + 7200);

    for (size_t i = 0; i < sizeof(Defaults::candles) / sizeof(Defaults::candle_data); i++)
    {
        qint64 t = 1000000 + 3600 * i;
        QCandlestickSet *cs = new QCandlestickSet(Defaults::candles[i].o, Defaults::candles[i].h, Defaults::candles[i].l, Defaults::candles[i].c, t);

        series->append(cs);
    }
//    size_t i = 1;
//    while (series->count() < 20)
//    {
//        qint64 t = 1000000 - 3600 * i;
//        QCandlestickSet *cs = new QCandlestickSet();
//        cs->setTimestamp(QDateTime::fromSecsSinceEpoch(t).toSecsSinceEpoch());

//        series->insert(0, cs);
//        i++;
//    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Data");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd-MM-yyyy hh:mm:ss");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignRight);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    setChart(chart);
//    setRenderHint(QPainter::Antialiasing);
}
