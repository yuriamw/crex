#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFile>

#include <QDebug>

#include "exchangeinfo.h"

ExchangeInfo::ExchangeInfo(QObject *parent) : QObject(parent)
{

}

void ExchangeInfo::clear()
{
    data_.clear();
}

void ExchangeInfo::append(const QByteArray &data)
{
    data_.append(data);
}

const QByteArray & ExchangeInfo::data()
{
    return data_;
}

int ExchangeInfo::size()
{
    return symbols.size();
}

bool ExchangeInfo::parse()
{
//    qDebug() << data_;

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data_, &jsonError);
    if (doc.isNull())
    {
        qDebug() << "JSON error:" << jsonError.errorString();
        data_.clear();
        return false;
    }

//    dumpToFile("exchange.json", doc);

    return read(doc.object());
}

bool ExchangeInfo::dumpToFile(const QString & filename, const QJsonDocument & doc)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "IO error:" << file.errorString();
        return false;
    }
    if (file.write(doc.toJson()) < 0)
    {
        qDebug() << "IO error:" << file.errorString();
        return false;
    }
    file.close();
    return true;
}

bool ExchangeInfo::read(const QJsonObject &json)
{
    if (json.contains("serverTime"))
    {
        qDebug() << "servertime:" << json["serverTime"];
    }
    if (json.contains("timezone"))
    {
        qDebug() << "timezone:" << json["timezone"];
    }
    if (json.contains("symbols") && json["symbols"].isArray())
    {
        QJsonArray jsonSyms = json["symbols"].toArray();
        symbols.clear();
        symbols.reserve(jsonSyms.size());
        for (int i = 0; i < jsonSyms.size(); ++i)
        {
            QJsonObject jsym = jsonSyms[i].toObject();
            Symbol symbol;
            if (!symbol.read(jsym))
            {
                return false;
            }
            symbols.append(symbol);
        }
    }
    return true;
}
