#include "exqwttchart.h"

#include <QwtDate>
#include <QwtDateScaleEngine>
#include <QwtDateScaleDraw>
#include <QwtPlotTradingCurve>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtText>
#include <QwtLinearScaleEngine>
#include <QwtPlotMagnifier>
#include <QwtPickerTrackerMachine>

#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include <QDateTime>
#include <QColor>
#include <QPen>

#include "data/candledata.h"

#include "widgets/excharttracker.h"

#include "logger.h"

namespace crex::chart {

namespace {

    class DateScaleDraw : public QwtDateScaleDraw
    {
    public:
        DateScaleDraw( Qt::TimeSpec timeSpec )
            : QwtDateScaleDraw( timeSpec )
        {
            setDateFormat( QwtDate::Minute, "hh:mm\nddd dd MMM" );
            setDateFormat( QwtDate::Hour, "hh:mm\nddd dd MMM" );
            setDateFormat( QwtDate::Day, "ddd dd MMM" );
            setDateFormat( QwtDate::Week, "Www" );
            setDateFormat( QwtDate::Month, "MMM" );
            setDateFormat( QwtDate::Year, "yyyy" );
        }
    };

} // namespace

ExQwtTChart::ExQwtTChart(ExchangeProtocol *protocol, Core *core, const QString symbol, QWidget *parent):
    QwtPlot(parent)
    , request(nullptr)
    , protocol(protocol)
    , wssProtocol(core->exchangeWssProtocol())
    , symbol(symbol)
    , curveData(new crex::data::CurveData())
{
    setTitle(symbol);

    setAxisScaleDraw(QwtAxis::XBottom, new DateScaleDraw(Qt::UTC));
    setAxisScaleEngine(QwtAxis::XBottom, new QwtDateScaleEngine(Qt::UTC));

    auto yScaleEngine = new QwtLinearScaleEngine();
    yScaleEngine->setAttribute(QwtScaleEngine::Floating);
    setAxisScaleEngine(QwtAxis::YRight, yScaleEngine);

    setAxisVisible(QwtAxis::YLeft, false);
    setAxisVisible(QwtAxis::YRight, true);
//    setAxisTitle(QwtAxis::YRight, QString("USDT"));

    createXZoom();

    createCurve();

    createTracker();

    (void) new QwtPlotPanner( canvas() );

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

    curve->setSymbolExtent(60 * 1000.0);

    curve->setData(curveData);

    curve->attach(this);

    curve->setVisible(true);
}

void ExQwtTChart::createXZoom()
{
    QwtPlotMagnifier *zoom_x = new QwtPlotMagnifier(canvas());
//    zoom_x->setWheelModifiers(Qt::ControlModifier);
    zoom_x->setAxisEnabled(Qt::XAxis, true);
    zoom_x->setAxisEnabled(Qt::YAxis, false);
    zoom_x->setWheelFactor(1.1);
    zoom_x->setMouseButton(Qt::NoButton, Qt::NoModifier);
    zoom_x->setZoomInKey(Qt::Key_Plus, Qt::ControlModifier | Qt::ShiftModifier);
    zoom_x->setZoomOutKey(Qt::Key_Minus, Qt::ControlModifier);
}

void ExQwtTChart::createTracker()
{
    ExChartTracker *tracker = new ExChartTracker(this->canvas());
    tracker->setStateMachine(new QwtPickerTrackerMachine());
    tracker->setRubberBandPen(QPen("MediumOrchid"));
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
    auto dsize = curve->dataSize();
    if (dsize > 0)
    {
        startTime = curve->sample(dsize - 1).time;
    }

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

    curveData->values().append(samples);

    setAxisScaleDraw(QwtAxis::XBottom, new DateScaleDraw(Qt::UTC));

    replot();
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
