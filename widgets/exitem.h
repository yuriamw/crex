#ifndef EXITEM_H
#define EXITEM_H

#include <QGraphicsLayoutItem>
#include <QGraphicsItem>

namespace crex::w {

class ExItem : public QGraphicsLayoutItem, public QGraphicsItem
{
public:
    explicit ExItem(int num = 0, QGraphicsItem *parent = nullptr);

    // Inherited from QGraphicsLayoutItem
    void setGeometry(const QRectF &geom) override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

    // Inherited from QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
    int num_;
};

} // namespace crex::w

#endif // EXITEM_H
