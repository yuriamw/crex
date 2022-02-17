#include "widgets/exchart.h"

#include <QLineF>
#include <QRectF>
#include <QPointF>
#include <QMarginsF>

#include "exaxis.h"

#include "logger.h"

namespace crex::ch {

ExChart::ExChart(QGraphicsItem * parent)
    : ExItem(parent)
    , vertical_axis_(nullptr)
    , horizontal_axis_(nullptr)
    , tool_box_(new ExItem(this))
{
    updateAxesGeometry();
}

int ExChart::appendCandle(qreal open, qreal close, qreal high, qreal low)
{
    crex::candle::ExCandle candle(open, close, high, low);
    candles_.append(candle);
    return candles_.size() - 1;
}

void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    const QSizeF area(paintArea());
    const QSizeF bound(area - QSizeF(1, 1));

    // Outline
    {
        QPen pen;

        pen.setColor(Qt::blue);
        pen.setWidth(1);
        painter->setPen(pen);
    painter->drawRect(0, 0, bound.width(), bound.height());
    }

    // Margins
    {
        QPen oldPen(painter->pen());
        QBrush oldBrush(painter->brush());

        QPen pen(painter->pen());
        pen.setColor(Qt::lightGray);
        pen.setWidth(1);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        QBrush brush(painter->brush());
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);

        QRectF rect(QPointF(0, 0), paintArea());
        rect -= axis(Qt::Vertical)->margins();
        painter->drawRect(rect);

        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }

    // Candles
    const auto candle_width = crex::candle::ExCandle::width() + 1;
    const auto ax = axis(Qt::Vertical);
    const QSizeF clipSize(bound - QSizeF(0, ax->margins().bottom()));
    const QRectF clipRect(QPointF(0, ax->margins().top()), QSizeF(bound.width(), bound.height() - ax->margins().top() - ax->margins().bottom()));

    const auto visible_candles = bound.width() / candle_width;
    const auto candles_count = candles_.size() > visible_candles ? visible_candles : candles_.size();

    for (int i = 0; i < candles_count - 1; i++)
    {
        const crex::candle::ExCandle & candle(candles_.at(i));

        // Draw line from top to bottom to make clipping easier
        QLineF lineHL(candle.lineHL(clipSize.width() - candle_width * (i + 1), clipSize.height(), ax->min(), ax->max()));
        QRectF rectOC(candle.rectOC(clipSize.width() - candle_width * (i + 1), clipSize.height(), ax->min(), ax->max()));

        // Clip HL line
        const int TOP = 1;
        const int BOT = 2;
        const auto vcode = [&](const qreal y)
        {
            return (y < clipRect.top() ? TOP : 0) | (y > clipRect.bottom() ? BOT : 0);
        };

        const int outside_1 = vcode(lineHL.y1());
        const int outside_2 = vcode(lineHL.y2());
        // Both on the same side out of rect
        bool outsideHL = outside_1 & outside_2;
        if (! outsideHL)
        {
            // Intersect top of rect
            if (outside_1 && (! outside_2))
            {
                lineHL.setP1(QPointF(lineHL.x1(), clipRect.top()));
            }
            // Intersect bottom of rect
            else if (( !outside_1) && outside_2)
            {
                lineHL.setP2(QPointF(lineHL.x2(), clipRect.bottom()));
            }
            // Intersect top AND bottom of rect
            else if (outside_1 && outside_2)
            {
                lineHL.setP1(QPointF(lineHL.x1(), clipRect.top()));
                lineHL.setP2(QPointF(lineHL.x2(), clipRect.bottom()));
            }
        }

        // Clip body rect
        rectOC &= clipRect;

        if (rectOC.isEmpty() && outsideHL)
            continue;

        QBrush brush;
        QPen pen;
        QColor color(candle.isDown() ? Qt::red : Qt::darkGreen);

        brush.setStyle( Qt::SolidPattern);
        brush.setColor(color);
        pen.setStyle( Qt::SolidLine);
        pen.setColor(color);
        pen.setWidth(1);

        painter->setBrush(brush);
        painter->setPen(pen);

        painter->drawLine(lineHL);
        painter->drawRect(rectOC);
    }
}

const QSizeF ExChart::paintArea() const
{
    QSizeF area(size());
    QSizeF delta(0, 0);

    if (vertical_axis_)
        delta.setWidth(vertical_axis_->size().width());
    if (horizontal_axis_)
        delta.setHeight(horizontal_axis_->size().height());

    area -= delta;
    return area;
}

void ExChart::setSize(const QSizeF &new_size)
{
    ExItem::setSize(new_size);
    updateAxesGeometry();
}

void ExChart::setAxisGeometry(ExAxis * axis)
{
    if (!axis)
        return;

    if (axis->orientation() == Qt::Horizontal)
    {
        // Horizontal axis does not allow to change height
        axis->setSize(QSizeF(paintArea().width(), 0));
        axis->setPos(0, size().height() - axis->size().height());
    }
    else
    {
        // Vertical axis does not allow to change width
        axis->setSize(QSizeF(0, paintArea().height()));
        axis->setPos(size().width() - axis->size().width(), 0);
    }
    setToolBoxGeometry();
}

void ExChart::setToolBoxGeometry()
{
    QSizeF sz(tool_box_->size());

    if (vertical_axis_)
        sz.setWidth(vertical_axis_->size().width());
    if (horizontal_axis_)
        sz.setHeight(horizontal_axis_->size().height());
    tool_box_->setSize(sz);

    QSizeF pa(paintArea());
    QPointF pt(pa.width(), pa.height());
    tool_box_->setPos(pt);
}

void ExChart::setAxis(Qt::Orientation orientation, ExAxis * axis)
{

    if (orientation == Qt::Horizontal)
        horizontal_axis_ = axis;
    else
        vertical_axis_ = axis;

    setAxisGeometry(this->axis(orientation));
}

ExAxis *ExChart::axis(Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal)
        return horizontal_axis_;
    else
        return vertical_axis_;

}

void ExChart::updateAxesGeometry()
{
    setAxisGeometry(horizontal_axis_);
    setAxisGeometry(vertical_axis_);
}

} // namespace crex::ch
