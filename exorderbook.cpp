#include <QWidget>
#include <QTableView>
#include <QLayout>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>

#include "logger.h"
#include "exorderbook.h"
#include "mdichild.h"

using namespace orderbook;

////////////////////////////////////////////////////////////////////////////////
/// OrderBookModel

ExOrderBookModel::ExOrderBookModel(QObject *parent)
    : QAbstractTableModel(parent)
    , price_precision_(0)
    , quantity_precision_(0)
{
    asks_.clear();
    bids_.clear();
}

void ExOrderBookModel::setPrecision(int pricePrecision, int quantityPrecision)
{
    price_precision_ = pricePrecision;
    quantity_precision_ = quantityPrecision;
}

bool ExOrderBookModel::isAsk(int row) const
{
    return row < asks_.count();
}

bool ExOrderBookModel::isBid(int row) const
{
    return row >= asks_.count();
}

QList<struct orderbook::order>::iterator ExOrderBookModel::askOrBid(int row)
{
    QList<struct orderbook::order>::iterator it;
    if (isAsk(row))
    {
        it = asks_.begin();
        it += asks_.count() - row - 1;
    }
    else
    {
        it = bids_.begin();
        it += row - asks_.count();
    }
    return it;
}

QList<struct orderbook::order>::const_iterator ExOrderBookModel::cAskOrBid(int row) const
{
    QList<struct orderbook::order>::const_iterator it;
    if (isAsk(row))
    {
        it = asks_.cbegin();
        it += asks_.count() - row - 1;
    }
    else
    {
        it = bids_.cbegin();
        it += row - asks_.count();
    }
    return it;
}

int ExOrderBookModel::rowCount(const QModelIndex &/*parent*/) const
{
    return asks_.count() + bids_.count();
}

int ExOrderBookModel::columnCount(const QModelIndex &/*parent*/) const
{
    return MAX_COLUMN;
}

QVariant ExOrderBookModel::data(const QModelIndex &index, int role) const
{
//    TRACE("") << index;
    if ( !( (role == Qt::DisplayRole) || (role == Qt::EditRole) || (role == Qt::TextColorRole) ) )
        return QVariant();
    if (!index.isValid())
        return QVariant();
    if (index.column() > MAX_COLUMN)
    {
        TRACE("") << "ERROR column" << index.column();
        return QVariant();
    }

    if (role == Qt::TextColorRole)
    {
        if (isAsk(index.row()))
            return QColor(255, 51, 51);
        else
            return QColor(0, 153, 0);
    }
    else
    {
        QList<struct orderbook::order>::const_iterator it = cAskOrBid(index.row());

        switch (index.column()) {
            case PRICE_COL:
                return QString::number(it->price, 'f', price_precision_);
            case QTY_COL:
                return QString::number(it->qty, 'f', quantity_precision_);
        }
    }
    TRACE("") << "ERROR God sake here we are!!!";
    return QVariant();
}

bool ExOrderBookModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() >= MAX_COLUMN)
        {
            TRACE("") << "ERROR column" << index.column();
            return false;
        }
        int r = index.row();
        QList<struct orderbook::order>::iterator it = askOrBid(r);

        switch (index.column()) {
            case PRICE_COL:
                it->price = value.toDouble();
            break;
            case QTY_COL:
                it->qty = value.toDouble();
            break;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void ExOrderBookModel::setOrderBook(QList<orderbook::order> &asks, QList<orderbook::order> &bids)
{
    beginRemoveRows(QModelIndex(), 0, asks_.count() - 1);
    asks_.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, asks.count() - 1);
    asks_.append(asks);
    endInsertRows();

    beginRemoveRows(QModelIndex(), asks_.count(), asks_.count() + bids_.count() - 1);
    bids_.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), asks_.count(), asks_.count() + bids.count() - 1);
    bids_.append(bids);
    endInsertRows();
}

QVariant ExOrderBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
//    TRACE("") << section << role;
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (section >= MAX_COLUMN)
    {
        TRACE("") << "ERROR column" << section;
        return QVariant();
    }

    switch (section) {
    case PRICE_COL:
        return "Price";
    case QTY_COL:
        return "Qty";
    }
    TRACE("") << "ERROR God sake here we are!!!";
    return QVariant();
}

Qt::ItemFlags ExOrderBookModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

////////////////////////////////////////////////////////////////////////////////
/// OrderBook

ExOrderBook::ExOrderBook(ExchangeProtocol *protocol, ExchangeInfo *exinfo, const QString symbol, QWidget *parent)
    : QWidget(parent)
    , exchange_symbol_(new QComboBox())
    , orderbook_(new QTableView(this))
    , request_(nullptr)
    , protocol_(protocol)
    , exchange_info_(exinfo)
    , timer_(new QTimer(this))
    , model_(new ExOrderBookModel(this))

