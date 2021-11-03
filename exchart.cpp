#include <iostream>
#include <algorithm>

#include "exchart.h"

#include <QTimer>
#include <QStringList>
#include <QDate>
#include <QDateTime>

#include <QIcon>
#include <QChart>
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QHCandlestickModelMapper>
#include <QFont>
#include <QPoint>
//#include <QCursor>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>

#include <QGraphicsView>

#include "logger.h"
#include "exchange/exchangerequest.h"

using namespace QtCharts;

namespace Defaults {

    static const qint64 shifters[] = { 1, 2, 2, 2, 3, 6, 6, 5, 5, 7, 4, 4, 4, 4, 3, 2 };

    struct crex::data::candle_data candlerand()
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

        struct crex::data::candle_data d = { v[0], v[1], v[2], v[3], 0 };

        return d;
    }
}

// This MUST correspond to TimeFrame enum
static const Defaults::TimeFrameType timeFrameSeconds[] = {
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

static QMap<Defaults::TimeFrame, QString> timeFrameToStr = {
    {Defaults::TF_1m,    "1m"},
    {Defaults::TF_3m,    "3m"},
    {Defaults::TF_5m,    "5m"},
    {Defaults::TF_15m,   "15m"},
    {Defaults::TF_30m,   "30m"},
    {Defaults::TF_1h,    "1h"},
    {Defaults::TF_2h,    "2h"},
    {Defaults::TF_4h,    "4h"},
    {Defaults::TF_6h,    "6h"},
    {Defaults::TF_8h,    "8h"},
    {Defaults::TF_12h,   "12h"},
    {Defaults::TF_1d,    "1d"},
    {Defaults::TF_3d,    "3d"},
    {Defaults::TF_1w,    "1w"},
    {Defaults::TF_1M,    "1M"},
};

#if 0
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
#endif

static bool candlestickTimeComparator(const struct crex::data::candle_data &a, const struct crex::data::candle_data &b)
{
    return a.t < b.t;
}

static bool candlestickHighComparator(const struct crex::data::candle_data &a, const struct crex::data::candle_data &b)
{
    return a.h < b.h;
}

static bool candlestickLowComparator(const struct crex::data::candle_data &a, const struct crex::data::candle_data &b)
{
    return a.l < b.l;
}

static int column_width()
{
    return Defaults::WIDTH + Defaults::SPACING;
}

////////////////////////////////////////////////////////////////////////////////
/// model

ExModel::ExModel(QObject *parent) : QAbstractTableModel(parent)
{
    candle_data_.clear();
//    for (size_t i = 0; i < 256; i++)
//    {
//        qint64 timeFrame_ = 3600;
//        QDateTime now = QDateTime(QDateTime::currentDateTimeUtc());
//        QDateTime dt = now.addSecs(-i * timeFrame_);
//        struct crex::data::candle_data d = Defaults::candlerand();
//        d.t = dt.toMSecsSinceEpoch();

//        candle_data_.append(d);
//    }
}

int ExModel::rowCount(const QModelIndex &/*parent*/) const
{
    return candle_data_.count();
}

int ExModel::columnCount(const QModelIndex &/*parent*/) const
{
    return Defaults::MAX_COLUMN;
}

QVariant ExModel::data(const QModelIndex &index, int role) const
{
//    TRACE("") << index;
    if ( !( (role == Qt::DisplayRole) || (role == Qt::EditRole) ) )
        return QVariant();
    if (!index.isValid())
        return QVariant();
    if (index.column() > Defaults::MAX_COLUMN)
    {
        TRACE("") << "ERROR column" << index.column();
        return QVariant();
    }

    struct crex::data::candle_data d = candle_data_.at(index.row());
    switch (index.column()) {
        case Defaults::OPEN_COL:
            return d.o;
        case Defaults::HIGH_COL:
            return d.h;
        case Defaults::LOW_COL:
            return d.l;
        case Defaults::CLOSE_COL:
            return d.c;
        case Defaults::TIME_COL:
            return d.t;
    }
    TRACE("") << "ERROR God sake here we are!!!";
    return QVariant();
}

bool ExModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() >= Defaults::MAX_COLUMN)
        {
            TRACE("") << "ERROR column" << index.column();
            return false;
        }
        int r = index.row();
        switch (index.column()) {
            case Defaults::OPEN_COL:
                candle_data_[r].o = value.toDouble();
            break;
            case Defaults::HIGH_COL:
                candle_data_[r].h = value.toDouble();
            break;
            case Defaults::LOW_COL:
                candle_data_[r].l = value.toDouble();
            break;
            case Defaults::CLOSE_COL:
                candle_data_[r].c = value.toDouble();
            break;
            case Defaults::TIME_COL:
                candle_data_[r].t = value.toDouble();
            break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void ExModel::setCandles(QList<crex::data::candle_data> &candles)
{
    if (!candles.count())
        return;

    beginRemoveRows(QModelIndex(), 0, candle_data_.count() - 1);
    candle_data_.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, candles.count() - 1);
    candle_data_.append(candles);
    endInsertRows();
}

