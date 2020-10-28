#ifndef EXCHANGEINFO_H
#define EXCHANGEINFO_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>

#include "symbol.h"

class ExchangeInfo : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeInfo(QObject *parent = nullptr);

    void clear();
    void append(const QByteArray &data);
    const QByteArray & data();
    bool parse();
    int size();

    QList<Symbol> symbols;

signals:

private:
    bool read(const QJsonObject &json);
    bool dumpToFile(const QString & filename, const QJsonDocument & doc);

private:
    QByteArray data_;
};

#endif // EXCHANGEINFO_H
