#pragma once

#include <QwtPlot>
#include <QwtOHLCSample>
#include <QwtPlotTradingCurve>
#include <QwtPlotMarker>

#include <QJsonArray>

#include <core/core.h>

#include <data/curvedata.h>

#include "exchangeprotocol.h"
#include "exchange/exchangerequest.h"
#include "exchange/exwssprotocol.h"
#include "exchange/exwssrequest.h"

namespace crex::chart {

class ExQwtChart : public QwtPlot
{
    Q_OBJECT
public:
    ExQwtChart(ExchangeProtocol *protocol, Core *core, const QString symbol, QWidget *parent = nullptr);

private slots:
    void onTimer();
    void onCandleDataReady();

private:
    ExchangeRequest *request;
    ExchangeProtocol *protocol;
    ExWssProtocol *wssProtocol;
    QString symbol;

    crex::data::CurveData *curveData;
    QwtPlotTradingCurve *curve;
    QwtPlotMarker *vLineMarker;
    QwtPlotMarker *hLineMarker;

private:
    void createCurve();
    void createXZoom();
    void createTracker();

    void parseJSON(QByteArray &json_data);
    QwtOHLCSample parseJSONCandle(const QJsonArray &json);
};

} // namespace crex::chart
