#include "datasim.h"

#include <sys/time.h>

#include <vector>
#include <deque>

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

void DataSim::mesureVector(long long int count)
{
    struct timeval tv1, tv2;

    crex::data::candle_data candle = {0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<crex::data::candle_data> vector;

    //////////////////////////////////////////////////////////////////////////////////////////
    TRACE("TEST vector.push_back");
    vector.clear();
    if (gettimeofday(&tv1, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for start");
        return;
    }

    for (long long int i = 0; i < count; i++)
    {
        candle.o++;
        vector.push_back(candle);
    }

    if (gettimeofday(&tv2, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for end");
        return;
    }
    TRACE("DONE vector.push_back for") << count << "START:" << tv1.tv_sec << "END" << tv2.tv_sec << "TIME:" << tv2.tv_sec - tv1.tv_sec;

    //////////////////////////////////////////////////////////////////////////////////////////
    TRACE("TEST vector.erase(begin)");
    if (gettimeofday(&tv1, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for start");
        return;
    }

    for (long long int i = 0; i < count; i++)
    {
        vector.erase(vector.begin());
    }

    if (gettimeofday(&tv2, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for end");
        return;
    }
    TRACE("DONE vector.erase(begin) for") << count << "START:" << tv1.tv_sec << "END" << tv2.tv_sec << "TIME:" << tv2.tv_sec - tv1.tv_sec;

    //////////////////////////////////////////////////////////////////////////////////////////
}

void DataSim::mesureDeque(long long int count)
{
    struct timeval tv1, tv2;

    crex::data::candle_data candle = {0.0, 0.0, 0.0, 0.0, 0.0};
    std::deque<crex::data::candle_data> deque;

    //////////////////////////////////////////////////////////////////////////////////////////
    TRACE("TEST deque.push_back");
    deque.clear();
    if (gettimeofday(&tv1, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for start");
        return;
    }

    for (long long int i = 0; i < count; i++)
    {
        candle.o++;
        deque.push_back(candle);
    }

    if (gettimeofday(&tv2, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for end");
        return;
    }
    TRACE("DONE deque.push_back for") << count << "START:" << tv1.tv_sec << "END" << tv2.tv_sec << "TIME:" << tv2.tv_sec - tv1.tv_sec;

    //////////////////////////////////////////////////////////////////////////////////////////
    TRACE("TEST deque[i]++");
    if (gettimeofday(&tv1, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for start");
        return;
    }

    for (long long int i = 0; i < (count / 2); i++)
    {
        deque[i].o++;
        deque[count - i - 1].o++;
    }

    if (gettimeofday(&tv2, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for end");
        return;
    }
    TRACE("DONE deque[i]++ for") << count << "START:" << tv1.tv_sec << "END" << tv2.tv_sec << "TIME:" << tv2.tv_sec - tv1.tv_sec;

    //////////////////////////////////////////////////////////////////////////////////////////
    TRACE("TEST deque.pop_front");
    if (gettimeofday(&tv1, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for start");
        return;
    }

    for (long long int i = 0; i < count; i++)
    {
        deque.pop_front();
    }

    if (gettimeofday(&tv2, nullptr))
    {
        TRACE("ERROR: gettimeofday failed for end");
        return;
    }
    TRACE("DONE deque.pop_front for") << count << "START:" << tv1.tv_sec << "END" << tv2.tv_sec << "TIME:" << tv2.tv_sec - tv1.tv_sec;

    //////////////////////////////////////////////////////////////////////////////////////////
}

} // namespace crex::data
