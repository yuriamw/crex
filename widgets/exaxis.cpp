#include "widgets/exaxis.h"

#include <QVariant>

namespace crex::ch {

ExAxis::ExAxis(const Qt::Orientation orientation, QGraphicsItem * parent)
    : ExItem(parent)
    , orientation_(orientation)
{
    if (orientation_ == Qt::Horizontal)
    {
        setMaximumHeight(24);
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    }
    else
    {
        setMaximumWidth(24);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    }
}

QRectF ExAxis::boundingRect() const
 {
    return QRectF(QPointF(0, 0), geometry().size());
 }

const QVariant ExAxis::min() const
{
    return min_;
}

const QVariant ExAxis::max() const
{
    return max_;
}

void ExAxis::setMax(const QVariant &  max)
{
    max_ = max;
}

void ExAxis::setMin(const QVariant &  min)
{
    min_ = min;
}

void ExAxis::setRange(const QVariant & min, const QVariant & max)
{
    min_ = min;
    max_ = max;
}

 void ExAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
    (void)option;
    (void)widget;

    painter->drawRect(0, 0, geometry().width(), geometry().height());

    QString text;
    if (orientation_ == Qt::Horizontal)
    {
        text = "Horizontal";
    }
    else
    {
        text = "Vertical";
    }

    if (orientation_ == Qt::Vertical)
    {
        painter->rotate(90);
        painter->translate(0, - geometry().width());
        painter->drawText(QRectF(20, 1, geometry().height() - 21, geometry().width() - 1), text);
        painter->translate(0, geometry().width());
        painter->rotate(-90);
    }
    else
        painter->drawText(QRectF(20, 1, geometry().width() - 21, geometry().height() - 1), text);
}

} // namespace crex::ch
