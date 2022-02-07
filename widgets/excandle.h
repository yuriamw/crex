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
    explicit ExCandle(const qreal open, const qreal close, const qreal high, const qreal low, QGraphicsItem *parent = nullptr);
    void setCandle(const qreal open, const qreal close, const qreal high, const qreal low);

    qreal open() const;
    qreal close() const;
    qreal high() const;
    qreal low() const;

    QGraphicsItem *item();
    const QPolygonF & shape() const;
    bool isUp() const;
    bool isDown() const;

    static qreal width();
    qreal height() const;

private:
    void prepareCandlePolygon();

private:
    QGraphicsPolygonItem *polygon_item_;
    QPolygonF polygon_;
    qreal open_;
    qreal close_;
    qreal high_;
    qreal low_;
};

} // namespace crex::candle

#endif // EXCANDLE_H
