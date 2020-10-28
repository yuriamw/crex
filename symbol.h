#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include <QJsonObject>

class Symbol
{
public:
    Symbol();

    bool read(const QJsonObject &json);

public:
    QString symbol;
    QString baseAsset;
    QString quoteAsset;
    int baseAssetPrecision;
    int quotePrecision;
    int pricePrecision;

};

#endif // SYMBOL_H
