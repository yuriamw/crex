#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>

#include "exchangeinfo.h"
#include "exchangeprotocol.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = nullptr);

    ExchangeInfo *exchangeInfo();
    ExchangeProtocol *exchangeProtocol();

private:
    void requestExchangeInfo();

private slots:
    void onExchangeProtocolError();
    void onExchangeProtocolDataReady();

signals:

private:
    ExchangeInfo *exchange_info_;
    ExchangeProtocol *exchange_protocol_;
    QTimer *exchange_info_timer_;
};

#endif // CORE_H
