#include "widgets/exchart.h"

#include <qcustomplot.h>

#include <QSharedPointer>
#include <QCursor>
#include <QIcon>
#include <QTimer>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QString>

#include "logger.h"

namespace crex::chart {

namespace {
    QStringList TimeFrames = {
        "1m",
        "3m",
        "5m",
        "15m",
        "30m",
        "1h",
        "2h",
        "4h",
        "6h",
        "8h",
        "12h",
        "1d",
        "3d",
        "1w",
        "1M"
    };
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::ExChart
/// \param parent
///
ExChart::ExChart(ExchangeProtocol *protocol, const QString symbol, QWidget *parent):
    QCustomPlot(parent)
  , hCursorLine(new QCPItemLine(this))
  , vCursorLine(new QCPItemLine(this))
  , dataInitialized(false)
  , autoScaleY(true)
  , request_(nullptr)
  , protocol_(protocol)
  , timeFrame(TimeFrames[0])
  , tfCombo(new QComboBox(this))
  , olhcDisplay(new QLabel("OLHC", this))
{
    setWindowIcon(QIcon::fromTheme("graphics"));
    setSymbol(std::move(symbol));
    setMinimumSize(640, 480);

    yAxis->setVisible(false);
    yAxis2->setVisible(true);
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));

    cursorLines.append(hCursorLine);
    cursorLines.append(vCursorLine);
    for (int i = 0; i < cursorLines.size(); i++)
    {
        cursorLines.at(i)->start->setType(QCPItemPosition::ptAbsolute);
        cursorLines.at(i)->end->setType(QCPItemPosition::ptAbsolute);
        cursorLines.at(i)->setPen(QPen(Qt::DotLine));
    }

    financial = new QCPFinancial(xAxis, yAxis2);
    financial->setChartStyle(QCPFinancial::csCandlestick);
    financial->setWidthType(QCPFinancial::wtAbsolute);
//    financial->setWidthType(QCPFinancial::wtAxisRectRatio);
//    financial->setWidthType(QCPFinancial::wtPlotCoords);
    financial->setWidth(5);
    xAxis->setRange(0, 0);

    QSharedPointer<QCPAxisTickerDateTime> xTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xAxis->setTicker(xTicker);
    setCursor(QCursor(Qt::CrossCursor));

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    createTimeFrameButton();

    QTimer::singleShot(100, this, &ExChart::onTimer);
}

void ExChart::setSymbol(QString symbol)
{
    symbol_.clear();
    symbol_ = std::move(symbol);

    if (symbol_.isEmpty())
        setWindowTitle("?");
    else
        setWindowTitle(symbol_);
}

