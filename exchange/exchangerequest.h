#ifndef EXCHANGEREQUEST_H
#define EXCHANGEREQUEST_H

#include <QObject>

class ExchangeRequest : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeRequest(QObject *parent = nullptr);

signals:

};

#endif // EXCHANGEREQUEST_H
