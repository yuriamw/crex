#include "widgets/exchart.h"

#include <qcustomplot.h>

#include <QSharedPointer>
#include <QCursor>
#include <QIcon>
#include <QTimer>
#include <QByteArray>

#include "logger.h"

namespace crex::chart {

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::ExChart
/// \param parent
///
ExChart::ExChart(ExchangeProtocol *protocol, const QString symbol, QWidget *parent):
    QCustomPlot(parent)
  , hCursorLine(new QCPItemLine(this))
  , vCursorLine(new QCPItemLine(this))
  , request_(nullptr)
  , protocol_(protocol)
  , timeFrame("1H")
  , tfButton(new QToolButton(this))
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
    financial->setWidth(5);

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
    financial->setData(cont);
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
                 .arg(QDateTime::fromMSecsSinceEpoch(d->key * 1000).toString("yyyy-MM-dd hh:mm"))
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
    tfButton->move(4, 4);

    placeTimeFrameButton();

    connect(tfButton, &QToolButton::clicked, this, &ExChart::switchTF);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::placeTimeFrameButton
///
void ExChart::placeTimeFrameButton()
{
    tfButton->setText(timeFrame);

    QFontMetrics fm(tfButton->font());
    int w = fm.width(timeFrame);
    int h = fm.height();
    QSize sz(w, h);
    sz += QSize(4, 2);

    tfButton->resize(sz);
    tfButton->setFocusPolicy(Qt::NoFocus);

    olhcDisplay->move(tfButton->pos().x() + tfButton->size().width() + 4, 4);
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
void ExChart::switchTF()
{
    static QString timeFrames[] = { "1m", "5m", "15m", "30m", "1H", "4H", "1D", "1W", "1M", QString()};

    for (int i = 0; !timeFrames[i].isNull(); i++)
    {
        if (timeFrames[i] == timeFrame)
        {
            i++; if (timeFrames[i] == "") i = 0;
            timeFrame = timeFrames[i];
            break;
        }
    }
    placeTimeFrameButton();
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::scaleData
///
void ExChart::scaleData(qreal tmin, qreal tmax, qreal dmin, qreal dmax)
{
    qreal r = dmax - dmin;
    dmin -= r / 10;
    dmax += r / 10;

    r = tmax - tmin;
    tmin -= r / 10;
    tmax += r / 10;

    xAxis->setRange(tmin, tmax);

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

    qreal dmin = std::numeric_limits<qreal>::max();
    qreal dmax = -1.0;
    qreal tmin = std::numeric_limits<qreal>::max();
    qreal tmax = -1.0;
    for (int i = 0; i < doc.array().count(); i++)
    {
        if (!doc[i].isArray())
        {
            TRACE("JSON candle") << i << "is not array!!!";
            continue;
        }
        QCPFinancialData fd = parseJSONCandle(doc[i].toArray());
        cont->add(std::move(fd));

        if (fd.low < dmin) dmin = fd.low;
        if (fd.high > dmax) dmax = fd.high;

        if (fd.key < tmin) tmin = fd.key;
        if (fd.key > tmax) tmax = fd.key;
    }

    setCandles(cont);

    scaleData(tmin, tmax, dmin, dmax);
}

QCPFinancialData ExChart::parseJSONCandle(const QJsonArray &arr)
{
    //    TRACE("") << json[i];
    QCPFinancialData fd(arr[0].toDouble() / 1000.0,
                        arr[1].toString().toDouble(),
                        arr[2].toString().toDouble(),
                        arr[3].toString().toDouble(),
                        arr[4].toString().toDouble()
            );
    return fd;
}

} // namespace crex::chart

