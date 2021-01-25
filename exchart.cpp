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
    static const qreal  MIN_P    = 0.95; // 5% lower
    static const qreal  MAX_P    = 1.05; // 5% higher

    typedef qint64 TimeFrameType;

    typedef enum {
        TF_1m = 0,
        TF_3m,
        TF_5m,
        TF_15m,
        TF_30m,
        TF_1h,
        TF_2h,
        TF_4h,
        TF_6h,
        TF_8h,
        TF_12h,
        TF_1d,
        TF_3d,
        TF_1w,
        TF_1M
    } TimeFrame;

    // This MUST correspond to TimeFrame enum
    static const TimeFrameType timeFrameSeconds[] = {
        1 * 60,             //    1m
        3 * 60,             //    3m
        5 * 60,             //    5m
        15 * 60,            //    15m
        30 * 60,            //    30m
        1 * 60 * 60,        //    1h
        2 * 60 * 60,        //    2h
        4 * 60 * 60,        //    4h
        6 * 60 * 60,        //    6h
        8 * 60 * 60,        //    8h
        12 * 60 * 60,       //    12h
        1 * 60 * 60 * 24,   //    1d
        3 * 60 * 60 * 24,   //    3d
        7 * 60 * 60 * 24,   //    1w
        30 * 60 * 60 * 24,  //    1M
    };

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

static bool candlestickSetTimeComparator(const QCandlestickSet *a, const QCandlestickSet *b)
{
    return a->timestamp() < b->timestamp();
}

static bool candlestickSetHighComparator(const QCandlestickSet *a, const QCandlestickSet *b)
{
    return a->high() < b->high();
}

static bool candlestickSetLowComparator(const QCandlestickSet *a, const QCandlestickSet *b)
{
    return a->low() < b->low();
}

static qint64 column_width()
{
    return Defaults::WIDTH + Defaults::SPACING;
}

ExChart::ExChart()
    : scrollFit_(false)
    , timeFrame_(3600*24)
{
//    setMinimumSize(280, 500);
    setInteractive(true);
//    setDragMode(QGraphicsView::ScrollHandDrag);
//    QCursor c = cursor();
//    setCursor(c);
    QSize si = sizeIncrement();
    si.setWidth(column_width());
    setSizeIncrement(si);

    QCandlestickSeries *series = new QCandlestickSeries();
//    series->setName("Candles");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    series->setBodyOutlineVisible(false);
    series->setCapsVisible(false);
    series->setMaximumColumnWidth(Defaults::WIDTH);
    series->setMinimumColumnWidth(Defaults::WIDTH);

    series->pen().setColor(QColor(Qt::magenta));

    for (size_t i = 0; i < 256; i++)
    {
        QDateTime now = QDateTime(QDateTime::currentDateTimeUtc().date())/*.addSecs(-1 * timeFrame_)*/;
        QDateTime dt = now.addSecs(-i * timeFrame_);
        struct Defaults::candle_data d = Defaults::candlerand();

        QCandlestickSet *cs = new QCandlestickSet(d.o, d.h, d.l, d.c, dt.toMSecsSinceEpoch());

        series->append(cs);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
//    chart->setTitle("Data");

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
//    axisX->setLabelsAngle(90);
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignRight);

    QFont labelFont;
    labelFont.setPixelSize(8);
    axisX->setLabelsFont(labelFont);
    axisY->setLabelsFont(labelFont);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    setChart(chart);
//    setRenderHint(QPainter::Antialiasing);
    qreal minP = minLowValue();
    qreal maxP = maxHighValue();
    minP *= Defaults::MIN_P;
    maxP *= Defaults::MAX_P;
    axisY->setRange(minP, maxP);

    connect(series, &QCandlestickSeries::hovered, this, &ExChart::onHover);

//    std::cerr << QLocale::system().name().toStdString() << std::endl;
//    std::cerr << QLocale().name().toStdString() << std::endl;
}

