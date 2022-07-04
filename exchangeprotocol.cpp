#include <QtNetwork>
#include <QUrl>
#include <QString>
#include <QByteArray>

#include <QMessageBox>

#include "exchangeprotocol.h"

#include "logger.h"

ExchangeProtocol::ExchangeProtocol(const QString & name, const QString &baseUrl, const QString &path, QObject *parent)
    : QObject(parent)
    , name_(name)
    , base_url_(baseUrl)
    , path_(path)
{
    initNAM();
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

/////////////////////////////////////////////////////////////////////////////////

ExchangeRequest *ExchangeProtocol::requestExchangeInfo()
{
    ExchangeRequest *rq = new ExchangeRequest(this);

    QNetworkReply *reply = nam_.get(QNetworkRequest(QUrl(QString("%1/%2/%3")
                                         .arg(baseUrl())
                                         .arg(path())
                                         .arg("exchangeInfo"))));
    rq->setReply(reply);

//    requests_.append(rq);
    return rq;

//    connect(reply, &QNetworkReply::finished, this, &ExchangeProtocol::httpFinished);
//    connect(reply, &QIODevice::readyRead, this, &ExchangeProtocol::httpReadyRead);
}

ExchangeRequest *ExchangeProtocol::requestExchangeCandledata(const QString &symbol, const QString &timeFrame, const qlonglong startTime)
{
    ExchangeRequest *rq = new ExchangeRequest(this);

//    klines?symbol=BTCUSDT&interval=1h&startTime=12345678901234
    QString req("%1/%2/%3?symbol=%4&interval=%5");
    if (startTime > 0)
    {
        req += QString("&startTime=%1").arg(startTime);
    }
    QUrl url(req.arg(baseUrl())
             .arg(path())
             .arg("klines")
             .arg(symbol)
             .arg(timeFrame)
            );

    QNetworkReply *reply = nam_.get(QNetworkRequest(url));
    rq->setReply(reply);

    return rq;
}

ExchangeRequest *ExchangeProtocol::requestExchangeDepthOfMarket(const QString & symbol, const int limit)
{
    ExchangeRequest *rq = new ExchangeRequest(this);

//    depth?symbol=BTCUSDT&limit=500
    QNetworkReply *reply = nam_.get(QNetworkRequest(QUrl(QString("%1/%2/%3?symbol=%4&limit=%5")
                                         .arg(baseUrl())
                                         .arg(path())
                                         .arg("depth")
                                         .arg(symbol)
                                         .arg(limit)
                                    )));
    rq->setReply(reply);

//    requests_.append(rq);
    return rq;
}

void ExchangeProtocol::initNAM()
{
    connect(&nam_, &QNetworkAccessManager::authenticationRequired, this, &ExchangeProtocol::slotAuthenticationRequired);
    connect(&nam_, &QNetworkAccessManager::sslErrors, this, &ExchangeProtocol::sslErrors);
}

void ExchangeProtocol::slotAuthenticationRequired(QNetworkReply *reply, QAuthenticator */*authenticator*/)
{
    TRACE("");
    reply->error();
    emit networkError();
}

void ExchangeProtocol::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    TRACE("");
    QString errorString;
    for (const QSslError &error : errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(nullptr, tr("SSL Errors"),
                             tr("SSL error has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
