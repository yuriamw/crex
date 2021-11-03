#ifndef DATA_H
#define DATA_H

#include <QtGlobal>

namespace crex::data {

struct candle_data {
    qreal l;
    qreal o;
    qreal c;
    qreal h;
    qreal t;
};

} // namespace crex::data

#endif // DATA_H
