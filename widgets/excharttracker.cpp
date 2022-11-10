#include "excharttracker.h"

#include <QwtPickerMachine>
#include <QwtPlot>
#include <QwtPlotTradingCurve>
#include <QwtText>

#include <QPen>

#include "logger.h"

namespace crex::chart {

struct compareX
{
    inline bool operator()( const double x, const QwtOHLCSample &sample ) const
    {
        return ( x < sample.time );
    }
};


ExChartTracker::ExChartTracker(QWidget *parent)
    : QwtPlotPicker(parent)
{
    setTrackerMode(QwtPlotPicker::ActiveOnly);
    setRubberBand(VLineRubberBand);

    setStateMachine(new QwtPickerDragPointMachine());
}

QwtText ExChartTracker::trackerTextF(const QPointF &pos) const
{
    QwtText trackerText;
    trackerText.setColor(Qt::black);

    QString info;

    const QwtPlotItemList curves = plot()->itemList( QwtPlotItem::Rtti_PlotTradingCurve );
    for (int i = 0; i < curves.size(); i++)
    {
        TRACE("") << i;
        const QString curveInfo = curveInfoAt(static_cast<const QwtPlotTradingCurve *>(curves[i]), pos);
        if (!curveInfo.isEmpty())
        {
            if (!info.isEmpty())
                info += "<br>";
            info += curveInfo;
        }
    }

    trackerText.setText(info);
    return trackerText;
}

QString ExChartTracker::curveInfoAt(const QwtPlotTradingCurve *curve, const QPointF &pos) const
{
    const QLineF line = curveLineAt(curve, pos.x());
    if (line.isNull())
        return QString();

    const double y = line.pointAt( (pos.x() - line.p1().x()) / line.dx() ).y();
    QString info( "%1" );

    return info.arg( y );
}

QLineF ExChartTracker::curveLineAt(const QwtPlotTradingCurve* curve, double x ) const
{
    QLineF line;

    if ( curve->dataSize() >= 2 )
    {
        const QRectF br = curve->boundingRect();
        if ( ( br.width() > 0 ) && ( x >= br.left() ) && ( x <= br.right() ) )
        {
            int index = qwtUpperSampleIndex<QwtOHLCSample>(*curve->data(), x, compareX());

            if ( index == -1 && x == curve->sample( curve->dataSize() - 1 ).time )
            {
                // the last sample is excluded from qwtUpperSampleIndex
                index = curve->dataSize() - 1;
            }

            if ( index > 0 )
            {
                line.setP1(QPointF(curve->sample(index - 1).time, curve->sample(index - 1).close));
                line.setP2(QPointF(curve->sample(index).time, curve->sample(index).close));
            }
        }
    }

    return line;
}



} // namespace crex::chart
