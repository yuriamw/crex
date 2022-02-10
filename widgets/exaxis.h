#ifndef EXSCALE_H
#define EXSCALE_H

#include "widgets/exitem.h"

#include <QVariant>
#include <QRectF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>
#include <QFont>
#include <QFontMetrics>

namespace crex::ch {

class ExAxis : public ExItem
{
public:
    ExAxis(const Qt::Orientation orientation = Qt::Horizontal, QGraphicsItem *parent = nullptr);

    // --- Inherited from QGraphicsLayoutItem -------------------------------------------
protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const override;

    // --- Inherited from QGraphicsItem -------------------------------------------------
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // --- Interface --------------------------------------------------------------------
public:
    const QFont & font() const;
    void setFont(const QFont & font);
    const QFontMetrics fontMetrics() const;

    qreal min() const;
    qreal max() const;
    void setMax(const qreal max);
    void setMin(const qreal min);
    void setRange(const qreal min, const qreal max);

private:
    void calculateSizeHint();
    void paintHorizontal(QPainter *painter);

    void paintVertical(QPainter *painter);
    void paintVerticalLevels(QPainter *painter, const qreal botY, const qreal topY);

private:
    QFont font_;

    Qt::Orientation orientation_;
    qreal max_;
    qreal min_;
    qreal geom_hint_;
};

} // namespace crex::ch

#endif // EXSCALE_H
