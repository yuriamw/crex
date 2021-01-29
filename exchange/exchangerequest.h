#ifndef EXCHANGEREQUEST_H
#define EXCHANGEREQUEST_H

#include <QObject>
#include <QString>

#include <QNetworkReply>


///
/// \brief The ExchangeRequest class
///
/// This class MUST be deleted by consumer of incomming data
/// typicaly in the slot connected to dataReady signal
/// using deleteLater()
///
class ExchangeRequest : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeRequest(QObject *parent = nullptr);
    QNetworkReply *reply() const;
    const QByteArray &data() const;
    void setReply(QNetworkReply *rply);

signals:
    void networkError();
    void dataReady();

private slots:
    void requestFinished();
    void receiveData();

private:
    QNetworkReply *reply_;
    QByteArray data_;
};

#endif // EXCHANGEREQUEST_H
