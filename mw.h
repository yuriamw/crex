#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QString>

#include "exchangeinfo.h"

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

private:
    QTreeWidget *twMarket;
    QNetworkAccessManager nam;
    QNetworkReply *reply;

    ExchangeInfo exchangeInfo;
};
#endif // MW_H
