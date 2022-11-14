#include "extradingcurve.h"

#include <QwtPainter>

namespace crex::chart {

ExTradingCurve::ExTradingCurve()
    : QwtPlotTradingCurve()
{

}

void ExTradingCurve::drawUserSymbol( QPainter* painter, QwtPlotTradingCurve::SymbolStyle symbolStyle, const QwtOHLCSample& sample, Qt::Orientation orientation, bool inverted, double symbolWidth ) const
{
    Q_UNUSED(symbolStyle);

    // sample is translated to paint device coordinates so
    // here we have it upside down for paint device with (0,0) at top left corner.
    // That is whyle open > close == increasing
    auto direction = [](const bool inverted, const QwtOHLCSample &sample){
        if (inverted)
            return sample.open <= sample.close ? QwtPlotTradingCurve::Increasing : QwtPlotTradingCurve::Decreasing;

        return sample.open >= sample.close ? QwtPlotTradingCurve::Increasing : QwtPlotTradingCurve::Decreasing;
    };

    const QBrush brush = symbolBrush(direction(inverted, sample));
    const QColor color = brush.color();
    QPen pen = painter->pen();
    pen.setColor(color);
    painter->setPen(pen);

    const double t = sample.time;
    const double v1 = sample.low;
    const double v2= sample.high;

    if ( orientation == Qt::Vertical )
    {
        QwtPainter::drawLine(painter, t, v1, t, v2);

        QRectF rect(t - 0.5 * symbolWidth, sample.open, symbolWidth, sample.close - sample.open);
        QwtPainter::drawRect(painter, rect);
    }
    else
    {
        QwtPainter::drawLine(painter, v1, t, v2, t);

        const QRectF rect(sample.open, t - 0.5 * symbolWidth, sample.close - sample.open, symbolWidth);
        QwtPainter::drawRect(painter, rect);
    }
}

} // namespace crex::chart
