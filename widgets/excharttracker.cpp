#include "excharttracker.h"

#include <QwtPickerMachine>
#include <QwtPlot>
#include <QwtPlotTradingCurve>
#include <QwtText>

#include <QPen>
#include <QDateTime>

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
//    setRubberBand(NoRubberBand);

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
    const QwtOHLCSample value = curveLineAt(curve, pos.x());
    if (!value.isValid())
    {
        return QString();
    }

    QString info( "T:%1 O:%2 H:%3 L:%4 C:%5" );

    return info.arg(QDateTime::fromMSecsSinceEpoch(value.time).toString("yyyy-MM-dd hh:mm"))
            .arg(value.open)
            .arg(value.high)
            .arg(value.low)
            .arg(value.close);
}

QwtOHLCSample ExChartTracker::curveLineAt(const QwtPlotTradingCurve* curve, double x ) const
{
    QwtOHLCSample value(0.0, -1.0, 0.0, 0.0, 1.0); // invalid OHLC sample

    if ( curve->dataSize() >= 2 )
    {
//
//        Use boundingRec() for optimization but implementation of boundingRect() in CurveData
//        somehow break the XBottom axis (re)scale
//        if ( ( br.width() > 0 ) && ( x >= br.left() ) && ( x <= br.right() ) )
        {
            int index = qwtUpperSampleIndex<QwtOHLCSample>(*curve->data(), x, compareX());

            if ( index == -1 && x >= curve->sample( curve->dataSize() - 1 ).time )
            {
                // the last sample is excluded from qwtUpperSampleIndex
                index = curve->dataSize() - 1;
            }

            if ( index > 0 )
            {
                value = curve->sample(index);
            }
        }
    }

    return value;
}



} // namespace crex::chart
