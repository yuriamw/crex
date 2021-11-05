#include "widgets/exchart.h"

namespace crex::ch {

ExChart::ExChart(int num, QGraphicsItem * parent)
    : ExItem(num, parent)
{

}

QRectF ExChart::boundingRect() const
 {
     qreal penWidth = 1;
     return QRectF(1, 1, 20 + penWidth, 20 + penWidth);
 }

 void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
     (void)option;
     (void)widget;

     painter->drawRoundedRect(1, 1, 20, 20, 5, 5);
 }

} // namespace crex::ch
