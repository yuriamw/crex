#include "core.h"

#include <QTimer>

#include "exchange/exchangerequest.h"
#include "exchange/exwssprotocol.h"
#include "exchange/exwssrequest.h"

#include "exchangeinfo.h"
#include "exchangeprotocol.h"

#include "logger.h"

Core::Core(QObject *parent)
    : QObject(parent)
    , exchange_protocol_(new ExchangeProtocol("Binance FUTURES", "https://fapi.binance.com", "fapi/v1", this))
    , exchange_wss_protocol_(new ExWssProtocol("Binance FUTURES", "wss://fstream.binance.com/ws", "", this))
    , exchange_info_(new ExchangeInfo(exchange_protocol_, this))
    , exchange_info_request_(nullptr)
{
    connect(exchange_protocol_, &ExchangeProtocol::networkError, this, &Core::onExchangeProtocolError);
}

ExchangeInfo *Core::exchangeInfo()
{
    return exchange_info_;
}

ExchangeProtocol *Core::exchangeProtocol()
{
    return exchange_protocol_;
}

ExWssProtocol *Core::exchangeWssProtocol()
{
    return new ExWssProtocol("Binance FUTURES", "wss://fstream.binance.com/ws", "", this);
//    return exchange_wss_protocol_;
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void Core::onExchangeProtocolError()
{
    TRACE("");
}

