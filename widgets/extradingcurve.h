#pragma once

#include <QwtPlotTradingCurve>
#include <QwtOHLCSample>

#include <QPainter>

namespace crex::chart {

class ExTradingCurve : public QwtPlotTradingCurve
{
public:
    ExTradingCurve();
    void drawUserSymbol(QPainter* painter, QwtPlotTradingCurve::SymbolStyle symbolStyle, const QwtOHLCSample& sample, Qt::Orientation orientation, bool inverted, double symbolWidth) const QWT_OVERRIDE;
};

} // namespace crex::chart

