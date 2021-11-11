#ifndef DATA_H
#define DATA_H

#include <QtGlobal>
#include <QString>
#include <QJsonDocument>

namespace crex::data {

struct candle_data {
    qreal l;
    qreal o;
    qreal c;
    qreal h;
    qreal t;
};

bool dumpToFile(const QString & filename, const QJsonDocument & doc);

} // namespace crex::data

#endif // DATA_H