QMap<QString, QDateTime> ExChart::dataRange()
{
    QCandlestickSeries *series = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    QList<QCandlestickSet *> sets = series->sets();
    QCandlestickSet *min = *std::min_element(sets.begin(), sets.end(), candlestickSetTimeComparator);
    QCandlestickSet *max = *std::max_element(sets.begin(), sets.end(), candlestickSetTimeComparator);
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

qreal ExChart::minLowValue()
{
    QCandlestickSeries *series = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    QList<QCandlestickSet *> sets = series->sets();
    QCandlestickSet *set = *std::min_element(sets.begin(), sets.end(), candlestickSetLowComparator);
    return set->low();
}

qreal ExChart::maxHighValue()
{
    QCandlestickSeries *series = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    QList<QCandlestickSet *> sets = series->sets();
    QCandlestickSet *set = *std::max_element(sets.begin(), sets.end(), candlestickSetHighComparator);
    return set->high();
}

void ExChart::zoomWithPixels(QPoint pixels)
{
    TRACE("Unexpected event!") << pixels;
}

void ExChart::zoomWithDegrees(QPoint steps)
{
    TRACE("Not yet implemented!") << steps;
}

void ExChart::scrollHorizontal(QPoint steps)
{
    int step = steps.x();
    if (!step)
        return;

    QMap<QString, QDateTime> map = dataRange();

    QDateTime minD = map["oldest"];
    QDateTime maxD = map["youngest"];
//    TRACE("") << set_count << minD << maxD;

    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    QDateTime minX = axisX->min().addSecs(step * timeFrame_);
    QDateTime maxX = axisX->max().addSecs(step * timeFrame_);

    if (scrollFit())
    {
        // Side mergin is 2 ticks
        if (minX < minD.addSecs(-2 * timeFrame_))
            return;
        if (maxX > maxD.addSecs(2 * timeFrame_))
            return;
    }
    else
    {
        // Side mergin is size - 3 ticks
        if (minX > maxD.addSecs(-3 * timeFrame_))
            return;
        if (maxX < minD.addSecs(3 * timeFrame_))
            return;
    }

    axisX->setRange(minX, maxX);
//    TRACE("") << axisX->min() << "<->" << axisX->max();
}

void ExChart::scrollVertical(QPoint steps)
{
    qreal diffP = steps.y();
    if (!diffP)
        return;

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).at(0));
    qreal minY = axisY->min();
    qreal maxY = axisY->max();
    qreal diffY = maxY - minY;
    QRectF pa = chart()->plotArea();
    qreal pixels = pa.bottom() - pa.top();
    qreal shiftP = diffP / pixels;
    qreal shiftY = diffY * shiftP;
    minY += shiftY;
    maxY += shiftY;
    axisY->setRange(minY, maxY);
}

////////////////////////////////////////////////////////////////////////////////
/// Exvents

void ExChart::resizeEvent(QResizeEvent *event)
{
    QDateTime maxD = youngestData();
    QRectF pa = chart()->plotArea();
    int w = pa.width();
//    TRACE("") << size() << event->size() << event->oldSize() << pa;
    int ticks = w / column_width();

    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    axisX->setTickCount((ticks));
    axisX->setRange(maxD.addSecs(-(axisX->tickCount()) * timeFrame_), maxD);

    QChartView::resizeEvent(event);
}

void ExChart::mousePressEvent(QMouseEvent *event)
{
//    TRACE("") << event->buttons() << "pos:" << event->pos() << "local:" << event->localPos() << "window:" << event->windowPos() << "screen:" << event->screenPos();
    if (event->buttons() & Qt::LeftButton)
    {
        if (chart()->plotArea().contains(event->localPos()))
        {
            dragStart_ = event->pos();
            event->accept();
            return;
        }
    }
    QChartView::mousePressEvent(event);
}

void ExChart::mouseMoveEvent(QMouseEvent *event)
{
//    TRACE("") << event->buttons() << "pos:" << event->pos() << "local:" << event->localPos() << "window:" << event->windowPos() << "screen:" << event->screenPos();
    if (event->buttons() & Qt::LeftButton)
    {
        if (chart()->plotArea().contains(event->localPos()))
        {
            if (dragStart_.x())
            {
                int d = (dragStart_.x() - event->pos().x()) / column_width();
                if (d)
                {
                    scrollHorizontal(QPoint(d, 0));
                    dragStart_.setX(event->pos().x());
                }
            }
            if (dragStart_.y())
            {
                // 0 of axis is at bottom while 0 of widget is on top
                // invert direction here
                scrollVertical(QPoint(0, event->pos().y() - dragStart_.y()));
                dragStart_.setY(event->pos().y());
            }
            event->accept();
            return;
        }
    }
    QChartView::mouseMoveEvent(event);
}

void ExChart::mouseReleaseEvent(QMouseEvent *event)
{
//    TRACE("") << event->buttons() << "pos:" << event->pos() << "local:" << event->localPos() << "window:" << event->windowPos() << "screen:" << event->screenPos();
    if (!dragStart_.isNull())
    {
        dragStart_ = QPoint(0, 0);
        event->accept();
        return;
    }
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
        zoomWithPixels(numPixels);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        zoomWithDegrees(numSteps);
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

qint64 ExChart::timeFrame()
{
    return timeFrame_;
}

bool ExChart::setTimeFrame(qint64 t)
{
    bool found = false;
    for (size_t i = 0; i < (sizeof(Defaults::timeFrameSeconds) / sizeof(Defaults::TimeFrameType)); i++)
    {
        if (t == Defaults::timeFrameSeconds[i])
        {
            found = true;
            break;
        }
    }
    if (!found)
        return false;

    timeFrame_ = t;
    return true;
}
