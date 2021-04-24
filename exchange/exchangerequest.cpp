#include "exchangerequest.h"

#include "logger.h"

ExchangeRequest::ExchangeRequest(QObject *parent)
    : QObject(parent)
    , reply_(nullptr)
{
    data_.clear();
}

QNetworkReply *ExchangeRequest::reply() const
{
    return reply_;
}

const QByteArray & ExchangeRequest::data() const
{
    return data_;
}

void ExchangeRequest::setReply(QNetworkReply *rply)
{
    reply_ = rply;
    connect(reply_, &QNetworkReply::finished, this, &ExchangeRequest::requestFinished);
    connect(reply_, &QIODevice::readyRead, this, &ExchangeRequest::receiveData);
    connect(reply_, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &ExchangeRequest::requestError);
}

void ExchangeRequest::receiveData()
{
    data_.append(reply_->readAll());
}

void ExchangeRequest::requestFinished()
{
//    TRACE("");

    if (reply_->error()) {
        TRACE("") << "HTTP error";
        reply_->deleteLater();
        reply_ = nullptr;
        data_.clear();
        emit networkError();
        return;
    }

    const QVariant redirectionTarget = reply_->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply_->deleteLater();
    reply_ = nullptr;

    if (!redirectionTarget.isNull()) {
        QUrl url;
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
        TRACE("") << "!!!Redirected to:" << redirectedUrl;
        return;
    }
    emit dataReady();
}

void ExchangeRequest::requestError(QNetworkReply::NetworkError code)
{
    TRACE("") << code;
}
