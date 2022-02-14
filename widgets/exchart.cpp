#include "widgets/exchart.h"

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

    QPen pen;

    pen.setColor(Qt::blue);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawRect(0, 0, bound.width(), bound.height());

    const auto candle_width = crex::candle::ExCandle::width() + 1;

    for (int i = 0; i < candles_.size(); i++)
    {
        const crex::candle::ExCandle & candle(candles_.at(i));

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
