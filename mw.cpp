#include "mw.h"

#include <QMdiArea>
#include <QMdiSubWindow>
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
#include <QFileDialog>

#include "logger.h"

#include "exchangeinfo.h"
#include "exchangeprotocol.h"
#include "mdichild.h"

#include "widgets/exchart.h"

MW::MW(ExchangeProtocol *exprot, Core *core, ExchangeInfo *exinfo, QWidget *parent)
    : QMainWindow(parent)
    , core_(core)
    , exchange_info_(exinfo)
    , exchange_protocol_(exprot)
    , mdiArea(new QMdiArea(this))
{
    setCentralWidget(mdiArea);

    exchange_date_time_ = new QLabel(tr("no data"));
    statusBar()->addPermanentWidget(exchange_date_time_);

    startExchangeClock();

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
            .arg(exchange_info_->exchangeTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(exchange_info_->exchangeTimeZone().displayName(QTimeZone::GenericTime, QTimeZone::ShortName)));
}

void MW::createMenus()
{
    QMenu *menu;
    QAction *act;

    act = new QAction(tr("Settings"));
    connect(act, &QAction::triggered, this, &MW::onSettings);
    menuBar()->addAction(act);

    act = new QAction(tr("Save symbols"));
    connect(act, &QAction::triggered, this, &MW::onSaveMarket);
    menuBar()->addAction(act);

    act = new QAction(tr("OrderBook"));
    connect(act, &QAction::triggered, this, &MW::onNewOrderBook);
    menuBar()->addAction(act);

    act = new QAction(tr("Chart"));
    connect(act, &QAction::triggered, this, &MW::onNewChart);
    menuBar()->addAction(act);

    windowMenu = menu = menuBar()->addMenu(tr("Window"));
    connect(menu, &QMenu::aboutToShow, this, &MW::onUpdateWindowMenu);
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
    tvMarket->setModel(exchange_info_->model());
}

void MW::createOrderBookWindow(const QString symbol)
{
    if (hasMdiChild(crex::mdichild::MdiDOM, symbol))
        return;

    ExOrderBook *book = new ExOrderBook(exchange_protocol_, exchange_info_, std::move(symbol));

    mdiArea->addSubWindow(book);
    QSize size = book->parentWidget()->size();
    size.setHeight(mdiArea->viewport()->size().height());
    book->parentWidget()->resize(size);
    book->parentWidget()->updateGeometry();
    book->show();
}

void MW::createChartWindow(const QString symbol)
{
    if (hasMdiChild(crex::mdichild::MdiChart, symbol))
        return;

//    ExQChart *chart = new ExQChart(exchange_protocol_, std::move(symbol));
//    mdiArea->addSubWindow(chart);
//    chart->show();

    crex::chart::ExChart *ec = new crex::chart::ExChart(exchange_protocol_, core_, std::move(symbol));
    mdiArea->addSubWindow(ec);
    ec->show();
}

void MW::startExchangeClock()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MW::updateTimeLabel);
    timer->start(500);
}

////////////////////////////////////////////////////////////////////////////////
/// MDI

QWidget *MW::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return activeSubWindow->widget();
    return nullptr;
}

bool MW::hasMdiChild(crex::mdichild::MdiType mditype, const QString & title) const
{
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);

        crex::mdichild::MdiType mt = mdiChildType(mdiSubWindow->widget());

        QWidget *widget = mdiSubWindow->widget();

        if ((widget->windowTitle() == title) && (mt == mditype))
            return true;
    }
    return false;
}

crex::mdichild::MdiType MW::mdiChildType(QWidget *widget) const
{
    ExOrderBook *book = qobject_cast<ExOrderBook *>(widget);
    ExQChart *chart = qobject_cast<ExQChart *>(widget);
    crex::chart::ExChart *ec = qobject_cast<crex::chart::ExChart *>(widget);
    crex::mdichild::MdiType mt = crex::mdichild::MdiInvalid;

    if (chart)
        mt = crex::mdichild::MdiChart;
    if (ec)
        mt = crex::mdichild::MdiChart;
    if (book)
        mt = crex::mdichild::MdiDOM;

    return mt;
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

void MW::onNewChart()
{
    QString symbol;
    QModelIndexList indexes = tvMarket->selectionModel()->selectedIndexes();
    if (indexes.size() > 0)
    {
        QModelIndex selectedIndex = indexes.at(0);
        symbol = tvMarket->model()->data(selectedIndex).toString();
    }
    createChartWindow(symbol);
}

void MW::onNewOrderBook()
{
    QString symbol;
    QModelIndexList indexes = tvMarket->selectionModel()->selectedIndexes();
    if (indexes.size() > 0)
    {
        QModelIndex selectedIndex = indexes.at(0);
        symbol = tvMarket->model()->data(selectedIndex).toString();
    }
    createOrderBookWindow(symbol);
}

void MW::onTvItemActivated(const QModelIndex &index)
{
    const QString symbol = tvMarket->model()->data(index).toString();
    TRACE("") << symbol;

    createChartWindow(symbol);
//    createOrderBookWindow(symbol);
}

void MW::onUpdateWindowMenu()
{
    windowMenu->clear();

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        QWidget *child = mdiSubWindow->widget();

        QString text = child->windowTitle();
        QIcon icon = child->windowIcon();

        QAction *action = windowMenu->addAction(icon, text, mdiSubWindow, [this, mdiSubWindow]() {
                mdiArea->setActiveSubWindow(mdiSubWindow);
            });
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

void MW:: onSaveMarket()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save market symbols to file"), "", tr("Text (*.txt)"));
    exchange_info_->onSaveSymbols(file);
    TRACE("") << file;
}
