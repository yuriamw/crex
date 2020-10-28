#ifndef SYMBOLITEM_H
#define SYMBOLITEM_H

#include <QList>

#include "symbol.h"

class SymbolItem
{
public:
    explicit SymbolItem(const QList<Symbol> &data, SymbolItem *parentItem = 0);
    ~SymbolItem();

    void appendChild(SymbolItem *child);

    SymbolItem *child(int row);
    int childCount() const;
    int columnCount() const;
    Symbol data(int column) const;
    int row() const;
    SymbolItem *parentItem();

private:
    QList<SymbolItem*> m_childItems;
    QList<Symbol> m_itemData;
    SymbolItem *m_parentItem;
};

#endif // SYMBOLITEM_H
