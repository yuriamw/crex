#include "core.h"

#include <QTimer>

#include "exchangeinfo.h"
#include "exchangeprotocol.h"
#include "logger.h"

Core::Core(QObject *parent)
    : QObject(parent)
    , exchange_info_(new ExchangeInfo(this))
    , exchange_protocol_(new ExchangeProtocol("Binance FUTURES", "https://fapi.binance.com", "fapi/v1", this))
    , exchange_info_timer_(new QTimer(this))
{
    exchange_info_timer_->setSingleShot(true);
    exchange_info_timer_->setInterval(5000);
    connect(exchange_info_timer_, &QTimer::timeout, this, &Core::requestExchangeInfo);
    exchange_info_timer_->start();

    connect(exchange_protocol_, &ExchangeProtocol::networkError, this, &Core::onExchangeProtocolError);
    connect(exchange_protocol_, &ExchangeProtocol::dataReady, this, &Core::onExchangeProtocolDataReady);

    requestExchangeInfo();
}

ExchangeInfo *Core::exchangeInfo()
{
    return exchange_info_;
}

ExchangeProtocol *Core::exchangeProtocol()
{
    return exchange_protocol_;
}

void Core::requestExchangeInfo()
{
//    TRACE("");

    exchange_protocol_->requestExchangeInfo();
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void Core::onExchangeProtocolError()
{
    exchange_info_timer_->stop();
    exchange_info_->clear();
    exchange_info_timer_->start();
}

void Core::onExchangeProtocolDataReady()
{
    QByteArray data(exchange_protocol_->data());
    exchange_info_->parse(data);

    exchange_info_timer_->start();
}

