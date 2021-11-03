#include "datasim.h"

#include "logger.h"

namespace crex::data {

DataSim::DataSim()
{

}

// o = 3i + 20
// c = (-1)(i&1) + o
QVector<crex::data::candle_data> DataSim::getLine(int len, bool randomize, QDateTime start)
{
    QVector<crex::data::candle_data> vector;
    vector.resize(len);
    for (int i = 0; i < len; i++)
    {
        crex::data::candle_data candle;

        candle.o = 3.0 * i + 20.0;
        candle.c = candle.o + 3.0;
        // swap first candle
        if (i == 0)
        {
            candle.h = candle.c;
            candle.c = candle.o;
            candle.o = candle.h;
        }
        candle.h = candle.c  + 7.0;
        candle.l = candle.o - 5.0;
//        candle.c = -1 * (i & 1) + candle.o;
//        candle.h = candle.o > candle.c ? candle.o - 3 : candle.c + 5;
//        candle.l = candle.o > candle.c ? candle.c - 3 : candle.o + 5;

        TRACE("") << candle.o << candle.c << candle.h << candle.l;

        qint64 timeFrame_ = 3600;
        QDateTime now = start.isValid() ? start : QDateTime(QDateTime::currentDateTimeUtc());
        QDateTime dt = now.addSecs(-i * timeFrame_);
        candle.t = dt.toMSecsSinceEpoch();

        vector[i] = candle;
    }

    return vector;
}

QVector<crex::data::candle_data> DataSim::getTriangle(int wavelen, int waves, bool randomize, QDateTime start)
{
    QVector<crex::data::candle_data> vector;

    for (int i = 0; i < waves; i++)
    {
        vector.append(getLine(wavelen, randomize, start));
    }

    return vector;
}

} // namespace crex::data
