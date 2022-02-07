#ifndef EXSCALE_H
#define EXSCALE_H

#include "widgets/exitem.h"

#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

namespace crex::ch {

class ExScale : public ExItem
{
public:
    ExScale(const Qt::Orientation orientation = Qt::Horizontal, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Qt::Orientation orientation_;
};

} // namespace crex::ch

#endif // EXSCALE_H
