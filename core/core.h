#pragma once
#include <QObject>

#include "exchange/exwssprotocol.h"
#include "exchangeinfo.h"
#include "exchangeprotocol.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

    ExchangeInfo *exchangeInfo();
    ExchangeProtocol *exchangeProtocol();
    ExWssProtocol *exchangeWssProtocol();

private slots:
    void onExchangeProtocolError();

signals:

private:
    ExchangeProtocol *exchange_protocol_;
    ExWssProtocol *exchange_wss_protocol_;
    ExchangeInfo *exchange_info_;

    ExchangeRequest *exchange_info_request_;
};
