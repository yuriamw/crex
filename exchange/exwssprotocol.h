#ifndef EXWSSPROTOCOL_H
#define EXWSSPROTOCOL_H

#include <QObject>
#include <QList>

#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>

#include "exchange/exwssrequest.h"

class ExWssProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ExWssProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path = QString(), QObject *parent = nullptr);

    ExWssRequest *requestExchangeCandledata(const QString &symbol, const QString &timeFrame, const qlonglong startTime = 0);

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onSslErrors(const QList<QSslError> &errors);
    void onError(QAbstractSocket::SocketError error);
    void wssDisconnected();

signals:

private:
    QString name_;
    QString base_;
    QString path_;

    QWebSocket web_socket_;
};

#endif // EXWSSPROTOCOL_H
