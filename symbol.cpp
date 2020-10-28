#include "symbol.h"

#include <QString>

#include <QDebug>

Symbol::Symbol() :
    symbol("")
    ,baseAsset("")
    ,quoteAsset("")
    ,baseAssetPrecision(0)
    ,quotePrecision(0)
    ,pricePrecision(0)

{

}

bool Symbol::read(const QJsonObject &json)
{
    if (!json.contains("symbol"))
    {
        qDebug() << "Parse error: no 'symbol'";
        return false;
    }
    if (!json.contains("baseAsset"))
    {
        qDebug() << "Parse error: no 'baseAsset'";
        return false;
    }
    if (!json.contains("quoteAsset"))
    {
        qDebug() << "Parse error: no 'quoteAsset'";
        return false;
    }
    if (!json.contains("baseAssetPrecision"))
    {
        qDebug() << "Parse error: no 'baseAssetPrecision'";
        return false;
    }
    if (!json.contains("quotePrecision"))
    {
        qDebug() << "Parse error: no 'quotePrecision'";
        return false;
    }
    if (!json.contains("pricePrecision"))
    {
        qDebug() << "Parse error: no 'pricePrecision'";
        return false;
    }
    symbol = json["symbol"].toString();
    baseAsset = json["baseAsset"].toString();
    quoteAsset = json["quoteAsset"].toString();
    baseAssetPrecision = json["baseAssetPrecision"].toInt();
    quotePrecision = json["quotePrecision"].toInt();
    pricePrecision = json["pricePrecision"].toInt();

    return true;
}

