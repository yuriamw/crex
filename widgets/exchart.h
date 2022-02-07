#ifndef CHART_H
#define CHART_H

#include "widgets/exitem.h"

#include <QList>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

#include "excandle.h"

namespace crex::ch {

class ExChart : public ExItem
{
public:
    ExChart(QGraphicsItem *parent = nullptr);

    int appendCandle(qreal open, qreal close, qreal high, qreal low);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QList<crex::candle::ExCandle> candles_;
};

} // namespace crex::ch

#endif // CHART_H
