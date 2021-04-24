#include "symbol.h"

#include <QString>

#include "logger.h"

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
        TRACE("") << "Parse error: no 'symbol'";
        return false;
    }
    if (!json.contains("baseAsset"))
    {
        TRACE("") << "Parse error: no 'baseAsset'";
        return false;
    }
    if (!json.contains("quoteAsset"))
    {
        TRACE("") << "Parse error: no 'quoteAsset'";
        return false;
    }
    if (!json.contains("baseAssetPrecision"))
    {
        TRACE("") << "Parse error: no 'baseAssetPrecision'";
        return false;
    }
    if (!json.contains("quotePrecision"))
    {
        TRACE("") << "Parse error: no 'quotePrecision'";
        return false;
    }
    if (!json.contains("pricePrecision"))
    {
        TRACE("") << "Parse error: no 'pricePrecision'";
        return false;
    }
    symbol = json["symbol"].toString();
    baseAsset = json["baseAsset"].toString();
    quoteAsset = json["quoteAsset"].toString();
    baseAssetPrecision = json["baseAssetPrecision"].toInt();
    quotePrecision = json["quotePrecision"].toInt();
    pricePrecision = json["pricePrecision"].toInt();
    quantityPrecision = json["quantityPrecision"].toInt();

    return true;
}

