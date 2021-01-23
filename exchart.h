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

class ExChart : public QtCharts::QChartView
{
public:
    ExChart();

    bool scrollFit();
    void setScrollFit(bool fit);

    qint64 timeFrame(); // seconds
    bool setTimeFrame(qint64 t); // seconds

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
};

#endif // EXCHART_H
