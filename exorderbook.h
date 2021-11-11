#ifndef EXORDERBOOK_H
#define EXORDERBOOK_H

#include <QWidget>
#include <QTableView>
#include <QComboBox>
#include <QLayout>

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

#include <QTimer>

#include <QJsonDocument>

#include "exchange/exchangerequest.h"
#include "exchangeprotocol.h"
#include "exchangeinfo.h"
#include "symbol.h"
#include "mdichild.h"

namespace orderbook {

    static const int    MAX_COLUMN = 2;
    static const int    PRICE_COL  = 0;
    static const int    QTY_COL    = 1;

    struct order {
        qreal price;
        qreal qty;
    };

} // namespace orderbook

////////////////////////////////////////////////////////////////////////////////
/// OrderBookModel

class ExOrderBookModel : public QAbstractTableModel
{
public:
    explicit ExOrderBookModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setOrderBook(QList<orderbook::order> &asks, QList<orderbook::order> &bids);
    void setPrecision(int pricePrecision, int quantityPrecision);

private:
    QList<struct orderbook::order>::iterator askOrBid(int row);
    QList<struct orderbook::order>::const_iterator cAskOrBid(int row) const;
    bool isAsk(int row) const;
    bool isBid(int row) const;

private:
    QList<struct orderbook::order> asks_;
    QList<struct orderbook::order> bids_;

    int price_precision_;
    int quantity_precision_;
};

////////////////////////////////////////////////////////////////////////////////
/// OrderBook

class ExOrderBook : public QWidget//, private crex::mdichild::MdiChild
{
    Q_OBJECT
public:
    explicit ExOrderBook(ExchangeProtocol *protocol, ExchangeInfo *exinfo, const QString symbol, QWidget *parent = nullptr);
//    crex::mdichild::MdiType mdiType();

public slots:
    void setSymbol(const Symbol symbol);
    void setSymbolName(const QString symbol);

signals:

private slots:
    void onTimer();
    void onDepthOfMarketDataReady();

private:
    void parseJSON(QByteArray &json_data);
    void parseJSON(const QJsonArray &json, QList<orderbook::order> & orders);
    orderbook::order parseJSONOrder(const QJsonArray &json);

    void createMenus(QLayout *layout);

private:
    QComboBox *exchange_symbol_;
    QTableView *orderbook_;
    ExchangeRequest *request_;
    ExchangeProtocol *protocol_;
    ExchangeInfo *exchange_info_;
    QTimer *timer_;
    Symbol symbol_;

    ExOrderBookModel *model_;
};

#endif // EXORDERBOOK_H
