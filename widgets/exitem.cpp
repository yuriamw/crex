#include "exitem.h"

#include <QPoint>
#include <QSizeF>
#include <QRectF>
#include <QPainter>
#include <QGraphicsLineItem>
#include <QGraphicsItem>

#include "logger.h"

namespace crex::ch {

ExItem::ExItem(QGraphicsItem *parent)
    : QGraphicsLayoutItem()
    , QGraphicsItem(parent)
{
    setGraphicsItem(this);
}

// Inherited from QGraphicsLayoutItem
void ExItem::setGeometry(const QRectF &geom)
{
    prepareGeometryChange();
    QGraphicsLayoutItem::setGeometry(geom);
    setPos(geom.topLeft());
}

QSizeF ExItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return QSizeF(100, 140);
        case Qt::MaximumSize:
            return QSizeF(100000,100000);
        default:
            break;
    }
    return constraint;
}

// Inherited from QGraphicsItem
QRectF ExItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), geometry().size());
}

void ExItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::magenta);
    painter->setBrush(QBrush(Qt::magenta));
    QPointF p(geometry().size().width() / 2 + 1, geometry().size().height() / 2 + 1);
    painter->drawEllipse(p, geometry().size().width() / 2 - 2, geometry().size().height() / 2 - 2);

    painter->setPen(Qt::cyan);
    painter->setBrush(QBrush(Qt::cyan));
    QPointF tl(QPointF(geometry().size().width() / 4, geometry().size().height() / 4));
    QPointF br(tl);
    br *= 3;
    painter->drawRect(QRectF(tl, br));
}

} // namespace crex::ch
