#ifndef EXCANDLE_H
#define EXCANDLE_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

namespace crex::candle {

auto const candle_side_width = 2;

class ExCandle
{
public:
    explicit ExCandle(QGraphicsItem *parent = nullptr);
    void setCandle(qreal open, qreal close, qreal high, qreal low);

    QGraphicsItem *item();

    static qreal width();
    qreal height();

private:
    QGraphicsPolygonItem *polygon_;
    qreal open_;
    qreal close_;
    qreal high_;
    qreal low_;
};

} // namespace crex::candle

#endif // EXCANDLE_H
