#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStatusBar>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QString>
#include <QByteArray>

#include "exchangeinfo.h"
#include "exchangeprotocol.h"

class MW : public QMainWindow
{
    Q_OBJECT

public:
    MW(QWidget *parent = nullptr);
    ~MW();

private:
    void onConnect(bool checked);

    void initNAM();

    void httpFinished();
    void httpReadyRead();
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    void updateTimeLabel();

private:
    QLabel *exchange_date_time_;
    QTreeView *tvMarket;
    QNetworkAccessManager nam;
    QNetworkReply *reply;
    QByteArray data_;

    ExchangeInfo exchangeInfo;
    ExchangeProtocol *exchangeProtocol;
};
#endif // MW_H
