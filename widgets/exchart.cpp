#include "widgets/exchart.h"

#include <qcustomplot.h>

#include <QSharedPointer>
#include <QCursor>

namespace crex::chart {

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::ExChart
/// \param parent
///
ExChart::ExChart(QWidget *parent):
    QCustomPlot(parent)
  , hCursorLine(new QCPItemLine(this))
  , vCursorLine(new QCPItemLine(this))
  , timeFrame("1H")
  , tfButton(new QToolButton(this))
  , olhcDisplay(new QLabel("OLHC", this))
{
    yAxis->setVisible(false);
    yAxis2->setVisible(true);
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));

    cursorLines.append(hCursorLine);
    cursorLines.append(vCursorLine);
    for (int i = 0; i < cursorLines.size(); i++)
    {
        cursorLines.at(i)->start->setType(QCPItemPosition::ptAbsolute);
        cursorLines.at(i)->end->setType(QCPItemPosition::ptAbsolute);
        cursorLines.at(i)->setPen(QPen(Qt::DotLine));
    }

    financial = new QCPFinancial(xAxis, yAxis2);
    financial->setChartStyle(QCPFinancial::csCandlestick);
    financial->setWidthType(QCPFinancial::wtAbsolute);
    financial->setWidth(5);

    QSharedPointer<QCPAxisTickerDateTime> xTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xAxis->setTicker(xTicker);
    setCursor(QCursor(Qt::CrossCursor));

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    createTimeFrameButton();
}

void ExChart::setCandles(QSharedPointer<QCPFinancialDataContainer> cont)
{
    financial->setData(cont);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::resizeEvent
/// \param event
///
void ExChart::resizeEvent(QResizeEvent *event)
{
    QCustomPlot::resizeEvent(event);

    if (financial->widthType() != QCPFinancial::wtAbsolute)
        return;

//    qDebug() << event << viewport() << axisRect(0)->size() << axisRect(0)->topLeft() << axisRect(0)->bottomRight() << axisRectCount() << graphCount();
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::mouseMoveEvent
/// \param event
///
void ExChart::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    QRect bounds = QRect(axisRect(0)->topLeft(), axisRect(0)->bottomRight());
    if (event->x() >= bounds.topLeft().x() && event->x() < bounds.bottomRight().x())
    {
        vCursorLine->start->setCoords(event->x(), bounds.topLeft().y());
        vCursorLine->end->setCoords(event->x(), bounds.bottomRight().y());
    }
    if (event->y() >= bounds.topLeft().y() && event->y() < bounds.bottomRight().y())
    {
        hCursorLine->start->setCoords(bounds.topLeft().x(), event->y());
        hCursorLine->end->setCoords(bounds.bottomRight().x(), event->y());
    }
    replot();

    const auto d = financial->data()->findBegin(xAxis->pixelToCoord(event->x()), true);
    QString olhc(QString("%1 O:%2 H:%3 L:%4 C:%5")
                 .arg(QDateTime::fromMSecsSinceEpoch(d->key * 1000).toString("yyyy-MM-dd hh:mm"))
                 .arg(d->open)
                 .arg(d->high)
                 .arg(d->low)
                 .arg(d->close)
            );

    placeOlhcLabel(olhc);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::createTimeFrameButton
///
void ExChart::createTimeFrameButton()
{
    tfButton->move(4, 4);

    placeTimeFrameButton();

    connect(tfButton, &QToolButton::clicked, this, &ExChart::switchTF);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::placeTimeFrameButton
///
void ExChart::placeTimeFrameButton()
{
    tfButton->setText(timeFrame);

    QFontMetrics fm(tfButton->font());
    int w = fm.width(timeFrame);
    int h = fm.height();
    QSize sz(w, h);
    sz += QSize(4, 2);

    tfButton->resize(sz);
    tfButton->setFocusPolicy(Qt::NoFocus);

    olhcDisplay->move(tfButton->pos().x() + tfButton->size().width() + 4, 4);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::placeOlhcLabel
///
void ExChart::placeOlhcLabel(const QString &s)
{
    olhcDisplay->setText(s);
    QFontMetrics fm(olhcDisplay->font());
    int w = fm.width(s);
    int h = fm.height();
    QSize sz(w, h);
//    sz += QSize(4, 2);
    olhcDisplay->resize(sz);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ExChart::switchTF
///
void ExChart::switchTF()
{
    static QString timeFrames[] = { "1m", "5m", "15m", "30m", "1H", "4H", "1D", "1W", "1M", QString()};

    for (int i = 0; !timeFrames[i].isNull(); i++)
    {
        if (timeFrames[i] == timeFrame)
        {
            i++; if (timeFrames[i] == "") i = 0;
            timeFrame = timeFrames[i];
            break;
        }
    }
    placeTimeFrameButton();
}

} // namespace crex::chart

