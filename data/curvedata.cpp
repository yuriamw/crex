#include "data/curvedata.h"

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

} // namespace crex::data
