#include "mw.h"

#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QFrame>
#include <QLayout>

#include <QtNetwork>
#include <QUrl>

#include <QMessageBox>

#include <QDebug>

MW::MW(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *menu;
    QAction *act;
    QToolBar *toolBar;

    QWidget *w = new QWidget();
    QHBoxLayout *lh = new QHBoxLayout();
    w->setLayout(lh);

    twMarket = new QTreeWidget();
    lh->addWidget(twMarket);

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

    initNAM();
}

MW::~MW()
{
}

void MW::onConnect(bool /*checked*/)
{
    qDebug() << "exchangeInfo...";

    exchangeInfo.clear();

    reply = nam.get(QNetworkRequest(QUrl("https://fapi.binance.com/fapi/v1/exchangeInfo")));
    connect(reply, &QNetworkReply::finished, this, &MW::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &MW::httpReadyRead);

//    if (checked)
//    {
//        qDebug() << "Connect to server";
//        nam.get(QUrl("https://fapi.binance.com/fapi/v1/exchangeInfo"));
//    }
//    else
//    {
//        qDebug() << "Disconnect from server";
//    }
}

void MW::initNAM()
{
    connect(&nam, &QNetworkAccessManager::authenticationRequired, this, &MW::slotAuthenticationRequired);
    connect(&nam, &QNetworkAccessManager::sslErrors, this, &MW::sslErrors);
}

void MW::httpFinished()
{
    qDebug() << __FUNCTION__;
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
        qDebug() << "HTTP error";
        exchangeInfo.clear();
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
        qDebug() << "!!!Redirected to:" << redirectedUrl;
        return;
    }

//    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3")
//                         .arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
//    if (launchCheckBox->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
//    downloadButton->setEnabled(true);

    if (!exchangeInfo.parse())
    {
        return;
    }

    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < exchangeInfo.size(); ++i)
    {
        items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(exchangeInfo.symbols[i].symbol)));
    }
    twMarket->addTopLevelItems(items);
}

void MW::httpReadyRead()
{
    qDebug() << __FUNCTION__;
    exchangeInfo.append(reply->readAll());
}

void MW::slotAuthenticationRequired(QNetworkReply */*reply*/, QAuthenticator */*authenticator*/)
{
    qDebug() << __FUNCTION__;
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

