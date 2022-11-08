#include "exwssprotocol.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "data/data.h"
#include "logger.h"

ExWssProtocol::ExWssProtocol(const QString & exchangeName, const QString &baseUrl, const QString &path, QObject *parent)
    : QObject(parent)
    , id(0)
    , name_(exchangeName)
    , base_(baseUrl)
    , path_(path)
{
    connect(&web_socket_, &QWebSocket::connected, this, &ExWssProtocol::wssConnected);
    connect(&web_socket_, &QWebSocket::disconnected, this, &ExWssProtocol::wssDisconnected);
    connect(&web_socket_, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &ExWssProtocol::onSslErrors);
    connect(&web_socket_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ExWssProtocol::onError);

    connect(&web_socket_, &QWebSocket::textMessageReceived, this, &ExWssProtocol::onTextMessageReceived);
}

void ExWssProtocol::onTextMessageReceived(QString message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toLatin1());
    QJsonObject obj = doc.object();

    if (!obj.contains("e"))
        return;
    if (obj["e"] != "continuous_kline")
        return;

    data_.enqueue(obj);
    emit dataReady();
}

void ExWssProtocol::wssConnected()
{
    TRACE("");
    if (symbol_.isEmpty() || time_frame_.isEmpty())
    {
        TRACE("invalid params:") << "sym:" << symbol_ << "tf:" << time_frame_;
        return;
    }

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
            QString("%1_%2@continuousKline_%3").arg(symbol_).arg("perpetual").arg(time_frame_)
        };
        json["params"] = array;
        json["id"]     = ++id;

        TRACE("") << json;

        QJsonDocument d = QJsonDocument(json);

        web_socket_.sendTextMessage(QString(d.toJson(QJsonDocument::Compact).toStdString().c_str()));
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
ExWssRequest *ExWssProtocol::requestExchangeCandledata(const QString &symbol, const QString &timeFrame)
{
    symbol_ = symbol.toLower();
    time_frame_ = timeFrame;

    web_socket_.open(QUrl(base_));

    return nullptr;
}

void ExWssProtocol::unsubscribe()
{
    if (symbol_.isEmpty() || time_frame_.isEmpty())
    {
        return;
    }

//    {
//        "method": "UNSUBSCRIBE",
//        "params":
//        [
//            "btcusdt@depth"
//        ],
//        "id": 312
//    }

    QJsonObject json;
    json["method"] = "UNSUBSCRIBE";
    QJsonArray array = {
//                QString("%1@ticker").arg(sym),
//                QString("%1@depth").arg(sym),
//                QString("%1@kline_%2").arg(sym).arg(tf),
        QString("%1_%2@continuousKline_%3").arg(symbol_).arg("perpetual").arg(time_frame_)
    };
    json["params"] = array;
    json["id"]     = ++id;

    symbol_.clear();
    time_frame_.clear();

    TRACE("") << json;

    QJsonDocument d = QJsonDocument(json);

    web_socket_.sendTextMessage(QString(d.toJson(QJsonDocument::Compact).toStdString().c_str()));
    web_socket_.close();

    data_.clear();
}

QJsonObject ExWssProtocol::popData()
{
    if (!data_.empty())
    {
        return data_.dequeue();
    }
    return QJsonObject();
}
