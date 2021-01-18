#ifndef EXCHART_H
#define EXCHART_H

#include <QChartView>
#include <QCandlestickSet>
#include <QMouseEvent>
#include <QResizeEvent>

class ExChart : public QtCharts::QChartView
{
public:
    ExChart();

private:
    void onHover(bool status, QtCharts::QCandlestickSet *set);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // EXCHART_H
