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
    const auto axis_scale = (ax->max() - ax->min()) / (ax->size().height() - ax->margins().top() - ax->margins().bottom());
    const QSizeF clipSize(bound - QSizeF(0, ax->margins().bottom()));
    const QRectF clipRect(QPointF(0, ax->margins().top()), QSizeF(bound.width(), bound.height() - ax->margins().top() - ax->margins().bottom()));

    for (int i = 0; i < candles_.size(); i++)
    {
        const crex::candle::ExCandle & candle(candles_.at(i));
#if 1
        // Draw line from top to bottom to allow to translate it to rect
        const auto lineY1 = (candle.high() - ax->min()) / axis_scale;
        const auto lineY2 = (candle.low() - ax->min()) / axis_scale;
        QLineF candle_line(0.0, clipSize.height() - lineY1, 0.0, clipSize.height() - lineY2);
        candle_line.translate(clipSize.width() - candle_width * (i + 1), 0);

        const auto bodyY1 = (candle.top() - ax->min()) / axis_scale;
        const auto bodyY2 = candle.openClose() / axis_scale;
        QRectF candle_body(QPointF(clipSize.width() - candle_width * (i + 1), clipSize.height() - bodyY1), QSizeF(candle.width(), bodyY2));
        candle_body.translate( - candle.sideWidth(), 0);

        // Convert line to rect and clip both HL-line and OC-body
        QRectF line_rect( candle_line.p1(), QSizeF(0.5, candle_line.dy()) );
        line_rect &= clipRect;
        QRectF body_rect(clipRect & candle_body);

        if ( body_rect.isEmpty() && line_rect.isEmpty())
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

        // QRectF with 1 px size drwas it's shape using two lines
        // the resulting drawings is 2 px
        // We draw it as painter->pen().width() line
        painter->drawLine(line_rect.x(), line_rect.y(), line_rect.x(), line_rect.y() + line_rect.height());
        painter->drawRect(body_rect);
#else
        QPolygonF p(candle.shape());

        p.translate(bound.width() - candle_width * (i + 1),
                    bound.height() - candle.high());

        QBrush brush;
        QPen pen;
        QColor color(candle.isDown() ? Qt::red : Qt::darkGreen);

        brush.setStyle( Qt::SolidPattern);
        brush.setColor(color);
        pen.setColor(color);

        painter->setBrush(brush);
        painter->setPen(pen);

        painter->drawPolygon(p);
#endif
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
    setAxisGeometry(vertical_axis_);
    setAxisGeometry(horizontal_axis_);
}

} // namespace crex::ch
