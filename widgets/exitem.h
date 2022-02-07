#ifndef EXITEM_H
#define EXITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>

namespace crex::ch {

class ExItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    explicit ExItem(QGraphicsItem *parent = nullptr);

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
};

} // namespace crex::ch

#endif // EXITEM_H
