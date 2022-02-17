#include "excandle.h"

#include <QRectF>

#include "logger.h"

namespace crex::candle {

ExCandle::ExCandle()
    : open_(0)
    , close_(0)
    , high_(0)
    , low_(0)
{
}

ExCandle::ExCandle(const qreal open, const qreal close, const qreal high, const qreal low)
    : open_(open)
    , close_(close)
    , high_(high)
    , low_(low)
{
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

void ExCandle::setCandle(const qreal open, const qreal close, const qreal high, const qreal low)
{
    open_ = open;
    close_ = close;
    high_ = high;
    low_ = low;
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

qreal ExCandle::heightHL() const
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

qreal ExCandle::heightOC() const
{
    return isUp() ? close_ - open_ : open_ - close_;
}

const QLineF ExCandle::lineHL(const qreal x, const qreal height, const qreal min, const qreal max) const
{
    // Draw line from top to bottom to make clipping easier
    const auto axis_scale = (max - min) / height;
    const auto lineY1 = (high_ - min) / axis_scale;
    const auto lineY2 = (low_ - min) / axis_scale;
    return QLineF(x, height - lineY1, x, height - lineY2);
}

const QRectF ExCandle::rectOC(const qreal x, const qreal height, const qreal min, const qreal max) const
{
    const auto axis_scale = (max - min) / height;
    const auto bodyY1 = (top() - min) / axis_scale;
    const auto bodyY2 = heightOC() / axis_scale;
    return QRectF(QPointF(x, height - bodyY1), QSizeF(width(), bodyY2)).translated( - sideWidth(), 0);
}

} // namespace crex::candle

