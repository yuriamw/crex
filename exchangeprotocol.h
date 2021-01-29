#ifndef EXCHANGEPROTOCOL_H
#define EXCHANGEPROTOCOL_H

#include <QObject>
#include <QString>
#include <QList>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>

#include "exchange/exchangerequest.h"

class ExchangeProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path = QString(), QObject *parent = nullptr);

    const QString & name() const;
    const QString & baseUrl() const;
    const QString & path() const;

    const QByteArray & data() const;

    void requestExchangeInfo();
    ExchangeRequest *requestExchangeCandledata(const QString &symbol, const QString &timeFrame);
    void httpFinished();
    void httpReadyRead();
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    void initNAM();

signals:
    void networkError();
    void dataReady();

private:
    const QString name_;
    const QString base_url_;
    const QString path_;

    QList<ExchangeRequest *>requests_;
    QNetworkAccessManager nam;
    QNetworkReply *reply;
    QByteArray data_;
};

#endif // EXCHANGEPROTOCOL_H
