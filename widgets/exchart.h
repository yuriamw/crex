#ifndef CHART_H
#define CHART_H

#include "widgets/exitem.h"

#include <QList>
#include <QRectF>
#include <QPainter>
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

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    ExAxis *axis(Qt::Orientation orientation) const;
    void setAxis(Qt::Orientation orientation, ExAxis * axis);

    virtual void setSize(const QSizeF & size) override;

private:
    const QSizeF paintArea() const;
    void setAxisGeometry(ExAxis * axis);
    void setToolBoxGeometry();
    void updateAxesGeometry();

private:
    QList<crex::candle::ExCandle> candles_;

    ExAxis *vertical_axis_;
    ExAxis *horizontal_axis_;
    ExItem *tool_box_;
};

} // namespace crex::ch

#endif // CHART_H
