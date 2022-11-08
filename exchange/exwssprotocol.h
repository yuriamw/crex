#ifndef EXWSSPROTOCOL_H
#define EXWSSPROTOCOL_H

#include <QObject>
#include <QList>
#include <QQueue>
#include <QJsonObject>

#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>

#include "exchange/exwssrequest.h"

class ExWssProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ExWssProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path = QString(), QObject *parent = nullptr);

    ExWssRequest *requestExchangeCandledata(const QString &symbol, const QString &timeFrame);
    void unsubscribe();
    QJsonObject popData();

private Q_SLOTS:
    void onTextMessageReceived(QString message);
    void onSslErrors(const QList<QSslError> &errors);
    void onError(QAbstractSocket::SocketError error);
    void wssConnected();
    void wssDisconnected();

signals:
    void dataReady();

private:
    int id;
    QString symbol_;
    QString time_frame_;

    QString name_;
    QString base_;
    QString path_;

    QWebSocket web_socket_;

    QQueue<QJsonObject> data_;
};

#endif // EXWSSPROTOCOL_H
