#ifndef CHART_H
#define CHART_H

#include "widgets/exitem.h"

#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

namespace crex::ch {

class ExChart : public ExItem
{
public:
    ExChart(int num = 0, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

} // namespace crex::ch

#endif // CHART_H
