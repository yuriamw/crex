#ifndef EXCHART_H
#define EXCHART_H

#include <QChartView>
#include <QCandlestickSet>
#include <QHCandlestickModelMapper>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QResizeEvent>
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>

#include <QJsonArray>

#include "data/data.h"
#include "exchange/exchangerequest.h"
#include "exchangeprotocol.h"

////////////////////////////////////////////////////////////////////////////////
/// model

namespace Defaults {

    static const qint64 WIDTH    = 6;
    static const qint64 SPACING  = 4;
    static const qint64 RMARGIN  = 40;
    static const qint64 TIMEOUT  = 250;
    static const qreal  MIN_P    = 0.95; // 5% lower
    static const qreal  MAX_P    = 1.05; // 5% higher

    static const int    SCROLL_X_MARGIN = 2; // ticks

    static const int    MAX_COLUMN = 5;
    static const int    OPEN_COL   = 0;
    static const int    CLOSE_COL  = 1;
    static const int    HIGH_COL   = 2;
    static const int    LOW_COL    = 3;
    static const int    TIME_COL   = 4;

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

} // namespace Defaults

class ExModel : public QAbstractTableModel
{
public:
    explicit ExModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setCandles(QList<crex::data::candle_data> &candles);

//    QDateTime oldestData();
//    QDateTime youngestData();
    QMap<QString, QDateTime> dataRange();

    qreal minLowValue();
    qreal maxHighValue();

private:
    QList<struct crex::data::candle_data> candle_data_;
};

////////////////////////////////////////////////////////////////////////////////
/// ChartView

class ExQChart : public QtCharts::QChartView
{
    Q_OBJECT
public:
    ExQChart(ExchangeProtocol *protocol, const QString symbol, QWidget *parent = nullptr);

    bool scrollFit();
    void setScrollFit(bool fit);

    qint64 timeFrame(); // seconds
    bool setTimeFrame(qint64 t); // seconds

    QAbstractTableModel *model();

public slots:
    void setSymbol(const QString symbol);

private slots:
    void onHover(bool status, QtCharts::QCandlestickSet *set);
    void onTimer();
    void onCandleDataReady();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void resizeEvent(QResizeEvent *event);

private:
    QDateTime oldestData();
    QDateTime youngestData();
    QMap<QString, QDateTime> dataRange();

    qreal minLowValue();
    qreal maxHighValue();

    void zoomWithPixels(QPoint pixels);
    void zoomWithDegrees(QPoint steps);

    void scroll(QPoint steps);
    void scrollHorizontal(QPoint steps);
    void scrollVertical(QPoint steps);

    void parseJSON(QByteArray &json_data);
    crex::data::candle_data parseJSONCandle(const QJsonArray &json);

    void scaleData();
    void scaleDataX();
    void scaleDataY();
    void setAutoScale(bool ascale);

    void updateTimeLabel(QPointF localPos);

private:
    QPoint dragStart_;
    bool scrollFit_;
    qint64 timeFrame_;
    ExchangeRequest *request_;
    ExchangeProtocol *protocol_;
    QString symbol_;

    bool valid_;
    bool autoScale_;
    QDateTime timeAxisMin_;
    QDateTime timeAxisMax_;
    qreal valAxisMin_;
    qreal valAxisMax_;

    QGraphicsSimpleTextItem *timeLabel_;
    QGraphicsLineItem *timeLine_;

    ExModel *model_;
    QtCharts::QHCandlestickModelMapper *mapper_;
};

#endif // EXCHART_H
