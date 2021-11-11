#include "data.h"

#include <QFile>
#include <QString>
#include <QJsonDocument>

#include "logger.h"

namespace crex::data {

bool dumpToFile(const QString & filename, const QJsonDocument & doc)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        TRACE("") << "IO error:" << file.errorString();
        return false;
    }
    if (file.write(doc.toJson()) < 0)
    {
        TRACE("") << "IO error:" << file.errorString();
        return false;
    }
    file.close();
    return true;
}

} //namespace crex::data
