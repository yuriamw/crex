#include "widgets/exaxis.h"

#include <QVariant>
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
    , max_(0)
    , min_(0)
    , geom_hint_(getAxisMinimumSize(orientation_, font_))
{
    if (orientation_ == Qt::Horizontal)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    }
    else
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    }
    setRange(min_, max_);
}

// --------------------------------------------------------------------------------
// --- Inherited from QGraphicsLayoutItem -----------------------------------------

QSizeF ExAxis::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            if (orientation_ == Qt::Horizontal)
            {
                return QSizeF(geometry().width(), geom_hint_);
            }
            else
            {
                return QSizeF(geom_hint_, geometry().height());
            }
        case Qt::MaximumSize:
            return QSizeF(100000,100000);
        default:
            break;
    }
    return constraint;
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
    painter->drawRect(0, 0, size().width(), size().height());
}

// --------------------------------------------------------------------------------

void ExAxis::paintHorizontal(QPainter *painter)
{
    painter->drawText(QRectF(20, 1, size().width() - 20, size().height()), "Horizontal");
}

void ExAxis::paintVertical(QPainter *painter)
{
    // max_ on top
    const auto fontHeight = fontMetrics().height();
    const auto topVal = max_ - fontHeight + AxisDecoration_OuterMargin; // max_ == height in pixels!!!
    const QString topLabel(QString("%1").arg(topVal, 0, 'f', 2));
    const auto topY = fontHeight + AxisDecoration_OuterMargin;
    painter->drawText(QPointF(AxisDecoration_Size, topY), topLabel);
    painter->drawLine(0, topY, AxisDecoration_MajorLevelSize, topY);

    // min_ on bottom
    const qreal botVal = min_ + AxisDecoration_OuterMargin;
    const QString botLabel(QString("%1").arg(botVal, 0, 'f', 2));
    const auto botY = size().height() - AxisDecoration_OuterMargin;
    painter->drawText(QPointF(AxisDecoration_Size, botY), botLabel);
    painter->drawLine(0, botY, AxisDecoration_MajorLevelSize, botY);

    paintVerticalLevels(painter, botY, topY);
}

void ExAxis::paintVerticalLevels(QPainter *painter, const qreal botY, const qreal topY)
{
    const auto fontHeight = fontMetrics().height();

    const auto range = botY - topY;
    const auto minStep = fontHeight * 3;
    if (range / minStep < 2)
        return;

    const int count = range / minStep; // MUST be int
    const auto levelStep = range / count ;
    for (auto i = topY + levelStep; i < botY; i += levelStep)
    {
        const qreal val = size().height() - i + AxisDecoration_OuterMargin;
        const QString label(QString("%1").arg(val, 0, 'f', 2));
        painter->drawText(QPointF(AxisDecoration_Size, i), label);
        painter->drawLine(0, i, AxisDecoration_MinorLevelSize, i);
    }
}

void ExAxis::calculateSizeHint()
{
    if (orientation_ == Qt::Horizontal)
    {
        int fontHeight = fontMetrics().height();
        geom_hint_ = fontHeight + AxisDecoration_Size + AxisDecoration_OuterMargin;

        QRectF r = geometry();
        r.setHeight(geom_hint_);
        setGeometry(r);

        updateGeometry();
    }
    else
    {
        const auto calc_hint = [&](qreal what)
        {
            int fontHeight = fontMetrics().height();
            qreal val = what - fontHeight + AxisDecoration_OuterMargin;
            QString label = QString("%1").arg(val, 0, 'f', 2);
            int labelWidth = fontMetrics().horizontalAdvance(label);
            return labelWidth + AxisDecoration_Size + AxisDecoration_OuterMargin;
        };
        auto topHint = calc_hint(max_);
        auto botHint = calc_hint(min_);
        geom_hint_ = std::max(topHint, botHint);

        QRectF r = geometry();
        r.setWidth(geom_hint_);
        setGeometry(r);

        updateGeometry();
    }
}

// --------------------------------------------------------------------------------

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