QVariant ExModel::headerData(int section, Qt::Orientation orientation, int role) const
{
//    TRACE("") << section << role;
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (section >= Defaults::MAX_COLUMN)
    {
        TRACE("") << "ERROR column" << section;
        return QVariant();
    }

    switch (section) {
    case Defaults::OPEN_COL:
        return "Open";
    case Defaults::HIGH_COL:
        return "High";
    case Defaults::LOW_COL:
        return "Low";
    case Defaults::CLOSE_COL:
        return "Close";
    case Defaults::TIME_COL:
        return "DateTime";
    }
    TRACE("") << "ERROR God sake here we are!!!";
    return QVariant();
}

Qt::ItemFlags ExModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QMap<QString, QDateTime> ExModel::dataRange()
{
    if (candle_data_.count() < 1)
    {
        QMap<QString, QDateTime> map;
        map["oldest"]   = QDateTime::fromSecsSinceEpoch(0);
        map["youngest"] = QDateTime::fromSecsSinceEpoch(0);
        return map;
    }
    QList<crex::data::candle_data>::iterator min = std::min_element(candle_data_.begin(), candle_data_.end(), candlestickTimeComparator);
    QList<crex::data::candle_data>::iterator max = std::max_element(candle_data_.begin(), candle_data_.end(), candlestickTimeComparator);
    QDateTime minD = QDateTime::fromMSecsSinceEpoch(min->t);
    QDateTime maxD = QDateTime::fromMSecsSinceEpoch(max->t);

    QMap<QString, QDateTime> map;
    map["oldest"]   = minD;
    map["youngest"] = maxD;

    return map;
}

qreal ExModel::minLowValue()
{
    if (candle_data_.count() < 1)
        return 0;
    QList<crex::data::candle_data>::iterator min = std::min_element(candle_data_.begin(), candle_data_.end(), candlestickLowComparator);
    return min->l;
}

qreal ExModel::maxHighValue()
{
    if (candle_data_.count() < 1)
        return 0;
    QList<crex::data::candle_data>::iterator max = std::max_element(candle_data_.begin(), candle_data_.end(), candlestickHighComparator);
    return max->h;
}

////////////////////////////////////////////////////////////////////////////////
/// ChartView

