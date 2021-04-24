#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>
#include <QJsonObject>

class Symbol
{
public:
    Symbol();

    void clear();
    bool read(const QJsonObject &json);

public:
    QString symbol;
    QString baseAsset;
    QString quoteAsset;
    int baseAssetPrecision;
    int quotePrecision;
    int pricePrecision;
    int quantityPrecision;
};

#endif // SYMBOL_H
