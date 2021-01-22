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

    void scrollWithPixels(QPoint pixels);
    void scrollWithDegrees(QPoint steps);

private:
    bool scrollFit_;
};

#endif // EXCHART_H
