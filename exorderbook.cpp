#include "exorderbook.h"

ExOrderBook::ExOrderBook(ExchangeProtocol *protocol, const QString symbol, QWidget *parent)
    : QTableView(parent)
    , protocol_(protocol)
{
    setSymbol(symbol);
}

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

