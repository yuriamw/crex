#include <iostream>
#include <algorithm>

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
#include <QPoint>
//#include <QCursor>

#include <QGraphicsView>

#include "logger.h"

using namespace QtCharts;

namespace Defaults {

    static const qint64 WIDTH    = 6;
    static const qint64 SPACING  = 4;
    static const qint64 RMARGIN  = 40;
    static const qint64 TIMEOUT  = 250;

    struct candle_data {
        qreal l;
        qreal o;
        qreal c;
        qreal h;
    };

    static const qint64 shifters[] = { 1, 2, 2, 2, 3, 6, 6, 5, 5, 7, 4, 4, 4, 4, 3, 2 };

    struct candle_data candlerand()
    {
        qint64 a = 800;
        qint64 b = 1400;

        static qint64 i = 0;
        qreal s = (shifters[i++] * 100) / 100.0;
        i %= sizeof(shifters) / sizeof(qint64);

        qreal v[4] = {
            (-s + (a + qrand() % (b - a)) / 100.0),
            (-s + (a + qrand() % (b - a)) / 100.0),
            (-s + (a + qrand() % (b - a)) / 100.0),
            (-s + (a + qrand() % (b - a)) / 100.0),
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

static bool candlestickSetComparator(const QCandlestickSet *a, const QCandlestickSet *b)
{
    return a->timestamp() < b->timestamp();
}

static qint64 column_width()
{
    return Defaults::WIDTH + Defaults::SPACING;
}

ExChart::ExChart()
    : scrollFit_(false)
{
    setMinimumSize(280, 500);
    setInteractive(true);
//    setDragMode(QGraphicsView::ScrollHandDrag);
//    QCursor c = cursor();
//    setCursor(c);

    QCandlestickSeries *series = new QCandlestickSeries();
//    series->setName("Candles");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    series->setBodyOutlineVisible(false);
    series->setMaximumColumnWidth(Defaults::WIDTH);
    series->setMinimumColumnWidth(Defaults::WIDTH);


    for (size_t i = 0; i < 256; i++)
    {
        QDateTime now = QDateTime(QDateTime::currentDateTimeUtc().date())/*.addDays(-1)*/;
        QDateTime dt = now.addDays(-i);
        struct Defaults::candle_data d = Defaults::candlerand();

        QCandlestickSet *cs = new QCandlestickSet(d.o, d.h, d.l, d.c, dt.toMSecsSinceEpoch());

        series->append(cs);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Data");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
    axisX->setLabelsAngle(90);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 15);
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

QMap<QString, QDateTime> ExChart::dataRange()
{
    QCandlestickSeries *series = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    QList<QCandlestickSet *> sets = series->sets();
    QCandlestickSet *min = *std::min_element(sets.begin(), sets.end(), candlestickSetComparator);
    QCandlestickSet *max = *std::max_element(sets.begin(), sets.end(), candlestickSetComparator);
    QDateTime minD = QDateTime::fromMSecsSinceEpoch(min->timestamp());
    QDateTime maxD = QDateTime::fromMSecsSinceEpoch(max->timestamp());

    QMap<QString, QDateTime> map;
    map["oldest"]   = minD;
    map["youngest"] = maxD;

    return map;
}

QDateTime ExChart::oldestData()
{
    QMap<QString, QDateTime> map = dataRange();
    return map["oldest"];
}

QDateTime ExChart::youngestData()
{
    QMap<QString, QDateTime> map = dataRange();
    return map["youngest"];
}

void ExChart::scrollWithPixels(QPoint pixels)
{
    TRACE("Unexpected event!") << pixels;
}

void ExChart::scrollWithDegrees(QPoint steps)
{
    // Typicaly called by QWheelEvent.
    // Use steps.y() to scroll by time on axisX
    // Ignore steps.x()
    if (steps.y() == 0)
        return;

    int step = steps.y();
//    TRACE("") << step;

    QMap<QString, QDateTime> map = dataRange();

    QDateTime minD = map["oldest"];
    QDateTime maxD = map["youngest"];
//    TRACE("") << set_count << minD << maxD;

    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    QDateTime minX = axisX->min().addDays(step);
    QDateTime maxX = axisX->max().addDays(step);

    if (scrollFit())
    {
        // Side mergin is 2 ticks
        if (minX < minD.addDays(-2))
            return;
        if (maxX > maxD.addDays(2))
            return;
    }
    else
    {
        // Side mergin is size - 3 ticks
        if (minX > maxD.addDays(-3))
            return;
        if (maxX < minD.addDays(3))
            return;
    }

    axisX->setRange(minX, maxX);
//    TRACE("") << axisX->min() << "<->" << axisX->max();
}


////////////////////////////////////////////////////////////////////////////////
/// Exvents

void ExChart::resizeEvent(QResizeEvent *event)
{
    QDateTime maxD = youngestData();

    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    axisX->setTickCount((width() / column_width()));
    axisX->setRange(maxD.addDays(-(axisX->tickCount() + 1)), maxD);

    QChartView::resizeEvent(event);
}

void ExChart::mousePressEvent(QMouseEvent *event)
{
//    TRACE("") << event;
//    if (event->buttons() & Qt::LeftButton)
//    {
//        TRACE("move");
//    }
    QChartView::mousePressEvent(event);
}

void ExChart::mouseMoveEvent(QMouseEvent *event)
{
//    TRACE("") << event;
//    if (event->buttons() & Qt::LeftButton)
//    {
//        TRACE("move");
//    }
    QChartView::mouseMoveEvent(event);
}

void ExChart::mouseReleaseEvent(QMouseEvent *event)
{
//    TRACE("") << event;
//    if (event->buttons() & Qt::LeftButton)
//    {
//        TRACE("move");
//    }
    QChartView::mouseReleaseEvent(event);
}

void ExChart::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
        axisX->setMinorGridLineVisible(!axisX->isMinorGridLineVisible());
        axisX->setGridLineVisible(!axisX->isGridLineVisible());
        event->accept();
    }
    else
        QChartView::mouseDoubleClickEvent(event);
}

void ExChart::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta();

    // QT generates 120, 240 ... for mouse
    // but 1, 2, 5, 30 ... for touchpad
    // It is possible for touchpad to generate values > 120
    // but we ignore this here.
    // Normalize.
    if (abs(numDegrees.x()) >= 120 || abs(numDegrees.y()) >= 120)
        numDegrees /= 8;

    if (!numPixels.isNull()) {
        scrollWithPixels(numPixels);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        scrollWithDegrees(numSteps);
    }

    event->accept();
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void ExChart::onHover(bool status, QCandlestickSet *set)
{
    TRACE("") << status << QDateTime::fromMSecsSinceEpoch(set->timestamp(), Qt::UTC) << set->open() << set->low() << set->high() << set->close();
}

////////////////////////////////////////////////////////////////////////////////
/// Helpers

bool ExChart::scrollFit()
{
    return scrollFit_;
}

void ExChart::setScrollFit(bool fit)
{
    scrollFit_ = fit;
    resize(size());
}
