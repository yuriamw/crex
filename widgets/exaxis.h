#ifndef EXSCALE_H
#define EXSCALE_H

#include "widgets/exitem.h"

#include <QVariant>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

namespace crex::ch {

class ExAxis : public ExItem
{
public:
    ExAxis(const Qt::Orientation orientation = Qt::Horizontal, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    const QVariant min() const;
    const QVariant max() const;
    void setMax(const QVariant &  max);
    void setMin(const QVariant &  min);
    void setRange(const QVariant & min, const QVariant & max);

private:
    Qt::Orientation orientation_;
    QVariant max_;
    QVariant min_;
};

} // namespace crex::ch

#endif // EXSCALE_H
