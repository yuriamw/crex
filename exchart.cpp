#include <iostream>
#include "exchart.h"

#include <QStringList>
#include <QDate>
#include <QDateTime>

#include <QChart>
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QFont>
//#include <QCursor>

#include <QGraphicsView>

#include "logger.h"

using namespace QtCharts;

namespace Defaults {

    static const qint64 WIDTH    = 6;
    static const qint64 SPACING  = 4;
    static const qint64 RMARGIN  = 40;

    struct candle_data {
        qreal l;
        qreal o;
        qreal c;
        qreal h;
    };

    struct candle_data candlerand()
    {
        qint64 a = 800;
        qint64 b = 1400;
        qreal v[4] = {
            (a + qrand() % (b - a)) / 100.0,
            (a + qrand() % (b - a)) / 100.0,
            (a + qrand() % (b - a)) / 100.0,
            (a + qrand() % (b - a)) / 100.0,
        };
        int min = 0;
        for (int i = 0; i < 4; i++)
        {
            if (v[i] < v[min])
            {
                min = i;
            }
        }
        int max = 0;
        for (int i = 0; i < 4; i++)
        {
            if (v[i] > v[max])
            {
                max = i;
            }
        }
        qreal m = v[0];
        v[0] = v[min];
        v[min] = m;
        m = v[3];
        v[3] = v[max];
        v[max] = m;

        struct candle_data d = { v[0], v[1], v[2], v[3] };

        return d;
    }
}


static qint64 column_width()
{
    return Defaults::WIDTH + Defaults::SPACING;
}

ExChart::ExChart()
{
    setMinimumSize(280, 500);
//    QCursor c = cursor();
//    setCursor(c);

    QCandlestickSeries *series = new QCandlestickSeries();
//    series->setName("Candles");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    series->setBodyOutlineVisible(false);
    series->setMaximumColumnWidth(Defaults::WIDTH);
    series->setMinimumColumnWidth(Defaults::WIDTH);


    for (size_t i = 0; i < 1024; i++)
    {
        QDateTime now = QDateTime(QDateTime::currentDateTimeUtc().date())/*.addDays(-1)*/;
        QDateTime dt = now.addDays(-i);
        struct Defaults::candle_data d = Defaults::candlerand();

        QCandlestickSet *cs = new QCandlestickSet(d.o, d.h, d.l, d.c, dt.toMSecsSinceEpoch());

        series->append(cs);
    }
//    size_t i = 1;
//    while (series->count() < 20)
//    {
//        qint64 t = 1000000 - 3600 * i;
//        QCandlestickSet *cs = new QCandlestickSet();
//        cs->setTimestamp(QDateTime::fromSecsSinceEpoch(t, Qt::UTC).toMSecsSinceEpoch());

//        series->insert(0, cs);
//        i++;
//    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Data");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    TRACE("") << axisX->tickCount();
    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
    axisX->setLabelsAngle(90);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(7, 15);
    chart->addAxis(axisY, Qt::AlignRight);

    QFont labelFont;
    labelFont.setPixelSize(8);
    axisX->setLabelsFont(labelFont);
    axisY->setLabelsFont(labelFont);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    setChart(chart);
//    setRenderHint(QPainter::Antialiasing);

    connect(series, &QCandlestickSeries::hovered, this, &ExChart::onHover);

//    std::cerr << QLocale::system().name().toStdString() << std::endl;
//    std::cerr << QLocale().name().toStdString() << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
/// Exvents

void ExChart::resizeEvent(QResizeEvent *event)
{
    QDateTime maxD = QDateTime(QDateTime::currentDateTimeUtc().date().addDays(1));
    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    axisX->setTickCount((width() / column_width()) / 4);
    axisX->setMin(maxD.addDays(-(axisX->tickCount() + 1)));
    axisX->setMax(maxD);
    TRACE("") << axisX->min() << "<->" << axisX->max();

    QChartView::resizeEvent(event);
}

void ExChart::mouseMoveEvent(QMouseEvent *event)
{
    TRACE("") << event;
    if (event->buttons() & Qt::LeftButton)
    {
        TRACE("move");
    }
    QChartView::mouseMoveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void ExChart::onHover(bool status, QCandlestickSet *set)
{
    TRACE("") << status << QDateTime::fromMSecsSinceEpoch(set->timestamp(), Qt::UTC) << set->open() << set->low() << set->high() << set->close();
}

