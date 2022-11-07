#ifndef EXQWTCHART_H
#define EXQWTCHART_H

#include <QwtPlot>
#include <QwtOHLCSample>
#include <QwtPlotTradingCurve>

#include <QJsonArray>

#include <core/core.h>

#include "exchangeprotocol.h"
#include "exchange/exchangerequest.h"
#include "exchange/exwssprotocol.h"
#include "exchange/exwssrequest.h"

namespace crex::chart {

class ExQwtTChart : public QwtPlot
{
    Q_OBJECT
public:
    ExQwtTChart(ExchangeProtocol *protocol, Core *core, const QString symbol, QWidget *parent = nullptr);

private slots:
    void onTimer();
    void onCandleDataReady();

private:
    ExchangeRequest *request;
    ExchangeProtocol *protocol;
    ExWssProtocol *wssProtocol;
    QString symbol;

    QwtPlotTradingCurve *curve;

private:
    void createCurve();

    void parseJSON(QByteArray &json_data);
    QwtOHLCSample parseJSONCandle(const QJsonArray &json);

};

} // namespace crex::chart
#endif // EXQWTCHART_H
