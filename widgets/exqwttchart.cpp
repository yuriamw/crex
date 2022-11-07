#include "exqwttchart.h"

#include <QwtDate>
#include <QwtDateScaleEngine>
#include <QwtDateScaleDraw>
#include <QwtPlotTradingCurve>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtText>

#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include <QDateTime>
#include <QColor>
#include <QPen>

#include "logger.h"

namespace crex::chart {

class Zoomer : public QwtPlotZoomer
{
  public:
    Zoomer( QWidget* canvas )
        : QwtPlotZoomer( canvas )
    {
        setRubberBandPen( QColor( Qt::darkGreen ) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
    }

  protected:
    virtual QwtText trackerTextF( const QPointF& pos ) const QWT_OVERRIDE
    {
        const QDateTime dt = QwtDate::toDateTime( pos.x() );

        QString s;
        s += QwtDate::toString( QwtDate::toDateTime( pos.x() ),
            "MMM dd hh:mm ", QwtDate::FirstThursday );

        QwtText text( s );
        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
};

class DateScaleDraw : public QwtDateScaleDraw
{
  public:
    DateScaleDraw( Qt::TimeSpec timeSpec )
        : QwtDateScaleDraw( timeSpec )
    {
        // as we have dates from 2010 only we use
        // format strings without the year

        setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz\nddd dd MMM" );
        setDateFormat( QwtDate::Second, "hh:mm:ss\nddd dd MMM" );
        setDateFormat( QwtDate::Minute, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Hour, "hh:mm\nddd dd MMM" );
        setDateFormat( QwtDate::Day, "ddd dd MMM" );
        setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
};

ExQwtTChart::ExQwtTChart(ExchangeProtocol *protocol, Core *core, const QString symbol, QWidget *parent):
    QwtPlot(parent)
    , request(nullptr)
    , protocol(protocol)
    , wssProtocol(core->exchangeWssProtocol())
    , symbol(symbol)
{
    setTitle(symbol);

    setAxisTitle(QwtAxis::XBottom, symbol);
    setAxisScaleDraw(QwtAxis::XBottom, new DateScaleDraw(Qt::UTC));
    setAxisScaleEngine(QwtAxis::XBottom, new QwtDateScaleEngine(Qt::UTC));

    setAxisVisible(QwtAxis::YLeft, false);
    setAxisVisible(QwtAxis::YRight, true);
//    setAxisTitle(QwtAxis::YRight, QString("USDT"));
//    setAxisTitle(QwtAxis::YLeft, QString("USDT"));

    createCurve();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

//    Zoomer* zoomer = new Zoomer( canvas() );
//    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
//        Qt::RightButton, Qt::ControlModifier );
//    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
//        Qt::RightButton );

//    QwtPlotPanner* panner = new QwtPlotPanner( canvas() );
//    panner->setMouseButton( Qt::MiddleButton );

    QTimer::singleShot(100, this, &ExQwtTChart::onTimer);
}

void ExQwtTChart::createCurve()
{
    curve = new QwtPlotTradingCurve();
    curve->setOrientation(Qt::Vertical);

    curve->setXAxis(QwtAxis::XBottom);
    curve->setYAxis(QwtAxis::YRight);

//    curve->setSymbolExtent(60 * 1000.0);
//    curve->setMinSymbolWidth(3);
//    curve->setMaxSymbolWidth(7);

    curve->setSymbolBrush(QwtPlotTradingCurve::Decreasing, QColor(Qt::red));
    curve->setSymbolBrush(QwtPlotTradingCurve::Increasing, QColor(Qt::green));

    curve->attach(this);

    curve->setVisible(true);
}

////////////////////////////////////////////////////////////////////////////
/// Slots
///

void ExQwtTChart::onTimer()
{
    if (request)
        return;

    if (symbol.isEmpty())
        TRACE("empty");

    qlonglong startTime = -1.0; // Use default value
//    auto dsize = curve->dataSize();
//    if (dsize > 0)
//    {
//        startTime = curve->sample(dsize - 1).time;
//    }

    QString timeFrame = QString("1m");

    request = protocol->requestExchangeCandledata(symbol, timeFrame, startTime);
    connect(request, &ExchangeRequest::dataReady, this, &ExQwtTChart::onCandleDataReady);
}

void ExQwtTChart::onCandleDataReady()
{
    if (request)
    {
        TRACE("");
        QByteArray json_data(request->data());
        request->deleteLater();
        request = nullptr;

        parseJSON(json_data);
//        dataInitialized = true;
    }

    QTimer::singleShot(550, this, &ExQwtTChart::onTimer);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief ExQwtChart::parseJSON
/// \param json_data
///
/// JSON Parser

void ExQwtTChart::parseJSON(QByteArray &json_data)
{
    TRACE("");

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

    QVector<QwtOHLCSample> samples;
    samples.reserve(doc.array().count());

    for (int i = 0; i < doc.array().count(); i++)
    {
        if (!doc[i].isArray())
        {
            TRACE("JSON candle") << i << "is not array!!!";
            continue;
        }
        QwtOHLCSample sample = parseJSONCandle(doc[i].toArray());
        samples += sample;
    }

    QwtSeriesData<QwtOHLCSample> *items = curve->data();
//    if (items->size() == 0)
//    {
        curve->setSamples(samples);
//    }
//    else
//    {
//        bool found = false;
//        size_t idx;
//        for (size_t i = items->size() - 1; i >= 0; i--)
//        {
//            if (QDateTime::fromMSecsSinceEpoch(items->sample(i).time) == QDateTime::fromMSecsSinceEpoch(samples[0].time))
//            {
//                found = true;
//                idx = i;
//                break;
//            }
//        }
//        if (found)
//        {
//            auto cdata = dynamic_cast<QwtTradingChartData*>(items);
//        }
//    }
    curve->setSymbolExtent(60 * 1000.0);

    replot();

//    showItem(curve, true);
}

QwtOHLCSample ExQwtTChart::parseJSONCandle(const QJsonArray &arr)
{
    QwtOHLCSample sample(QwtDate::toDouble(
                             QDateTime::fromMSecsSinceEpoch(arr[0].toDouble(), Qt::UTC)
                         ),
                        arr[1].toString().toDouble(),
                        arr[2].toString().toDouble(),
                        arr[3].toString().toDouble(),
                        arr[4].toString().toDouble()
            );
    return sample;
}

} // namespace crex::chart
