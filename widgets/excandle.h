#ifndef EXCANDLE_H
#define EXCANDLE_H

#include <QLineF>
#include <QRectF>

namespace crex::candle {

auto const candle_side_width = 2;

class ExCandle
{
public:
    explicit ExCandle();
    explicit ExCandle(const qreal open, const qreal close, const qreal high, const qreal low);

    void setCandle(const qreal open, const qreal close, const qreal high, const qreal low);

    qreal open() const;
    qreal close() const;
    qreal high() const;
    qreal low() const;
    qreal top() const;
    qreal bottom() const;

    bool isUp() const;
    bool isDown() const;

    static qreal width();
    static qreal sideWidth();
    qreal heightHL() const;
    qreal heightOC() const;

    const QLineF lineHL(const qreal x, const qreal height, const qreal min, const qreal max) const;
    const QRectF rectOC(const qreal x, const qreal height, const qreal min, const qreal max) const;

private:
    qreal open_;
    qreal close_;
    qreal high_;
    qreal low_;
};

} // namespace crex::candle

#endif // EXCANDLE_H
