#pragma once

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
