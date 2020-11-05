#include "logger.h"

#include "symbolmodel.h"

SymbolModel::SymbolModel(const QList<Symbol> &symbols, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<Symbol> rootData;
    rootData << Symbol();
    rootItem = new SymbolItem(rootData);
    setupModelData(symbols, rootItem);
}

SymbolModel::~SymbolModel()
{
    delete rootItem;
}

QModelIndex SymbolModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SymbolItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SymbolItem*>(parent.internalPointer());

    SymbolItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SymbolModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SymbolItem *childItem = static_cast<SymbolItem*>(index.internalPointer());
    SymbolItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SymbolModel::rowCount(const QModelIndex &parent) const
{
    SymbolItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SymbolItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SymbolModel::columnCount(const QModelIndex &parent) const
{
    int count = 0;

    if (parent.isValid())
        count = static_cast<SymbolItem*>(parent.internalPointer())->columnCount();
    else
        count = rootItem->columnCount();

//    TRACE("") << count;
    return count;
}

QVariant SymbolModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    SymbolItem *item = static_cast<SymbolItem*>(index.internalPointer());

    if (index.column() == 0)
        return QVariant(item->data(index.column()).symbol);
    else
        return QVariant();
}

Qt::ItemFlags SymbolModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant SymbolModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        return QVariant("Symbol")/*rootItem->data(section)*/;

    return QVariant();
}

void SymbolModel::insertSymbol(Symbol &symbol/*, const QModelIndex &parent*/)
{
    int rows = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), rows, rows + 1);

    QList<Symbol> columnData;
    columnData << symbol;
    rootItem->appendChild(new SymbolItem(columnData, rootItem));

    endInsertRows();
}

void SymbolModel::setupModelData(const QList<Symbol> &symbols, SymbolItem *parent)
{
    for (int i = 0; i < symbols.size(); ++i)
    {
        QList<Symbol> columnData;
        columnData << symbols[i];
        parent->appendChild(new SymbolItem(columnData, parent));
    }
}
