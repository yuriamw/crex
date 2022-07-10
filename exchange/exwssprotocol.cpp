#include "exwssprotocol.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "data/data.h"
#include "logger.h"

ExWssProtocol::ExWssProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path, QObject *parent)
    : QObject(parent)
    , name_(exchangeName)
    , base_(baseUrl)
    , path_(path)
{
//    connect(&web_socket_, &QWebSocket::connected, this, &ExWssProtocol::onConnected);
    connect(&web_socket_, &QWebSocket::disconnected, this, &ExWssProtocol::wssDisconnected);
    connect(&web_socket_, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &ExWssProtocol::onSslErrors);
    connect(&web_socket_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ExWssProtocol::onError);

    connect(&web_socket_, &QWebSocket::textMessageReceived, this, &ExWssProtocol::onTextMessageReceived);
}

void ExWssProtocol::onConnected()
{
    TRACE("");

// 	{
// 		"method": "SUBSCRIBE",
// 		"params":
// 		[
// 			"btcusdt@aggTrade",
// 			"btcusdt@depth"
// 		],
// 		"id": 1
// 	}
    QJsonObject json;
    json["method"] = "SUBSCRIBE";
    QJsonArray array = {"btcusdt@ticker", "btcusdt@depth"};
    json["params"] = array;
    json["id"]     = 1;

    TRACE("Subscription:") << json;

    QJsonDocument d = QJsonDocument(json);

    web_socket_.sendTextMessage(QString(d.toJson(QJsonDocument::Compact).toStdString().c_str()));
}

void ExWssProtocol::onTextMessageReceived(QString message)
{
    static int i = -1;
    static QStringList list;

    QJsonDocument doc = QJsonDocument::fromJson(message.toLatin1());
//    crex::data::dumpToFile(QString("../wss/wss-%1.json").arg(++i, 8, 10, QChar('0')), doc);

    QString type = "UNKNOWN";
    QJsonObject obj = doc.object();
    if (obj.contains("id"))
        type = QString("ID: %1").arg(obj["id"].toInt());
    if (obj.contains("e"))
        type = QString("e: %1").arg(obj["e"].toString());

    if (list.indexOf(QRegExp(QString("^[0-9].*: %1").arg(type))) < 0)
    {
        QString t = QString("%1: %2").arg(i).arg(type);
        list.append(t);
        TRACE("t:") << t;
    }
}

void ExWssProtocol::wssDisconnected()
{
    TRACE("");
}

void ExWssProtocol::onSslErrors(const QList<QSslError> &errors)
{
// 	Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    TRACE("") << "SslError:\n" << errors;

    web_socket_.ignoreSslErrors();
}

void ExWssProtocol::onError(QAbstractSocket::SocketError error)
{
    TRACE("") << "Error:\n" << error << "\nstr: " << web_socket_.errorString();
}

/////////////////////////////////////////////////////////////////////////////
ExWssRequest *ExWssProtocol::requestExchangeCandledata(const QString &symbol, const QString &timeFrame, const qlonglong startTime)
{
    Q_UNUSED(symbol);
    Q_UNUSED(timeFrame);
    Q_UNUSED(startTime);

    web_socket_.open(QUrl(base_));

    auto sym = symbol.toLower();
    auto tf = timeFrame;
    connect(&web_socket_, &QWebSocket::connected, this, [this, sym, tf](){
        // 	{
        // 		"method": "SUBSCRIBE",
        // 		"params":
        // 		[
        // 			"btcusdt@aggTrade",
        // 			"btcusdt@depth"
        // 		],
        // 		"id": 1
        // 	}
            QJsonObject json;
            json["method"] = "SUBSCRIBE";
            QJsonArray array = {
//                QString("%1@ticker").arg(sym),
//                QString("%1@depth").arg(sym),
//                QString("%1@kline_%2").arg(sym).arg(tf),
                QString("%1_%2@continuousKline_%3").arg(sym).arg("perpetual").arg(tf)
            };
            json["params"] = array;
            json["id"]     = 1;

            TRACE("Subscription:") << json;

            QJsonDocument d = QJsonDocument(json);

            web_socket_.sendTextMessage(QString(d.toJson(QJsonDocument::Compact).toStdString().c_str()));
    });

    return nullptr;
}

