#include "exchartview.h"

#include <algorithm>

#include <QResizeEvent>

#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>

#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include "logger.h"

#include "data/data.h"
#include "data/datasim.h"

#include "widgets/excandle.h"
#include "widgets/exitem.h"
#include "widgets/exchart.h"
#include "widgets/exaxis.h"

namespace crex::ch {

namespace {
    const auto Chart_GridColumn = 1;
    const auto Chart_GridRow = 1;

    const auto DateAxis_GridColumn = 1;
    const auto DateAxis_GridRow = 2;

    const auto PriceAxis_GridColumn = 2;
    const auto PriceAxis_GridRow = 1;

    const auto Toolbox_GridColumn = 2;
    const auto Toolbox_GridRow = 2;
}

/////////////////////////////////////////////////////////////////////////////
///
ExChartView::ExChartView(const QString & symbol_name, QWidget *parent)
    : QGraphicsView(parent)
    , symbol_name_(std::move(symbol_name))
    , scene_ (new QGraphicsScene())
    , widget_ (new QGraphicsWidget(nullptr, Qt::Window))
    , chart_(new ExChart(widget_))
{
    resize(size());

    setWindowTitle(symbol_name_);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFrameShape(QFrame::NoFrame);
    setBackgroundRole(QPalette::Window);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setScene(scene_);

    widget_->setWindowTitle("Widget");
    scene_->addItem(widget_);

    setCursor(Qt::CrossCursor);

    chart_->setSize(size());
    crex::ch::ExAxis *axisX = new crex::ch::ExAxis(Qt::Horizontal, chart_);
    chart_->setAxis(Qt::Horizontal, axisX);

    crex::ch::ExAxis *axisY = new crex::ch::ExAxis(Qt::Vertical, chart_);
    chart_->setAxis(Qt::Vertical, axisY);

    setupCursorLines();

    setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

//     Simulate candle data
    simulateDataLine();
}

QGraphicsScene *ExChartView::scene()
{
    return scene_;
}

ExChart *ExChartView::chart()
{
    return chart_;
}

// Protected

void ExChartView::setWindowTitle(const QString & title)
{
    QGraphicsView::setWindowTitle(title);
}

// Protected Events

void ExChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons())
    {
        moveCursorLines(event->x(), event->y());
    }
}

void ExChartView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    // Next two lines looked up in QChartView source
    // Setting maximumSize by widget_ locks up the size of view
    // and it become fixedSize.
    // https://code.woboq.org/qt5/qtcharts/src/charts/qchartview.cpp.html#335
    // TODO: investigate maximumSize
    setMinimumSize(widget_->minimumSize().toSize().expandedTo(minimumSize()));
    // setMaximumSize(maximumSize().boundedTo(widget_->minimumSize().toSize()));

    scene_->setSceneRect(QRectF(QPointF(0, 0), event->size()));
    widget_->resize(event->size());
    moveCursorLines(vertical_cursor_->line().x1(), horizontal_cursor_->line().y1());

    chart_->setSize(size());
}

// Functions

void ExChartView::simulateDataLine()
{
    candles_ = crex::data::DataSim::getTriangle(32, 4);

    qreal min = candles_[0].l;
    qreal max = candles_[0].h;

    for (int i = 0; i < candles_.size(); i++)
    {
        chart_->appendCandle(candles_[i].o, candles_[i].c, candles_[i].h, candles_[i].l);
        min = std::min(min, candles_[i].l);
        max = std::max(max, candles_[i].h);
    }
    chart_->axis(Qt::Vertical)->setRange(min, max);
}

// Cursor

void ExChartView::setupCursorLines()
{
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::gray);
    vertical_cursor_ = new QGraphicsLineItem(chart_->size().width(), 0, chart_->size().width(), chart_->size().height());
    vertical_cursor_->setPen(pen);
    scene_->addItem(vertical_cursor_);
    horizontal_cursor_ = new QGraphicsLineItem(0, 0, chart_->size().width(), 0);
    horizontal_cursor_->setPen(pen);
    scene_->addItem(horizontal_cursor_);
}

void ExChartView::moveCursorLines(int x, int y)
{
    if (chart_->contains(QPointF(x, y)))
    {
        vertical_cursor_->setLine(x, 0, x, chart_->size().height());
        horizontal_cursor_->setLine(0, y, chart_->size().width(), y);
    }
}

} // namespace crex::ch

