#include "mw.h"

#include <QMdiArea>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStringList>

#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>

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
    , mdiArea(new QMdiArea(this))
{
    setCentralWidget(mdiArea);

    exchange_date_time_ = new QLabel(tr("no data"));
    statusBar()->addPermanentWidget(exchange_date_time_);

    startExchange();

    createMenus();
    createMarketView();

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

void MW::createMenus()
{
//    QMenu *menu;
    QAction *act;

    act = new QAction(tr("Settings"));
    connect(act, &QAction::triggered, this, &MW::onSettings);
    menuBar()->addAction(act);

//    menu = menuBar()->addMenu(tr("Connection"));
}

void MW::createMarketView()
{
    QDockWidget *dw = new QDockWidget(tr("Symbols"), this);
    dw->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dw->setAllowedAreas(Qt::LeftDockWidgetArea);
    dw->setFloating(false);
    addDockWidget(Qt::LeftDockWidgetArea, dw);
    tvMarket = new QTreeView();
    tvMarket->setMaximumWidth(160);
    tvMarket->setHeaderHidden(true);
    dw->setWidget(tvMarket);
    connect(tvMarket, &QTreeView::activated, this, &MW::onTvItemActivated);
    tvMarket->setModel(exchangeInfo.model());
}

void MW::createChartWindow(const QString symbol)
{
    ExChart *chart = new ExChart(exchangeProtocol, std::move(symbol));

    mdiArea->addSubWindow(chart);
    chart->show();
}

void MW::startExchange()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MW::updateTimeLabel);
    timer->start(500);

    exchange_info_timer_->setSingleShot(true);
    exchange_info_timer_->setInterval(5000);
    connect(exchange_info_timer_, &QTimer::timeout, this, &MW::requestExchangeInfo);
    exchange_info_timer_->start();

    connect(exchangeProtocol, &ExchangeProtocol::networkError, this, &MW::onExchangeProtocolError);
    connect(exchangeProtocol, &ExchangeProtocol::dataReady, this, &MW::onExchangeProtocolDataReady);

    requestExchangeInfo();
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void MW::onSettings()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(tr("Settings"));

    QVBoxLayout *l = new QVBoxLayout(dialog);

    l->addWidget(new QLabel("Not yet implemented"));

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(bb, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    l->addWidget(bb);

    dialog->open();
    TRACE("after dialog");
}

void MW::onConnect(bool checked)
{
    Q_UNUSED(checked);
    requestExchangeInfo();
}

void MW::onShowChart()
{
}

void MW::requestExchangeInfo()
{
    TRACE("exchangeInfo...");

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
    createChartWindow(tvMarket->model()->data(index).toString());
}

