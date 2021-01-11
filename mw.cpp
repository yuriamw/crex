#include "mw.h"

#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStringList>

#include <QTreeView>
#include <QWidget>
#include <QFrame>
#include <QLayout>

#include <QTimer>

#include <QMessageBox>

#include "logger.h"

#include "exchangeprotocol.h"

MW::MW(QWidget *parent)
    : QMainWindow(parent)
    , connectAction(nullptr)
    , exchangeProtocol(new ExchangeProtocol("Binance FUTURES", "https://fapi.binance.com", "fapi/v1", this))
    , exchange_info_timer_(new QTimer(this))
    , chart_(new ExChart())
{
    QMenu *menu;
    QAction *act;
    QToolBar *toolBar;

    QWidget *w = new QWidget();
    QHBoxLayout *lh = new QHBoxLayout();
    w->setLayout(lh);

    tvMarket = new QTreeView();
    lh->addWidget(tvMarket);
    connect(tvMarket, &QTreeView::activated, this, &MW::onTvItemActivated);
    tvMarket->setModel(exchangeInfo.model());

//    QFrame *p = new QFrame();
//    p->setMinimumWidth(300);
//    p->setFrameStyle(QFrame::Panel | QFrame::Raised);
//    lh->addWidget(p);

    lh->addWidget(chart_);

    setCentralWidget(w);

    menu = menuBar()->addMenu(tr("Connection"));

    act = connectAction = new QAction(QIcon::fromTheme("network-wired"), tr("&Connect"));
    act->setCheckable(true);
    connect(act, &QAction::triggered, this, &MW::onConnect);
    toolBar = addToolBar(tr("Connection"));
    toolBar->addAction(act);
    menu->addAction(act);

    act = connectAction = new QAction(QIcon::fromTheme("applications-graphics"), tr("Char&t"));
    act->setCheckable(false);
    connect(act, &QAction::triggered, this, &MW::onShowChart);
    toolBar = addToolBar(tr("Chart"));
    toolBar->addAction(act);
    menu->addAction(act);

    exchange_date_time_ = new QLabel(tr("no data"));
    statusBar()->addPermanentWidget(exchange_date_time_);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MW::updateTimeLabel);
    timer->start(500);

    exchange_info_timer_->setSingleShot(true);
    exchange_info_timer_->setInterval(500);
    connect(exchange_info_timer_, &QTimer::timeout, this, &MW::requestExchangeInfo);

    connect(exchangeProtocol, &ExchangeProtocol::networkError, this, &MW::onExchangeProtocolError);
    connect(exchangeProtocol, &ExchangeProtocol::dataReady, this, &MW::onExchangeProtocolDataReady);

    statusBar()->showMessage(tr("Ready"));
}

MW::~MW()
{
}

void MW::updateTimeLabel()
{
    exchange_date_time_->setText(QString("%1 %2")
            .arg(exchangeInfo.exchangeTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(exchangeInfo.exchangeTimeZone().displayName(QTimeZone::GenericTime, QTimeZone::ShortName)));
}

void MW::onConnect(bool checked)
{
    if (checked)
    {
        TRACE("") << "Connect to server";
        requestExchangeInfo();
    }
    else
    {
        TRACE("") << "Disconnect from server";
        exchange_info_timer_->stop();
    }
}

void MW::onShowChart()
{
}

void MW::requestExchangeInfo()
{
    TRACE("exchangeInfo...");

    if (!connectAction->isChecked())
    {
        TRACE("spuriouse");
        exchange_info_timer_->stop();
        return;
    }
    exchangeProtocol->requestExchangeInfo();
}

void MW::onExchangeProtocolError()
{
    connectAction->setChecked(false);
    exchange_info_timer_->stop();
    exchangeInfo.clear();
}

void MW::onExchangeProtocolDataReady()
{
    QByteArray data(exchangeProtocol->data());
    exchangeInfo.parse(data);

    exchange_info_timer_->start();
}

void MW::onTvItemActivated(const QModelIndex &index)
{
    TRACE("") << tvMarket->model()->data(index).toString();
}

