#include "widgets/exchart.h"

#include "exaxis.h"

#include "logger.h"

namespace crex::ch {

ExChart::ExChart(QGraphicsItem * parent)
    : ExItem(parent)
    , vertical_axis_(nullptr)
    , horizontal_axis_(nullptr)
{
//    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setMinimumSize(48, 32);
    updateAxesGeometry();
}

int ExChart::appendCandle(qreal open, qreal close, qreal high, qreal low)
{
    crex::candle::ExCandle candle(open, close, high, low);
    candles_.append(candle);
    return candles_.size() - 1;
}

QRectF ExChart::boundingRect() const
{
    return QRectF(QPointF(0, 0), geometry().size());
}

void ExChart::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QPen pen;

    pen.setColor(Qt::blue);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawRect(0, 0, geometry().width() - 1, geometry().height() - 1);

    const auto candle_width = crex::candle::ExCandle::width() + 1;

    for (int i = 0; i < candles_.size(); i++)
    {
        const crex::candle::ExCandle & candle(candles_.at(i));

        QPolygonF p(candle.shape());

        p.translate(geometry().width() - candle_width * (i + 1),
                    geometry().height() - candle.high());

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

void ExChart::setVerticalAxis(ExAxis * axis)
{
    vertical_axis_ = axis;
}

void ExChart::setHorizontalAxis(ExAxis * axis)
{
    horizontal_axis_ = axis;
}

ExAxis *ExChart::verticalAxis() const
{
    return vertical_axis_;
}

ExAxis *ExChart::horizontalAxis() const
{
    return horizontal_axis_;
}

void ExChart::updateAxesGeometry()
{
    if (vertical_axis_)
        vertical_axis_->setRange(0, geometry().height());
    if (horizontal_axis_)
        horizontal_axis_->setRange(0, geometry().width());
}

} // namespace crex::ch
