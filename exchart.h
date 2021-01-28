#ifndef EXCHART_H
#define EXCHART_H

#include <QChartView>
#include <QCandlestickSet>
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

////////////////////////////////////////////////////////////////////////////////
/// model

namespace Defaults {

    static const qint64 WIDTH    = 6;
    static const qint64 SPACING  = 4;
    static const qint64 RMARGIN  = 40;
    static const qint64 TIMEOUT  = 250;
    static const qreal  MIN_P    = 0.95; // 5% lower
    static const qreal  MAX_P    = 1.05; // 5% higher

    static const int    MAX_COLUMN = 5;

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
        qreal t;
    };
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

//    QDateTime oldestData();
//    QDateTime youngestData();
    QMap<QString, QDateTime> dataRange();

    qreal minLowValue();
    qreal maxHighValue();

private:
    QList<struct Defaults::candle_data> candle_data_;
};

////////////////////////////////////////////////////////////////////////////////
/// ChartView

class ExChart : public QtCharts::QChartView
{
public:
    ExChart();

    bool scrollFit();
    void setScrollFit(bool fit);

    qint64 timeFrame(); // seconds
    bool setTimeFrame(qint64 t); // seconds

    QAbstractTableModel *model();

private slots:
    void onHover(bool status, QtCharts::QCandlestickSet *set);

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

private:
    QPoint dragStart_;
    bool scrollFit_;
    qint64 timeFrame_;
    ExModel *model_;
};

#endif // EXCHART_H
