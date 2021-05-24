#include <QByteArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFile>
#include <QModelIndex>
#include <QDateTime>
#include <QTimeZone>

#include "logger.h"

#include "exchangeinfo.h"

#include "exchange/exchangerequest.h"
#include "exchangeprotocol.h"

#include "symbolmodel.h"

ExchangeInfo::ExchangeInfo(ExchangeProtocol *protocol, QObject *parent)
    : QObject(parent)
    , exchange_protocol_(protocol)
    , request_(nullptr)
    , timer_(new QTimer(this))
    , model_(new SymbolModel(QList<Symbol>()))
    , exchange_time_(0)
    , exchange_timezone_("-")
{
//    TRACE("") << QTimeZone::availableTimeZoneIds();
    timer_->setSingleShot(false);
    timer_->setInterval(550);
    connect(timer_, &QTimer::timeout, this, &ExchangeInfo::onTimer);
    timer_->start();
}

QAbstractItemModel *ExchangeInfo::model()
{
    return model_;
}

void ExchangeInfo::clear()
{
    symbols.clear();
    QModelIndex root = model_->index(0, 0, QModelIndex());
    model_->removeRows(0, model_->rowCount(root), root);
    exchange_timezone_ = "-";
    exchange_time_ = 0;
}

int ExchangeInfo::size()
{
    return symbols.size();
}

void ExchangeInfo::quoteLongInt(QByteArray & data, const QString & key)
{
    int pos_time = data.indexOf(key);
    if (pos_time > 0)
    {
        int pos_colon = data.indexOf(":", pos_time + key.size()); // + size of "key"
        int pos_num = pos_colon + 1;
        for (; (data[pos_num] < '0' || data[pos_num] > '9'); ++pos_num){;} // skip spaces if any
        int pos_comma = data.indexOf(',', pos_num);
        data.insert(pos_comma, 1, '"');
        data.insert(pos_num, 1, '"');
    }
}

const Symbol ExchangeInfo::getSymbol(const QString & symbol) const
{
    foreach (auto sym, symbols)
    {
        if (sym.symbol == symbol)
            return sym;
    }

    return Symbol();
}

////////////////////////////////////////////////////////////////////////////////
/// Slots

void ExchangeInfo::onTimer()
{
    if (request_)
        return;

    request_ = exchange_protocol_->requestExchangeInfo();
    connect(request_, &ExchangeRequest::dataReady, this, &ExchangeInfo::onExchangeInfoDataReady);
}

void ExchangeInfo::onExchangeInfoDataReady()
{
    if (request_)
    {
        QByteArray json_data(request_->data());
        request_->deleteLater();
        request_ = nullptr;

        parseJSON(json_data);
    }
}

void ExchangeInfo::onSaveSymbols(const QString &file)
{
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
    {
        TRACE("File open error:") << f.errorString();
        return;
    }
    QList<Symbol> sym(symbols);
    for (int i = 0; i< sym.size(); i++)
    {
        QString s(sym.at(i).symbol);
        s.append("\n");
        if (!f.write(s.toLatin1()))
        {
            TRACE("Failed to write file:") << file << f.errorString();
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// JSON Parser

bool ExchangeInfo::parseJSON(QByteArray & data)
{
    quoteLongInt(data, "serverTime");

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    if (doc.isNull())
    {
        TRACE("") << "JSON error:" << jsonError.errorString();
        return false;
    }

//    static int q = 0;
//    dumpToFile(QString("../exchangeinfo-dump-%1.json").arg(q++), doc);

    this->clear();
    return parseJSON(doc.object());
}

bool ExchangeInfo::dumpToFile(const QString & filename, const QJsonDocument & doc)
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

bool ExchangeInfo::parseJSON(const QJsonObject &json)
{
    if (json.contains("serverTime"))
    {
        uint64_t timestamp = QString(json["serverTime"].toString()).toLongLong();
        setExchangeTime(timestamp);
    }
    if (json.contains("timezone"))
    {
        setTimeZone(json["timezone"].toString());
    }
    if (json.contains("symbols") && json["symbols"].isArray())
    {
        QJsonArray jsonSyms = json["symbols"].toArray();
        symbols.clear();
        symbols.reserve(jsonSyms.size());
        for (int i = 0; i < jsonSyms.size(); ++i)
        {
            QJsonObject jsym = jsonSyms[i].toObject();
            Symbol symbol;
            if (!symbol.read(jsym))
            {
                return false;
            }
            symbols.append(symbol);
            model_->insertSymbol(symbol);
        }
    }
    return true;
}

QDateTime ExchangeInfo::exchangeTime()
{
    if (exchange_time_ == 0)
        return QDateTime();

    QDateTime datetime;
    datetime.setTimeZone(exchangeTimeZone());
    datetime.setTime_t(exchange_time_ / 1000);
    return datetime;
}

qlonglong ExchangeInfo::exchangeTimeStamp()
{
    return exchange_time_;
}

void ExchangeInfo::setExchangeTime(const qlonglong timestamp)
{
    exchange_time_ = timestamp;
}

QTimeZone ExchangeInfo::exchangeTimeZone()
{
    return QTimeZone(exchange_timezone_.toLatin1());
}

void ExchangeInfo::setTimeZone(const QString &timezone)
{
    exchange_timezone_ = timezone;
}
