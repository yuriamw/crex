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
#include "exaxis.h"

namespace crex::ch {

class ExChart : public ExItem
{
public:
    ExChart(QGraphicsItem *parent = nullptr);

    int appendCandle(qreal open, qreal close, qreal high, qreal low);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    ExAxis *verticalAxis() const;
    ExAxis *horizontalAxis() const;
    void setVerticalAxis(ExAxis * axis);
    void setHorizontalAxis(ExAxis * axis);
    void updateAxesGeometry();

private:
    QList<crex::candle::ExCandle> candles_;

    ExAxis *vertical_axis_;
    ExAxis *horizontal_axis_;
};

} // namespace crex::ch

#endif // CHART_H
