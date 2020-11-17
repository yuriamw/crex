#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStatusBar>
#include <QLabel>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QAction>

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

    void requestExchangeInfo();

    void updateTimeLabel();

private slots:
    void onExchangeProtocolError();
    void onExchangeProtocolDataReady();

    void onTvItemActivated(const QModelIndex &index);

private:
    QAction *connectAction;
    QLabel *exchange_date_time_;
    QTreeView *tvMarket;

    ExchangeInfo exchangeInfo;
    ExchangeProtocol *exchangeProtocol;

    QTimer *exchange_info_timer_;
};
#endif // MW_H
