#include "exitem.h"

#include <QPoint>
#include <QSizeF>
#include <QRectF>
#include <QPainter>
#include <QGraphicsLineItem>
#include <QGraphicsItem>

#include "logger.h"

namespace crex::ch {

ExItem::ExItem(int num, QGraphicsItem *parent)
    : QGraphicsLayoutItem()
    , QGraphicsItem(parent)
    , num_(num)
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
            // Do not allow a size smaller than the pixmap with two frames around it.
            return QSizeF(10, 30);
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

    QColor color;
    switch (num_ & 0x3) {
    case 1:
        color = Qt::blue;
        break;
    case 2:
        color = Qt::green;
        break;
    case 3:
        color = Qt::red;
        break;
    default:
        color = Qt::black;
    }

    painter->setPen(color);
    painter->setBrush(QBrush(color));
    QPointF p(geometry().size().width() / 2, geometry().size().height() / 2);
    painter->drawEllipse(p, geometry().size().width() / 2, geometry().size().height() / 2);
}

} // namespace crex::ch
