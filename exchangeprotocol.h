#ifndef EXCHANGEPROTOCOL_H
#define EXCHANGEPROTOCOL_H

#include <QObject>
#include <QString>

class ExchangeProtocol : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path = QString(), QObject *parent = nullptr);

    const QString & name() const;
    const QString & baseUrl() const;
    const QString & path() const;

signals:

private:
    const QString name_;
    const QString base_url_;
    const QString path_;
};

#endif // EXCHANGEPROTOCOL_H