void ExChart::setCandles(QSharedPointer<QCPFinancialDataContainer> cont)
{
    auto rmStart = cont->begin()->key;
    auto it = financial->data()->end();
    it--;
    auto rmEnd = it->key;
    if (rmStart < rmEnd)
    {
        financial->data()->removeAfter(rmStart);
    }
    else if (rmStart == rmEnd)
    {
        financial->data()->remove(rmStart);
    }
    financial->data()->add(*cont);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::resizeEvent
/// \param event
///
void ExChart::resizeEvent(QResizeEvent *event)
{
    QCustomPlot::resizeEvent(event);

    if (financial->widthType() != QCPFinancial::wtAbsolute)
        return;

//    qDebug() << event << viewport() << axisRect(0)->size() << axisRect(0)->topLeft() << axisRect(0)->bottomRight() << axisRectCount() << graphCount();
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::mouseMoveEvent
/// \param event
///
void ExChart::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    QRect bounds = QRect(axisRect(0)->topLeft(), axisRect(0)->bottomRight());
    if (event->x() >= bounds.topLeft().x() && event->x() < bounds.bottomRight().x())
    {
        vCursorLine->start->setCoords(event->x(), bounds.topLeft().y());
        vCursorLine->end->setCoords(event->x(), bounds.bottomRight().y());
    }
    if (event->y() >= bounds.topLeft().y() && event->y() < bounds.bottomRight().y())
    {
        hCursorLine->start->setCoords(bounds.topLeft().x(), event->y());
        hCursorLine->end->setCoords(bounds.bottomRight().x(), event->y());
    }
    replot();

    const auto d = financial->data()->findBegin(xAxis->pixelToCoord(event->x()), true);
    QString olhc(QString("%1 O:%2 H:%3 L:%4 C:%5")
                 .arg(QDateTime::fromMSecsSinceEpoch(d->key).toString("yyyy-MM-dd hh:mm"))
                 .arg(d->open)
                 .arg(d->high)
                 .arg(d->low)
                 .arg(d->close)
            );

    placeOlhcLabel(olhc);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::createTimeFrameButton
///
void ExChart::createTimeFrameButton()
{
    tfCombo->setFocusPolicy(Qt::NoFocus);
    tfCombo->move(4, 4);
    tfCombo->addItems(TimeFrames);
    connect(tfCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, QOverload<int>::of(&ExChart::switchTF));

    olhcDisplay->move(tfCombo->pos().x() + tfCombo->size().width() + 4, 4);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::placeOlhcLabel
///
void ExChart::placeOlhcLabel(const QString &s)
{
    olhcDisplay->setText(s);
    QFontMetrics fm(olhcDisplay->font());
    int w = fm.width(s);
    int h = fm.height();
    QSize sz(w, h);
//    sz += QSize(4, 2);
    olhcDisplay->resize(sz);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::switchTF
///
void ExChart::switchTF(int index)
{
    timeFrame = TimeFrames.at(index);
    financial->data()->clear();
}

int ExChart::visibleCandlesCount()
{
    const int N = axisRect(0)->size().width() / (financial->width() + 2) - 1;
    return N;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::scaleData
///
void ExChart::scaleData()
{
    scaleDataX();
    scaleDataY();
}

void ExChart::scaleDataX()
{
    if (dataInitialized)
        return;

    const int N = visibleCandlesCount();

    auto it = financial->data()->end();
    it--;
    auto tmax = it->key;
    it -= N;
    auto tmin = it->key;

    xAxis->setRange(tmin, tmax);
}

void ExChart::scaleDataY()
{
    if (!autoScaleY)
        return;

    const int N = visibleCandlesCount();
    auto end = financial->data()->findBegin(xAxis->pixelToCoord(axisRect(0)->bottomRight().x()), true);
    auto begin = financial->data()->findBegin(xAxis->pixelToCoord(axisRect(0)->topLeft().x()), true);

    auto itmin = std::min_element(begin, end, [](const auto &a, const auto &b) { return a.low < b.low; });
    auto itmax = std::max_element(begin, end, [](const auto &a, const auto &b) { return a.high < b.high; });

    auto dmin = itmin->low;
    auto dmax = itmax->high;
    auto r = dmax - dmin;
    dmin -= r / 10;
    dmax += r / 10;

    yAxis->setRange(dmin, dmax);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::onTimer
///
void ExChart::onTimer()
{
    if (request_)
        return;

    if (symbol_.isEmpty())
        TRACE("empty");

    qlonglong startTime = 0;
    if (financial->data()->size() > 0)
    {
        auto it = financial->data()->end();
        it--;
        startTime = it->key;
    }
    request_ = protocol_->requestExchangeCandledata(symbol_, timeFrame, startTime);
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
        dataInitialized = true;
    }

    QTimer::singleShot(550, this, &ExChart::onTimer);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief ExQChart::parseJSON
/// \param json_data
///
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

//     static int q = 0;
//     crex::data::dumpToFile(QString("../candle-dump-%1.json").arg(q++), doc);

    if (!doc.isArray())
    {
        TRACE("JSON is not array!!!");
        return;
    }

    QSharedPointer<QCPFinancialDataContainer> cont;
    cont = QSharedPointer<QCPFinancialDataContainer>(new QCPFinancialDataContainer);

    for (int i = 0; i < doc.array().count(); i++)
    {
        if (!doc[i].isArray())
        {
            TRACE("JSON candle") << i << "is not array!!!";
            continue;
        }
        QCPFinancialData fd = parseJSONCandle(doc[i].toArray());
        cont->add(std::move(fd));
    }

    setCandles(cont);
    replot();

    scaleData();
}

QCPFinancialData ExChart::parseJSONCandle(const QJsonArray &arr)
{
    QCPFinancialData fd(arr[0].toDouble(),
                        arr[1].toString().toDouble(),
                        arr[2].toString().toDouble(),
                        arr[3].toString().toDouble(),
                        arr[4].toString().toDouble()
            );
    return fd;
}

} // namespace crex::chart

