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
            return QSizeF(6, 6);
        case Qt::PreferredSize:
            return QSizeF(24, 24);
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

    painter->setPen(Qt::black);
    painter->drawRect(0, 0, size().width() - 1, size().height() - 1);

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

QSizeF ExItem::size() const
{
    return QSizeF(geometry().size() - QSizeF(1, 1));
}

} // namespace crex::ch
