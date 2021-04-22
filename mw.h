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
    MW(ExchangeProtocol *exprot, ExchangeInfo *exinfo, QWidget *parent = nullptr);
    ~MW();

private:
    void createMenus();
    void createMarketView();

    void createOrderBookWindow(const QString symbol);
    void createChartWindow(const QString symbol);

    void startExchangeClock();

    void updateTimeLabel();

    QWidget *activeMdiChild() const;
    bool hasMdiChild(crex::mdichild::MdiType mditype, const QString & title) const;
    crex::mdichild::MdiType mdiChildType(QWidget *widget) const;

private slots:
    void onSettings();
    void onNewChart();
    void onNewOrderBook();

    void onTvItemActivated(const QModelIndex &index);

    void onUpdateWindowMenu();

private:
    QLabel *exchange_date_time_;
    QTreeView *tvMarket;

    ExchangeInfo *exchange_info_;
    ExchangeProtocol *exchange_protocol_;

    QMdiArea *mdiArea;
    QMenu *windowMenu;
};
#endif // MW_H
