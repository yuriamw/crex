#ifndef EXCHANGEINFO_H
#define EXCHANGEINFO_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QTimeZone>

#include "symbol.h"
#include "symbolmodel.h"

class ExchangeInfo : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeInfo(QObject *parent = nullptr);

    void clear();
    bool parse(QByteArray & data);
    bool parse(const QJsonObject &json);
    int size();
    QDateTime exchangeTime();
    qlonglong exchangeTimeStamp();
    QTimeZone exchangeTimeZone();

    QList<Symbol> symbols;
    QAbstractItemModel *model();

signals:

private:
    void quoteLongInt(QByteArray & data, const QString & key);
    bool dumpToFile(const QString & filename, const QJsonDocument & doc);
    void setExchangeTime(const qlonglong timestamp);
    void setTimeZone(const QString &timezone);

private:
    SymbolModel *model_;
    qlonglong exchange_time_;
    QString exchange_timezone_;
};

#endif // EXCHANGEINFO_H
