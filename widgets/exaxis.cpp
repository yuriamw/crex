#include "widgets/exaxis.h"

#include <QFont>
#include <QFontMetrics>
#include <QMarginsF>

#include "logger.h"

namespace crex::ch {

namespace {
    const auto AxisDecoration_OuterMargin       = 2.0;

    const auto AxisDecoration_MajorLevelSize    = 4.0;
    const auto AxisDecoration_MinorLevelSize    = 2.0;
    const auto AxisDecoration_Size              = AxisDecoration_MajorLevelSize + AxisDecoration_OuterMargin;

    qreal getAxisMinimumSize(const Qt::Orientation orient, const QFont & fnt)
    {
        const auto size = orient == Qt::Horizontal ? QFontMetrics(fnt).height() : QFontMetrics(fnt).horizontalAdvance('0');
        return size + AxisDecoration_Size + AxisDecoration_OuterMargin;
    }
}

ExAxis::ExAxis(const Qt::Orientation orientation, QGraphicsItem * parent)
    : ExItem(parent)
    , font_(QFont())
    , orientation_(orientation)
    , min_(0)
    , max_(1)
    , geom_hint_(getAxisMinimumSize(orientation_, font_))
    , axis_format_("f")
    , precision_(2)
{
    calculateSizeHint();
}

// --------------------------------------------------------------------------------
// --- Inherited from QGraphicsItem -----------------------------------------------

void ExAxis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    painter->setFont(font_);
    if (orientation_ == Qt::Vertical)
    {
        paintVertical(painter);
    }
    else
    {
        paintHorizontal(painter);
    }

    QPen pen(painter->pen());
    pen.setWidth(1);

    if (orientation_ == Qt::Horizontal)
        pen.setColor(Qt::red);
    else
        pen.setColor(Qt::green);

    painter->setPen(pen);
    painter->drawRect(0, 0, size(true).width(), size(true).height());
}

// --------------------------------------------------------------------------------

void ExAxis::paintHorizontal(QPainter *painter)
{
    painter->drawText(QRectF(20, 1, size(true).width() - 20, size(true).height()), "Horizontal");
}

void ExAxis::paintVertical(QPainter *painter)
{
    const auto fontHeight = fontMetrics().height();

    const qreal topValPos = AxisDecoration_OuterMargin;
    const qreal botValPos = size(true).height() - fontHeight - AxisDecoration_OuterMargin;

    const auto range = botValPos - topValPos;
    const auto minStep = fontHeight * 3;

    if ((range / minStep) < 0.5)
        return;

    const int count = range / minStep < 2 ? 2 : range / minStep; // MUST be int

    const auto levelStep = range / count ;
    const auto valueStep = (max_ - min_) / count;
    QPointF valPos(AxisDecoration_OuterMargin + AxisDecoration_MajorLevelSize, topValPos);
    for (auto i = 0; i < count + 1; i++)
    {
        QSizeF valSize(size(true).width() - valPos.x() - AxisDecoration_OuterMargin, fontHeight + AxisDecoration_OuterMargin);
        QRectF valRect(valPos, valSize);
        QPointF levelPos(AxisDecoration_OuterMargin, valPos.y() + valSize.height() / 2);

        const qreal val = max_ - valueStep * i;
        const QString label(QString("%1").arg(val, 0, 'f', 2));

        if (!boundingRect().contains(valRect))
            return;

//        painter->drawRect(valRect);
        painter->drawLine(levelPos.x(), levelPos.y(), AxisDecoration_OuterMargin + AxisDecoration_MajorLevelSize, levelPos.y());
        painter->drawText(valRect, label);

        valPos += QPointF(0, levelStep);
    }
}

void ExAxis::setSize(const QSizeF & new_size)
{
    if (orientation_ == Qt::Horizontal)
        ExItem::setSize(QSizeF(new_size.width(), geom_hint_));
    else
        ExItem::setSize(QSizeF(geom_hint_, new_size.height()));
}

void ExAxis::calculateSizeHint()
{
    if (orientation_ == Qt::Horizontal)
    {
        int fontHeight = fontMetrics().height();
        geom_hint_ = fontHeight + AxisDecoration_Size + AxisDecoration_OuterMargin;

        QSizeF s = size();
        s.setHeight(geom_hint_);
        setSize(s);
    }
    else
    {
        const auto calc_hint = [&](qreal what)
        {
            int fontHeight = fontMetrics().height();
            qreal val = what - fontHeight + AxisDecoration_OuterMargin;
            QString label = QString("%1").arg(val, 0, axis_format_.toLatin1()[0], precision_);
            int labelWidth = fontMetrics().horizontalAdvance(label);
            return labelWidth + AxisDecoration_Size + AxisDecoration_OuterMargin;
        };
        auto topHint = calc_hint(max_);
        auto botHint = calc_hint(min_);
        geom_hint_ = std::max(topHint, botHint);

        QSizeF s = size();
        s.setWidth(geom_hint_);
        setSize(s);
    }
}

// --------------------------------------------------------------------------------

Qt::Orientation ExAxis::orientation() const
{
    return orientation_;
}

const QFont & ExAxis::font() const
{
    return font_;
}

void ExAxis::setFont(const QFont & font)
{
    font_ = font;
}

const QFontMetrics ExAxis::fontMetrics() const
{
    return QFontMetrics(font_);
}

const QMarginsF ExAxis::margins() const
{
    const auto margin = orientation_ == Qt::Horizontal ? 0 : fontMetrics().height() / 2 + AxisDecoration_OuterMargin;
    return QMarginsF(0, margin, 0, margin);
}

qreal ExAxis::min() const
{
    return min_;
}

qreal ExAxis::max() const
{
    return max_;
}

void ExAxis::setMax(const qreal max)
{
    setRange(min_, max);
}

void ExAxis::setMin(const qreal min)
{
    setRange(min, max_);
}

void ExAxis::setRange(const qreal min, const qreal max)
{
    if ((min_ == min) && (max_ == max))
        return;

    min_ = min;
    max_ = max;

    calculateSizeHint();
}

// --------------------------------------------------------------------------------


} // namespace crex::ch
