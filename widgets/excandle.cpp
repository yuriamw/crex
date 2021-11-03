#include "excandle.h"

#include <QPolygonF>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QColor>

namespace crex::candle {

ExCandle::ExCandle(QGraphicsItem *parent)
    : polygon_(new QGraphicsPolygonItem(parent))
{

}

QGraphicsItem *ExCandle::item()
{
    return polygon_;
}

void ExCandle::setCandle(qreal open, qreal close, qreal high, qreal low)
{
    open_ = open;
    close_ = close;
    high_ = high;
    low_ = low;

    QBrush brush;
    QPen pen;
    QColor color(open > close ? Qt::red : Qt::darkGreen);

    brush.setStyle( Qt::SolidPattern);
    brush.setColor(color);
    pen.setColor(color);

    QPolygonF p;
    qreal top = open > close ? open : close;
    qreal bottom = open > close ? close : open;

    p << QPointF(candle_side_width + 1, high - high);
    p << QPointF(candle_side_width + 1, high - top);
    p << QPointF(candle_side_width * 2 + 1, high - top);
    p << QPointF(candle_side_width * 2 + 1, high - bottom);
    p << QPointF(candle_side_width + 1, high - bottom);
    p << QPointF(candle_side_width + 1, high - low);
    p << QPointF(candle_side_width + 1, high - bottom);
    p << QPointF(0, high - bottom);
    p << QPointF(0, high - top);
    p << QPointF(candle_side_width + 1, high - top);

    polygon_->setBrush(brush);
    polygon_->setPen(pen);
    polygon_->setPolygon(p);
}

qreal ExCandle::width()
{
    return candle_side_width * 2 + 1;
}

qreal ExCandle::height()
{
    return high_ - low_;
}
} // namespace crex::candle

