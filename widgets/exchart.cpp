#include "widgets/exchart.h"

namespace crex::ch {

ExChart::ExChart(int num, QGraphicsItem * parent)
    : ExItem(num, parent)
{
}

QRectF ExChart::boundingRect() const
 {
    return QRectF(QPointF(0, 0), geometry().size());
 }

 void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
     (void)option;
     (void)widget;

     painter->drawRoundedRect(1, 1, 20, 20, 5, 5);
 }

} // namespace crex::ch
