#ifndef EXORDERBOOK_H
#define EXORDERBOOK_H

//#include <QWidget>
#include <QTableView>

#include "exchange/exchangerequest.h"
#include "exchangeprotocol.h"

class ExOrderBook : public QTableView
{
    Q_OBJECT
public:
    explicit ExOrderBook(ExchangeProtocol *protocol, const QString symbol, QWidget *parent = nullptr);

public slots:
    void setSymbol(const QString symbol);

signals:

private:
    ExchangeProtocol *protocol_;
    QString symbol_;

};

#endif // EXORDERBOOK_H
