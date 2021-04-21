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
//    TRACE("called");

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

const QByteArray & ExchangeProtocol::data() const
{
    return data_;
}

/////////////////////////////////////////////////////////////////////////////////

void ExchangeProtocol::requestExchangeInfo()
{
    data_.clear();

    reply = nam.get(QNetworkRequest(QUrl(QString("%1/%2/%3")
                                         .arg(baseUrl())
                                         .arg(path())
                                         .arg("exchangeInfo"))));
    connect(reply, &QNetworkReply::finished, this, &ExchangeProtocol::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &ExchangeProtocol::httpReadyRead);
}

ExchangeRequest *ExchangeProtocol::requestExchangeCandledata(const QString &symbol, const QString &timeFrame)
{
    ExchangeRequest *rq = new ExchangeRequest(this);

//    klines?symbol=BTCUSDT&interval=1h
    reply = nam.get(QNetworkRequest(QUrl(QString("%1/%2/%3?symbol=%4&interval=%5")
                                         .arg(baseUrl())
                                         .arg(path())
                                         .arg("klines")
                                         .arg(symbol)
                                         .arg(timeFrame)
                                    )));
    rq->setReply(reply);

//    requests_.append(rq);
    return rq;
}

ExchangeRequest *ExchangeProtocol::requestExchangeDepthOfMarket(const QString & symbol, const int limit)
{
    ExchangeRequest *rq = new ExchangeRequest(this);

//    depth?symbol=BTCUSDT&limit=500
    reply = nam.get(QNetworkRequest(QUrl(QString("%1/%2/%3?symbol=%4&limit=%5")
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
    connect(&nam, &QNetworkAccessManager::authenticationRequired, this, &ExchangeProtocol::slotAuthenticationRequired);
    connect(&nam, &QNetworkAccessManager::sslErrors, this, &ExchangeProtocol::sslErrors);
}

void ExchangeProtocol::httpFinished()
{
//    TRACE("");
//    QFileInfo fi;
//    if (file) {
//        fi.setFile(file->fileName());
//        file->close();
//        file.reset();
//    }

//    if (httpRequestAborted) {
//        reply->deleteLater();
//        reply = nullptr;
//        return;
//    }

    if (reply->error()) {
        TRACE("") << "HTTP error";
        reply->deleteLater();
        reply = nullptr;
        data_.clear();
        emit networkError();
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = nullptr;

    if (!redirectionTarget.isNull()) {
        QUrl url;
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
//        if (QMessageBox::question(this, tr("Redirect"),
//                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
//                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
//            QFile::remove(fi.absoluteFilePath());
//            downloadButton->setEnabled(true);
//            statusLabel->setText(tr("Download failed:\nRedirect rejected."));
//            return;
//        }
//        file = openFileForWrite(fi.absoluteFilePath());
//        if (!file) {
//            downloadButton->setEnabled(true);
//            return;
//        }
//        startRequest(redirectedUrl);
        TRACE("") << "!!!Redirected to:" << redirectedUrl;
        return;
    }

//    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
//                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
//    if (launchCheckBox->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
//    downloadButton->setEnabled(true);

//    {
//        static int q = 0;
//        QFile file(QString("exchangeinfo-%1.json").arg(q++));
//        if (!file.open(QIODevice::WriteOnly))
//        {
//            TRACE("") << "IO error:" << file.errorString();
//        }
//        if (file.write(data_) < 0)
//        {
//            TRACE("") << "IO error:" << file.errorString();
//        }
//        file.close();
//    }
    emit dataReady();
}

void ExchangeProtocol::httpReadyRead()
{
//    TRACE("");
    data_.append(reply->readAll());
}

void ExchangeProtocol::slotAuthenticationRequired(QNetworkReply */*reply*/, QAuthenticator */*authenticator*/)
{
//    TRACE("");
    data_.clear();
    emit networkError();
}

void ExchangeProtocol::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
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
    data_.clear();
    emit networkError();
}
