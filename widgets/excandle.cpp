#include "excandle.h"

#include <QPolygonF>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QColor>

namespace crex::candle {

ExCandle::ExCandle(QGraphicsItem *parent)
    : polygon_item_(new QGraphicsPolygonItem(parent))
    , open_(0)
    , close_(0)
    , high_(0)
    , low_(0)
{
    prepareCandlePolygon();
}

ExCandle::ExCandle(const qreal open, const qreal close, const qreal high, const qreal low, QGraphicsItem *parent)
    : polygon_item_(new QGraphicsPolygonItem(parent))
    , open_(open)
    , close_(close)
    , high_(high)
    , low_(low)
{
    prepareCandlePolygon();
}

qreal ExCandle::open() const
{
    return open_;
}

qreal ExCandle::close() const
{
    return close_;
}

qreal ExCandle::high() const
{
    return high_;
}

qreal ExCandle::low() const
{
    return low_;
}


QGraphicsItem *ExCandle::item()
{
    return polygon_item_;
}

void ExCandle::prepareCandlePolygon()
{
    QBrush brush;
    QPen pen;
    QColor color(isDown() ? Qt::red : Qt::darkGreen);

    brush.setStyle( Qt::SolidPattern);
    brush.setColor(color);
    pen.setColor(color);

    qreal top = isDown() ? open_ : close_;
    qreal bottom = isDown() ? close_ : open_;

    QPolygonF p;
    p << QPointF(candle_side_width + 1, high_ - high_);
    p << QPointF(candle_side_width + 1, high_ - top);
    p << QPointF(candle_side_width * 2 + 1, high_ - top);
    p << QPointF(candle_side_width * 2 + 1, high_ - bottom);
    p << QPointF(candle_side_width + 1, high_ - bottom);
    p << QPointF(candle_side_width + 1, high_ - low_);
    p << QPointF(candle_side_width + 1, high_ - bottom);
    p << QPointF(0, high_ - bottom);
    p << QPointF(0, high_ - top);
    p << QPointF(candle_side_width + 1, high_ - top);

    polygon_ = p;

//    polygon_.clear();
//    polygon_ << QPointF(candle_side_width + 1, high_);
//    polygon_ << QPointF(candle_side_width + 1, top);
//    polygon_ << QPointF(candle_side_width * 2 + 1, top);
//    polygon_ << QPointF(candle_side_width * 2 + 1, bottom);
//    polygon_ << QPointF(candle_side_width + 1, bottom);
//    polygon_ << QPointF(candle_side_width + 1, low_);
//    polygon_ << QPointF(candle_side_width + 1, bottom);
//    polygon_ << QPointF(0, bottom);
//    polygon_ << QPointF(0, top);
//    polygon_ << QPointF(candle_side_width + 1, top);

    polygon_item_->setBrush(brush);
    polygon_item_->setPen(pen);
    polygon_item_->setPolygon(p);
}

void ExCandle::setCandle(const qreal open, const qreal close, const qreal high, const qreal low)
{
    open_ = open;
    close_ = close;
    high_ = high;
    low_ = low;

    prepareCandlePolygon();
}

const QPolygonF & ExCandle::shape() const
{
    return polygon_;
}

bool ExCandle::isUp() const
{
    return open_ <= close_;
}

bool ExCandle::isDown() const
{
    return open_ > close_;
}

qreal ExCandle::width()
{
    return candle_side_width * 2 + 1;
}

qreal ExCandle::sideWidth()
{
    return candle_side_width;
}

qreal ExCandle::height() const
{
    return high_ - low_;
}

qreal ExCandle::top() const
{
    return isUp() ? close_ : open_;
}

qreal ExCandle::bottom() const
{
    return isUp() ? open_ : close_;
}

qreal ExCandle::openClose() const
{
    return isUp() ? close_ - open_ : open_ - close_;
}

} // namespace crex::candle

