#ifndef EXCHANGEINFO_H
#define EXCHANGEINFO_H

#include <QObject>
#include <QByteArray>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QTimeZone>

#include "symbol.h"
#include "symbolmodel.h"

#include "exchange/exchangerequest.h"
#include "exchangeprotocol.h"

class ExchangeInfo : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeInfo(ExchangeProtocol *protocol, QObject *parent = nullptr);

    void clear();
    bool parseJSON(QByteArray & data);
    bool parseJSON(const QJsonObject &json);
    int size();
    QDateTime exchangeTime();
    qlonglong exchangeTimeStamp();
    QTimeZone exchangeTimeZone();
    const Symbol getSymbol(const QString & symbol) const;

    QList<Symbol> symbols;
    QAbstractItemModel *model();

signals:

public slots:
    void onSaveSymbols(const QString & file);

private slots:
    void onTimer();
    void onExchangeInfoDataReady();

private:
    void quoteLongInt(QByteArray & data, const QString & key);
    bool dumpToFile(const QString & filename, const QJsonDocument & doc);
    void setExchangeTime(const qlonglong timestamp);
    void setTimeZone(const QString &timezone);

private:
    ExchangeProtocol *exchange_protocol_;
    ExchangeRequest *request_;
    QTimer *timer_;

    SymbolModel *model_;
    qlonglong exchange_time_;
    QString exchange_timezone_;
};

#endif // EXCHANGEINFO_H
