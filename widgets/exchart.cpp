#include "widgets/exchart.h"

namespace crex::ch {

ExChart::ExChart(QGraphicsItem * parent)
    : QGraphicsItem(parent)
{

}

QRectF ExChart::boundingRect() const
 {
     qreal penWidth = 1;
     return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
 }

 void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
     (void)option;
     (void)widget;

     painter->drawRoundedRect(-10, -10, 20, 20, 5, 5);
 }

} // namespace crex::ch
