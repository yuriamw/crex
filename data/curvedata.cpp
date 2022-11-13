#include "data/curvedata.h"

#include "logger.h"

namespace crex::data {

const CandleData & CurveData::values() const
{
    return candles;
}

CandleData & CurveData::values()
{
    return candles;
}

QwtOHLCSample CurveData::sample( size_t index ) const
{
    return candles.value( index );
}

size_t CurveData::size() const
{
    return candles.size();
}

// It looks needed for drawing optimization but somehow break the XBottom axis (re)scale
//QRectF CurveData::boundingRect() const
//{
//    return candles.boundingRect();
//}

} // namespace crex::data
