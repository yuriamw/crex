#pragma once

#include <QwtSeriesData>
#include <QwtOHLCSample>

#include <QRectF>

#include "data/candledata.h"

namespace crex::data {

class CurveData : public QwtSeriesData< QwtOHLCSample >
{
public:
    const CandleData & values() const;

    CandleData & values();
    virtual QwtOHLCSample sample( size_t index ) const QWT_OVERRIDE;
    virtual size_t size() const QWT_OVERRIDE;
// It looks needed for drawing optimization but somehow break the XBottom axis (re)scale
//    virtual QRectF boundingRect() const QWT_OVERRIDE;

private:
    CandleData candles;
};

} // namespace crex::data

