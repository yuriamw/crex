#include "widgets/exchart.h"

#include "logger.h"

namespace crex::ch {

ExChart::ExChart(QGraphicsItem * parent)
    : ExItem(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

int ExChart::appendCandle(qreal open, qreal close, qreal high, qreal low)
{
    crex::candle::ExCandle candle(open, close, high, low);
    candles_.append(candle);
    return candles_.size() - 1;
}

QRectF ExChart::boundingRect() const
{
    return QRectF(QPointF(0, 0), geometry().size());
}

void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QBrush brush;
    QPen pen;
    QColor color(Qt::black);

    brush.setStyle( Qt::SolidPattern);
    brush.setColor(color);
    pen.setColor(color);
    painter->drawRect(0, 0, geometry().width(), geometry().height());

    const auto candle_width = crex::candle::ExCandle::width() + 1;

    for (int i = 0; i < candles_.size(); i++)
    {
        const crex::candle::ExCandle & candle(candles_.at(i));

        QPolygonF p(candle.shape());

        p.translate(geometry().width() - candle_width * (i + 1),
                    geometry().height() - candle.high());

        QBrush brush;
        QPen pen;
        QColor color(candle.isDown() ? Qt::red : Qt::darkGreen);

        brush.setStyle( Qt::SolidPattern);
        brush.setColor(color);
        pen.setColor(color);

        painter->setBrush(brush);
        painter->setPen(pen);

        painter->drawPolygon(p);
    }
}

} // namespace crex::ch
