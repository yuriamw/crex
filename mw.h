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
#include "exchart.h"

class MW : public QMainWindow
{
    Q_OBJECT

public:
    MW(QWidget *parent = nullptr);
    ~MW();

private:
    void onConnect(bool checked);
    void onShowChart();

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
    ExChart *chart_;

    QTimer *exchange_info_timer_;
};
#endif // MW_H
