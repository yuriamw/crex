#include "mw.h"

#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStringList>

#include <QTreeView>
#include <QWidget>
#include <QFrame>
#include <QLayout>

#include <QTimer>

#include <QtNetwork>
#include <QUrl>

#include <QMessageBox>

#include "logger.h"

#include "exchangeprotocol.h"

MW::MW(QWidget *parent)
    : QMainWindow(parent)
    , exchangeProtocol(new ExchangeProtocol("Binance FUTURES", "https://fapi.binance.com", "fapi/v1", this))
{
    QMenu *menu;
    QAction *act;
    QToolBar *toolBar;

    QWidget *w = new QWidget();
    QHBoxLayout *lh = new QHBoxLayout();
    w->setLayout(lh);

    tvMarket = new QTreeView();
    lh->addWidget(tvMarket);
    tvMarket->setModel(exchangeInfo.model());

    QFrame *p = new QFrame();
    p->setMinimumWidth(300);
    p->setFrameStyle(QFrame::Panel | QFrame::Raised);
    lh->addWidget(p);

    setCentralWidget(w);

    menu = menuBar()->addMenu(tr("Connection"));

    act = new QAction(QIcon::fromTheme("network-wired"), tr("&Connect"));
//    act->setCheckable(true);
    connect(act, &QAction::triggered, this, &MW::onConnect);
    toolBar = addToolBar(tr("Connection"));
    toolBar->addAction(act);
    menu->addAction(act);

    exchange_date_time_ = new QLabel(tr("no data"));
    statusBar()->addPermanentWidget(exchange_date_time_);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MW::updateTimeLabel);
    timer->start(500);

    initNAM();

    statusBar()->showMessage(tr("Ready"));
}

MW::~MW()
{
}

void MW::updateTimeLabel()
{
    exchange_date_time_->setText(QString("%1 %2")
            .arg(exchangeInfo.exchangeTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(exchangeInfo.exchangeTimeZone().displayName(QTimeZone::GenericTime, QTimeZone::ShortName)));
}

void MW::onConnect(bool /*checked*/)
{
    TRACE("exchangeInfo...");

    data_.clear();

    reply = nam.get(QNetworkRequest(QUrl("https://fapi.binance.com/fapi/v1/exchangeInfo")));
    connect(reply, &QNetworkReply::finished, this, &MW::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &MW::httpReadyRead);

//    if (checked)
//    {
//        TRACE("") << "Connect to server";
//        nam.get(QUrl("https://fapi.binance.com/fapi/v1/exchangeInfo"));
//    }
//    else
//    {
//        TRACE("") << "Disconnect from server";
//    }
}

void MW::initNAM()
{
    connect(&nam, &QNetworkAccessManager::authenticationRequired, this, &MW::slotAuthenticationRequired);
    connect(&nam, &QNetworkAccessManager::sslErrors, this, &MW::sslErrors);
}

void MW::httpFinished()
{
    TRACE("");
//    QFileInfo fi;
//    if (file) {
//        fi.setFile(file->fileName());
//        file->close();
//        file.reset();
//    }

//    if (httpRequestAborted) {
//        reply->deleteLater();
//        reply = nullptr;
//        return;
//    }

    if (reply->error()) {
        TRACE("") << "HTTP error";
        exchangeInfo.clear();
        data_.clear();
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = nullptr;

    if (!redirectionTarget.isNull()) {
        QUrl url;
        const QUrl redirectedUrl = url.resolved(redirectionTarget.toUrl());
//        if (QMessageBox::question(this, tr("Redirect"),
//                                  tr("Redirect to %1 ?").arg(redirectedUrl.toString()),
//                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
//            QFile::remove(fi.absoluteFilePath());
//            downloadButton->setEnabled(true);
//            statusLabel->setText(tr("Download failed:\nRedirect rejected."));
//            return;
//        }
//        file = openFileForWrite(fi.absoluteFilePath());
//        if (!file) {
//            downloadButton->setEnabled(true);
//            return;
//        }
//        startRequest(redirectedUrl);
        TRACE("") << "!!!Redirected to:" << redirectedUrl;
        return;
    }

//    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
//                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
//    if (launchCheckBox->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
//    downloadButton->setEnabled(true);

//    {
//        static int q = 0;
//        QFile file(QString("exchangeinfo-%1.json").arg(q++));
//        if (!file.open(QIODevice::WriteOnly))
//        {
//            TRACE("") << "IO error:" << file.errorString();
//        }
//        if (file.write(data_) < 0)
//        {
//            TRACE("") << "IO error:" << file.errorString();
//        }
//        file.close();
//    }
    if (!exchangeInfo.parse(data_))
    {
        return;
    }
}

void MW::httpReadyRead()
{
    TRACE("");
    data_.append(reply->readAll());
}

void MW::slotAuthenticationRequired(QNetworkReply */*reply*/, QAuthenticator */*authenticator*/)
{
    TRACE("");
}

void MW::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString errorString;
    for (const QSslError &error : errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("SSL error has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}