ExChart::ExChart(ExchangeProtocol *protocol, const QString symbol, QWidget *parent)
    : QChartView(parent)
    , scrollFit_(false)
    , timeFrame_(3600)
    , request_(nullptr)
    , protocol_(protocol)
    , valid_(false)
    , autoScale_(true)
    , timeAxisMin_(QDateTime::fromSecsSinceEpoch(0))
    , timeAxisMax_(QDateTime::fromSecsSinceEpoch(0))
    , valAxisMin_(0.0)
    , valAxisMax_(0.0)
    , model_(new ExModel(this))
    , mapper_(new QHCandlestickModelMapper(this))
{
    setWindowIcon(QIcon::fromTheme("graphics"));

    setSymbol(std::move(symbol));

//    setMinimumSize(280, 500);
    setInteractive(true);
//    setDragMode(QGraphicsView::ScrollHandDrag);
//    cursor().setShape(Qt::CrossCursor);
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

    mapper_->setSeries(series);
    mapper_->setModel(model_);

    mapper_->setFirstSetRow(0);
    mapper_->setLastSetRow(model_->rowCount() - 1);
    mapper_->setOpenColumn(Defaults::OPEN_COL);
    mapper_->setCloseColumn(Defaults::CLOSE_COL);
    mapper_->setHighColumn(Defaults::HIGH_COL);
    mapper_->setLowColumn(Defaults::LOW_COL);
    mapper_->setTimestampColumn(Defaults::TIME_COL);

    QChart *chart = new QChart();
    chart->cursor().setShape(Qt::CrossCursor);
    chart->addSeries(series);
//    chart->setTitle("Data");

    QDateTimeAxis *axisX = new QDateTimeAxis();
//    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
    axisX->setFormat("");
    axisX->setLabelsVisible(false);
    axisX->setVisible(false);
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

    timeLabel_ = new QGraphicsSimpleTextItem(chart);
    timeLabel_->setText("0000-00-00 00:00");
    timeLabel_->setVisible(false);

    timeLine_ = new QGraphicsLineItem(chart);
    QPen p(timeLine_->pen());
    p.setStyle(Qt::DotLine);
    timeLine_->setPen(p);
    timeLine_->setVisible(false);

//    std::cerr << QLocale::system().name().toStdString() << std::endl;
//    std::cerr << QLocale().name().toStdString() << std::endl;

    QTimer::singleShot(1000, this, &ExChart::onTimer);
}

QAbstractTableModel *ExChart::model()
{
    return model_;
}

QMap<QString, QDateTime> ExChart::dataRange()
{
    return model_->dataRange();
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
    return model_->minLowValue();
}

qreal ExChart::maxHighValue()
{
    return model_->maxHighValue();
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

    setAutoScale(false);

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
        if (minX < minD.addSecs(-(Defaults::SCROLL_X_MARGIN) * timeFrame_))
            return;
        if (maxX > maxD.addSecs((Defaults::SCROLL_X_MARGIN) * timeFrame_))
            return;
    }
    else
    {
        // Side mergin is size - 3 ticks
        if (minX > maxD.addSecs(-(Defaults::SCROLL_X_MARGIN + 1) * timeFrame_))
            return;
        if (maxX < minD.addSecs((Defaults::SCROLL_X_MARGIN + 1) * timeFrame_))
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

    setAutoScale(false);

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
//    QDateTime maxD = youngestData();
//    QRectF pa = chart()->plotArea();
//    int w = pa.width();
////    TRACE("") << size() << event->size() << event->oldSize() << pa;
//    int ticks = w / column_width();

//    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
//    axisX->setTickCount((ticks));
//    axisX->setRange(maxD.addSecs(-(axisX->tickCount()) * timeFrame_), maxD);

    scaleData();

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

    updateTimeLabel(event->localPos());

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

void ExChart::setSymbol(const QString symbol)
{
    symbol_.clear();
    symbol_ = std::move(symbol);

    if (symbol_.isEmpty())
        setWindowTitle("?");
    else
        setWindowTitle(symbol_);
}

void ExChart::onHover(bool status, QCandlestickSet *set)
{
    TRACE("") << status << QDateTime::fromMSecsSinceEpoch(set->timestamp(), Qt::UTC) << set->open() << set->low() << set->high() << set->close();
}

void ExChart::onTimer()
{
    if (request_)
        return;

    if (symbol_.isEmpty())
        TRACE("empty");

    request_ = protocol_->requestExchangeCandledata(symbol_, "1h");
    connect(request_, &ExchangeRequest::dataReady, this, &ExChart::onCandleDataReady);
}

void ExChart::onCandleDataReady()
{
    if (request_)
    {
        QByteArray json_data(request_->data());
        request_->deleteLater();
        request_ = nullptr;

        parseJSON(json_data);
    }

    QTimer::singleShot(550, this, &ExChart::onTimer);
}

////////////////////////////////////////////////////////////////////////////////
/// JSON Parser

void ExChart::parseJSON(QByteArray &json_data)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(json_data, &jsonError);
    if (doc.isNull())
    {
        TRACE("") << "JSON error:" << jsonError.errorString();
        return;
    }

//    static int q = 0;
//    dumpToFile(QString("../candle-dump-%1.json").arg(q++), doc);

    if (!doc.isArray())
    {
        TRACE("JSON is not array!!!");
        return;
    }

    QList<crex::data::candle_data> candles;
    for (int i = 0; i < doc.array().count(); i++)
    {
        if (!doc[i].isArray())
        {
            TRACE("JSON candle") << i << "is not array!!!";
            continue;
        }
        candles.append(std::move(parseJSONCandle(doc[i].toArray())));
    }
    QCandlestickSeries *s = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));
    QList<QCandlestickSet *> sets = s->sets();
    model_->setCandles(candles);

    scaleData();
}

