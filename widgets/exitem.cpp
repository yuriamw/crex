#include "exitem.h"

#include <QPoint>
#include <QSizeF>
#include <QRectF>
#include <QPainter>
#include <QGraphicsItem>

#include "logger.h"

namespace crex::ch {

ExItem::ExItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , size_(QSizeF(2, 2))
{
}

// Inherited from QGraphicsItem
QRectF ExItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), size());
}

void ExItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::black);
    painter->drawRect(0, 0, size(true).width(), size(true).height());

    painter->setPen(Qt::magenta);
    painter->setBrush(QBrush(Qt::magenta));
    QPointF p(size().width() / 2 + 1, size().height() / 2 + 1);
    painter->drawEllipse(p, size().width() / 2 - 2, size().height() / 2 - 2);

    painter->setPen(Qt::cyan);
    painter->setBrush(QBrush(Qt::cyan));
    QPointF tl(QPointF(size().width() / 4, size().height() / 4));
    QPointF br(tl);
    br *= 3;
    painter->drawRect(QRectF(tl, br));
}

void ExItem::setSize(const QSizeF &size)
{
    prepareGeometryChange();
    size_ = size;
}

const QSizeF ExItem::size(bool bounded) const
{
    if (bounded)
        return QSizeF(size_ - QSizeF(1, 1));
    return size_;
}

} // namespace crex::ch
