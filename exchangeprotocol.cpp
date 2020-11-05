#include "exchangeprotocol.h"

#include "logger.h"

ExchangeProtocol::ExchangeProtocol(const QString & name, const QString &baseUrl, const QString &path, QObject *parent)
    : QObject(parent)
    , name_(name)
    , base_url_(baseUrl)
    , path_(path)
{
    TRACE("called");
}

const QString & ExchangeProtocol::name() const
{
    return name_;
}

const QString & ExchangeProtocol::baseUrl() const
{
    return base_url_;
}

const QString & ExchangeProtocol::path() const
{
    return path_;
}

