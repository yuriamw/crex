#include "exchart.h"

#include <QStringList>
#include <QDateTime>

#include <QChart>
#include <QCandlestickSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QCandlestickSet>

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
        {11.7, 12.9, 12.9,  11.5},
        {12.9, 10.3, 12.9,  10.2},
        {10.3,  9.5, 11.25,  9.1},
    };
};

ExChart::ExChart()
{
    setMinimumSize(480, 240);

    QCandlestickSeries *series = new QCandlestickSeries();
    series->setName("Candles");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));

    qreal p = 1000000;
    qreal s = 3600;
    QStringList categories;
    for (size_t i = 0; i < sizeof(Defaults::candles) / sizeof(Defaults::candle_data); i++)
    {
        qreal t = p + s * i;

        QCandlestickSet *cs = new QCandlestickSet(Defaults::candles[i].o, Defaults::candles[i].h, Defaults::candles[i].l, Defaults::candles[i].c, t);

        series->append(cs);
        categories << QDateTime::fromSecsSinceEpoch(t).toString("yyyy-MM-dd hh:mm:ss");
    }
    while (series->count() < 20)
    {
        p -= s;
        categories.insert(0, QDateTime::fromSecsSinceEpoch(p).toString("yyyy-MM-dd hh:mm:ss"));
        series->insert(0, new QCandlestickSet());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Data");
//    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->createDefaultAxes();
    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
}
