#ifndef EXITEM_H
#define EXITEM_H

#include <QGraphicsItem>
#include <QSizeF>

namespace crex::ch {

class ExItem : public QGraphicsItem
{
public:
    explicit ExItem(QGraphicsItem *parent = nullptr);

    // Inherited from QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    // If bounded == true decreases the size by QSizeF(1, 1) to bound to bordeds
    const QSizeF size(bool bounded = false) const;
    virtual void setSize(const QSizeF & size);

private:
    QSizeF size_;
};

} // namespace crex::ch

#endif // EXITEM_H
