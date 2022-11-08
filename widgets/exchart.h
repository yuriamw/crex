#ifndef EXCHART_H
#define EXCHART_H

#include <qcustomplot.h>

#include <QSharedPointer>
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QComboBox>
#include <QLabel>
#include <QJsonArray>

#include <core/core.h>

#include "exchangeprotocol.h"
#include "exchange/exchangerequest.h"
#include "exchange/exwssprotocol.h"
#include "exchange/exwssrequest.h"

namespace crex::chart {

class ExChart : public QCustomPlot
{
    Q_OBJECT

public:
    ExChart(ExchangeProtocol *protocol, Core *core, const QString symbol, QWidget *parent = nullptr);

    void setSymbol(QString symbol);
    void setCandles(QSharedPointer<QCPFinancialDataContainer> cont);

public slots:
    void switchTF(int index);
    void scaleData();
    void scaleDataX();
    void scaleDataY();

private slots:
    void onTimer();
    void onCandleDataReady();
    void onWssDataReady();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void createTimeFrameButton();
    void updateOlhcLabelValue(const QCPFinancialData &d);
    void placeOlhcLabel(const QString &s = QString());

    void parseJSON(QByteArray &json_data);
    QCPFinancialData parseJSONCandle(const QJsonArray &json);
    void parseWssJSON(QJsonObject &data);

    int visibleCandlesCount();


private:
    QCPItemLine *hCursorLine;
    QCPItemLine *vCursorLine;
    QVector<QCPItemLine *> cursorLines;

    QCPFinancial *financial;

    QString symbol_;
    bool dataInitialized;
    bool autoScaleY;
    ExchangeRequest *request_;
    ExWssRequest *wss_request_;
    ExchangeProtocol *protocol_;
    ExWssProtocol *wss_protocol_;
    QString timeFrame;
    QComboBox *tfCombo;
    QLabel *olhcDisplay;
    QTimer *candleTimer;
};

} // namespace crex::chart

#endif // EXCHART_H
