#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QTreeView>
#include <QStatusBar>
#include <QLabel>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QAction>
#include <QMenu>

#include "exchangeinfo.h"
#include "exchangeprotocol.h"
#include "exorderbook.h"
#include "exchart.h"
#include "mdichild.h"

class MW : public QMainWindow
{
    Q_OBJECT

public:
    MW(QWidget *parent = nullptr);
    ~MW();

private:
    void createMenus();
    void createMarketView();

    void createOrderBookWindow(const QString symbol);
    void createChartWindow(const QString symbol);

    void startExchange();

    void onConnect(bool checked);
    void onShowChart();

    void requestExchangeInfo();

    void updateTimeLabel();

    QWidget *activeMdiChild() const;
    bool hasMdiChild(crex::mdichild::MdiType mditype, const QString & title) const;
    crex::mdichild::MdiType mdiChildType(QWidget *widget) const;

private slots:
    void onSettings();
    void onNewChart();
    void onNewOrderBook();

    void onExchangeProtocolError();
    void onExchangeProtocolDataReady();

    void onTvItemActivated(const QModelIndex &index);

    void onUpdateWindowMenu();

private:
    QAction *connectAction;
    QLabel *exchange_date_time_;
    QTreeView *tvMarket;

    ExchangeInfo exchangeInfo;
    ExchangeProtocol *exchangeProtocol;

    QTimer *exchange_info_timer_;

    QMdiArea *mdiArea;
    QMenu *windowMenu;
};
#endif // MW_H
