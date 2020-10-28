#include "symbolitem.h"

#include "symbol.h"

//#include <>
SymbolItem::SymbolItem(const QList<Symbol> &data, SymbolItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

SymbolItem::~SymbolItem()
{
    qDeleteAll(m_childItems);
}

void SymbolItem::appendChild(SymbolItem *item)
{
    m_childItems.append(item);
}

SymbolItem *SymbolItem::child(int row)
{
    return m_childItems.value(row);
}

int SymbolItem::childCount() const
{
    return m_childItems.count();
}

int SymbolItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<SymbolItem*>(this));

    return 0;
}

int SymbolItem::columnCount() const
{
    return m_itemData.count();
}

Symbol SymbolItem::data(int column) const
{
    return m_itemData.value(column);
}

SymbolItem *SymbolItem::parentItem()
{
    return m_parentItem;
}
