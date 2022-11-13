#pragma once

#include <QVector>
#include <QDateTime>

#include <data/data.h>

namespace crex::data {

class DataSim
{
public:
    DataSim();

    static QVector<crex::data::candle_data> getLine(int len = 32, bool randomize = false, QDateTime start = QDateTime());
    static QVector<crex::data::candle_data> getTriangle(int wavelen = 32, int waves = 1, bool randomize = false, QDateTime start = QDateTime());

    static void mesureVector(long long int count = 1000000);
    static void mesureDeque(long long int count = 1000000);
};

} // namespace crex::data