{
    setWindowIcon(QIcon::fromTheme("mail-task"));

    QVBoxLayout *lv = new QVBoxLayout(this);
    exchange_symbol_->setModel(exchange_info_->model());
    connect(exchange_symbol_, &QComboBox::currentTextChanged, this, &ExOrderBook::setSymbolName);
    lv->addWidget(exchange_symbol_);

    lv->addWidget(orderbook_);
    orderbook_->setModel(model_);

    int idx = exchange_symbol_->findText(symbol, Qt::MatchExactly);
    if (idx >= 0)
    {
        // signal is not emited if (idx == exchange_symbol_->currentIndex())
        // handle it manualy
        if (idx == exchange_symbol_->currentIndex())
            setSymbolName(symbol);
        else
            exchange_symbol_->setCurrentIndex(idx);
    }
    else
    {
        exchange_symbol_->setCurrentIndex(-1);
    }

    createMenus(lv);

    timer_->setSingleShot(false);
    timer_->setInterval(550);
    connect(timer_, &QTimer::timeout, this, &ExOrderBook::onTimer);
    timer_->start();
}

void ExOrderBook::createMenus(QLayout *layout)
{
    QMenuBar *menuBar = new QMenuBar(this);
    layout->addWidget(menuBar);

    QMenu *menu = menuBar->addMenu("Actions");

    QAction *act = new QAction("First");
    menu->addAction(act);
    act = new QAction("Second");
    menu->addAction(act);
    act = new QAction("Third");
    menu->addAction(act);

    menu = menuBar->addMenu("Orders");
    act = new QAction("Buy");
    menu->addAction(act);
    act = new QAction("Sell");
    menu->addAction(act);
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void ExOrderBook::setSymbolName(const QString symbol)
{
    setSymbol(exchange_info_->getSymbol(symbol));

}

void ExOrderBook::setSymbol(const Symbol symbol)
{
    symbol_.clear();
    symbol_ = std::move(symbol);
    model_->setPrecision(symbol.pricePrecision, symbol.quantityPrecision);

    if (symbol_.symbol.isEmpty())
        setWindowTitle("?");
    else
        setWindowTitle(symbol_.symbol);
}

void ExOrderBook::onTimer()
{

    if (request_)
        return;

    if (symbol_.symbol.isEmpty())
    {
        TRACE("empty");
        return;
    }

    request_ = protocol_->requestExchangeDepthOfMarket(symbol_.symbol, 5);
    connect(request_, &ExchangeRequest::dataReady, this, &ExOrderBook::onDepthOfMarketDataReady);
}

void ExOrderBook::onDepthOfMarketDataReady()
{
    if (request_)
    {
        QByteArray json_data(request_->data());
        request_->deleteLater();
        request_ = nullptr;

        parseJSON(json_data);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// JSON Parser

bool ExOrderBook::dumpToFile(const QString & filename, const QJsonDocument & doc)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        TRACE("") << "IO error:" << file.errorString();
        return false;
    }
    if (file.write(doc.toJson()) < 0)
    {
        TRACE("") << "IO error:" << file.errorString();
        return false;
    }
    file.close();
    return true;
}

void ExOrderBook::parseJSON(QByteArray &json_data)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(json_data, &jsonError);
    if (doc.isNull())
    {
        TRACE("") << "JSON error:" << jsonError.errorString();
        return;
    }
    QJsonObject json = doc.object();

//    static int q = 0;
//    dumpToFile(QString("../orderbook-dump-%1.json").arg(q++), doc);

    /*
{
  "lastUpdateId": 1027024,
  "E": 1589436922972,   // Message output time
  "T": 1589436922959,   // Transaction time
  "bids": [
    [
      "4.00000000",     // PRICE
      "431.00000000"    // QTY
    ]
  ],
  "asks": [
    [
      "4.00000200",
      "12.00000000"
    ]
  ]
}
    */
    QList<orderbook::order> asks;
    if (json.contains("asks") && json["asks"].isArray())
    {
        QJsonArray jsonAsks = json["asks"].toArray();
        parseJSON(jsonAsks, asks);
    }

    QList<orderbook::order> bids;
    if (json.contains("bids") && json["bids"].isArray())
    {
        QJsonArray jsonBids = json["bids"].toArray();
        parseJSON(jsonBids, bids);
    }

    model_->setOrderBook(asks, bids);
}

void ExOrderBook::parseJSON(const QJsonArray &json, QList<orderbook::order> & orders)
{
    for (int i = 0; i < json.size(); ++i)
    {
        orders.append(std::move(parseJSONOrder(json[i].toArray())));
    }
}

orderbook::order ExOrderBook::parseJSONOrder(const QJsonArray &json)
{
    orderbook::order o;
    o.price = json[0].toString().toDouble();
    o.qty   = json[1].toString().toDouble();
    return o;
}

