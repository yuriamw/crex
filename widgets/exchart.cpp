#include "widgets/exchart.h"

namespace crex::ch {

ExChart::ExChart(int num, QGraphicsItem * parent)
    : ExItem(num, parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

QRectF ExChart::boundingRect() const
 {
    return QRectF(QPointF(0, 0), geometry().size());
 }

 void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
     (void)option;
     (void)widget;

     painter->drawRect(0, 0, geometry().width(), geometry().height());
 }

} // namespace crex::ch
