#include <QWidget>
#include <QTableView>
#include <QLayout>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>

#include "logger.h"
#include "exorderbook.h"
#include "mdichild.h"

using namespace orderbook;

////////////////////////////////////////////////////////////////////////////////
/// OrderBookModel

ExOrderBookModel::ExOrderBookModel(QObject *parent) : QAbstractTableModel(parent)
{
    asks_.clear();
    bids_.clear();
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

        struct order o = *it;
        switch (index.column()) {
            case PRICE_COL:
                return o.price;
            case QTY_COL:
                return o.qty;
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

ExOrderBook::ExOrderBook(ExchangeProtocol *protocol, const QString symbol, QWidget *parent)
    : QWidget(parent)
//    , crex::mdichild::MdiChild(crex::mdichild::MdiDOM, parent)
    , orderbook_(new QTableView(this))
    , request_(nullptr)
    , protocol_(protocol)
    , model_(new ExOrderBookModel(this))

{
    QVBoxLayout *lv = new QVBoxLayout(this);
    lv->addWidget(orderbook_);
    orderbook_->setModel(model_);

    if (symbol.isEmpty())
    {
        QString s = "";
        s += "{\n";
        s += "  \"lastUpdateId\": 1027024,\n";
        s += "  \"E\": 1589436922972,\n";
        s += "  \"T\": 1589436922959,\n";

        s += "  \"asks\": [\n";
        s += "    [\n";
        s += "      \"6\",\n";
        s += "      \"12\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"7\",\n";
        s += "      \"12\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"8\",\n";
        s += "      \"12\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"9\",\n";
        s += "      \"12\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"10\",\n";
        s += "      \"12\"\n";
        s += "    ]\n";
        s += "  ],\n";

        s += "  \"bids\": [\n";
        s += "    [\n";
        s += "      \"5\",\n";
        s += "      \"431\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"4\",\n";
        s += "      \"431\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"3\",\n";
        s += "      \"431\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"2\",\n";
        s += "      \"431\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"1\",\n";
        s += "      \"431\"\n";
        s += "    ],\n";
        s += "    [\n";
        s += "      \"0\",\n";
        s += "      \"431\"\n";
        s += "    ]\n";
        s += "  ]\n";

        s += "}";

        QByteArray ba = s.toLatin1();
        parseJSON(ba);
    }

    setSymbol(symbol);

    QTimer::singleShot(1000, this, &ExOrderBook::onTimer);

}

//crex::mdichild::MdiType ExOrderBook::mdiType()
//{
//    return MdiChild::mdiType();
//}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void ExOrderBook::setSymbol(const QString symbol)
{
    symbol_.clear();
    symbol_ = std::move(symbol);

    if (symbol_.isEmpty())
        setWindowTitle("?");
    else
        setWindowTitle(symbol_);
}

void ExOrderBook::onTimer()
{
    if (request_)
        return;

    if (symbol_.isEmpty())
        TRACE("empty");

    request_ = protocol_->requestExchangeDepthOfMarket(symbol_, 5);
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

    QTimer::singleShot(550, this, &ExOrderBook::onTimer);
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
//    dumpToFile(QString("orderbook-dump-%1.json").arg(q++), doc);

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