crex::data::candle_data ExChart::parseJSONCandle(const QJsonArray &json)
{
//    TRACE("") << json[i];
    struct crex::data::candle_data d;
    d.o = json[1].toString().toDouble();
    d.c = json[4].toString().toDouble();
    d.h = json[2].toString().toDouble();
    d.l = json[3].toString().toDouble();
    d.t = json[0].toDouble();
    return d;
}

void ExChart::scaleData()
{
    mapper_->setFirstSetRow(0);
    mapper_->setLastSetRow(model_->rowCount() - 1);

    scaleDataX();
    scaleDataY();
}

void ExChart::scaleDataX()
{
    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axes(Qt::Horizontal).at(0));
    QDateTime maxD = autoScale_ ? youngestData() : axisX->max();

    QRectF pa = chart()->plotArea();
    int w = pa.width();
    int ticks = w / column_width();
    QDateTime minD = maxD.addSecs(-ticks * timeFrame_);

    axisX->setTickCount((ticks));
    axisX->setRange(minD, maxD);
}

void ExChart::scaleDataY()
{
    if (autoScale_)
    {
        QValueAxis *axisY = qobject_cast<QValueAxis *>(chart()->axes(Qt::Vertical).at(0));
        qreal minP = minLowValue();
        qreal maxP = maxHighValue();
        minP *= Defaults::MIN_P;
        maxP *= Defaults::MAX_P;
        axisY->setRange(minP, maxP);
    }
}

void ExChart::setAutoScale(bool ascale)
{
    autoScale_ = ascale;
    scaleData();
}

void ExChart::updateTimeLabel(QPointF localPos)
{
    auto const widgetPos = localPos;
//    TRACE("") << "widgetPos:" << widgetPos;
    bool vis = chart()->plotArea().contains(widgetPos);
    timeLabel_->setVisible(vis);
    timeLine_->setVisible(vis);

    if (vis)
    {
        QCandlestickSeries *series = qobject_cast<QCandlestickSeries *>(chart()->series().at(0));

        auto const scenePos = mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
        auto const chartItemPos = chart()->mapFromScene(scenePos);
        QPoint chartRoundPos = chartItemPos.toPoint();
        chartRoundPos /= column_width();
        chartRoundPos *=column_width();
        QPointF const value = chart()->mapToValue(/*chartItemPos*/chartRoundPos, series);
//        TRACE("") << "scenePos:" << scenePos;
//        TRACE("") << "chartItemPos:" << chartItemPos;
//        TRACE("") << "valSeries:" << QDateTime::fromMSecsSinceEpoch(value.x()).toString("yyyy-MM-dd hh:mm:ss");

//        QList<QCandlestickSet *> sets = series->sets();
//        QPointF timeLinePos = chart()->mapToPosition(value, series);
//        TRACE("") << timeLinePos;

        timeLabel_->setText(QDateTime::fromMSecsSinceEpoch(value.x()).toString("yyyy-MM-dd hh:mm"));
        timeLabel_->setPos(widgetPos.x(), chart()->plotArea().height() + chart()->margins().top() + QFontMetrics(timeLabel_->font()).lineSpacing());

//        timeLine_->setLine(widgetPos.x(), chart()->plotArea().top(), widgetPos.x(), chart()->plotArea().top() + chart()->plotArea().height());
        timeLine_->setLine(chartRoundPos.x() - column_width() / 2, chart()->plotArea().top(), chartRoundPos.x() - column_width() / 2, chart()->plotArea().top() + chart()->plotArea().height());
    }
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
    for (size_t i = 0; i < (sizeof(timeFrameSeconds) / sizeof(Defaults::TimeFrameType)); i++)
    {
        if (t == timeFrameSeconds[i])
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
