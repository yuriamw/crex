#pragma once

#include <QwtPlotPicker>
#include <QwtText>
#include <QwtPlotTradingCurve>

#include <QWidget>
#include <QPointF>
#include <QPolygon>

namespace crex::chart {

class ExChartTracker : public QwtPlotPicker
{
public:
    ExChartTracker(QWidget *parent = nullptr);

protected:
    virtual QwtText trackerTextF(const QPointF &pos) const QWT_OVERRIDE;
    virtual QPolygon adjustedPoints( const QPolygon &points ) const QWT_OVERRIDE;

private:
    QString curveInfoAt(const QwtPlotTradingCurve *curve, const QPointF &pos) const;
    QwtOHLCSample curveSampleAt(const QwtPlotTradingCurve* curve, double x ) const;
};

} // namespace crex::chart
