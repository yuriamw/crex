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

#include <core/core.h>

#include "exchangeinfo.h"
#include "exchangeprotocol.h"
#include "exchange/exwssprotocol.h"
#include "exorderbook.h"
#include "exqchart.h"
#include "mdichild.h"

class MW : public QMainWindow
{
    Q_OBJECT

public:
    MW(ExchangeProtocol *exprot, Core *core, ExchangeInfo *exinfo, QWidget *parent = nullptr);
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

    void onSaveMarket();

private:
    QLabel *exchange_date_time_;
    QTreeView *tvMarket;

    Core *core_;
    ExchangeInfo *exchange_info_;
    ExchangeProtocol *exchange_protocol_;
//    ExWssProtocol *exchange_wss_protocol_;

    QMdiArea *mdiArea;
    QMenu *windowMenu;
};
#endif // MW_H
