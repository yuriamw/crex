#ifndef SYMBOLMODEL_H
#define SYMBOLMODEL_H

#include <QAbstractItemModel>

#include "symbol.h"
#include "symbolitem.h"

class SymbolModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SymbolModel(const QList<Symbol> &symbols, QObject *parent = 0);
    ~SymbolModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void insertSymbol(Symbol &symbol/*, const QModelIndex &parent = QModelIndex()*/);

private:
    void setupModelData(const QList<Symbol> &symbols, SymbolItem *parent);

    SymbolItem *rootItem;};

#endif // SYMBOLMODEL_H
