#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "exchange/exchangerequest.h"
#include "exchangeinfo.h"
#include "exchangeprotocol.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

    ExchangeInfo *exchangeInfo();
    ExchangeProtocol *exchangeProtocol();

private slots:
    void onExchangeProtocolError();

signals:

private:
    ExchangeProtocol *exchange_protocol_;
    ExchangeInfo *exchange_info_;

    ExchangeRequest *exchange_info_request_;
};

#endif // CORE_H
