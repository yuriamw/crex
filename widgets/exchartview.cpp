#include "exchartview.h"

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
#include "widgets/exscale.h"

namespace crex::ch {

/////////////////////////////////////////////////////////////////////////////
///
ExChartView::ExChartView(const QString & symbol_name, QWidget *parent)
    : QGraphicsView(parent)
    , symbol_name_(std::move(symbol_name))
//    , chart_(new ExChart(widget_))
{
    setWindowTitle(symbol_name_);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFrameShape(QFrame::NoFrame);
    setBackgroundRole(QPalette::Window);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    scene_ = new QGraphicsScene();
    setScene(scene_);

    widget_ = new QGraphicsWidget(nullptr, Qt::Window);
    widget_->setWindowTitle("Widget");
    scene_->addItem(widget_);

    chart_ = new ExChart();
    scene_->addItem(chart_);

    const auto scene_width = 600.0;
    const auto scene_height = 400.0;

    resize(scene_width, scene_height);
    scene_->setSceneRect(0, 0, scene_width, scene_height);

    setCursor(Qt::CrossCursor);

    QGraphicsGridLayout *glayout = new QGraphicsGridLayout();
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);
    glayout->setContentsMargins(0, 0, 0, 0);

    chart_->setMinimumSize(320, 240);
    glayout->addItem(chart_, Chart_GridRow, Chart_GridColumn, Qt::Alignment());

    crex::ch::ExScale *vScale = new crex::ch::ExScale(Qt::Vertical);
    glayout->addItem(vScale, PriceScale_GridRow, PriceScale_GridColumn, Qt::Alignment());

    crex::ch::ExScale *hScale = new crex::ch::ExScale(Qt::Horizontal);
    glayout->addItem(hScale, DateScale_GridRow, DateScale_GridColumn, Qt::Alignment());

    crex::ch::ExItem *tBox = new crex::ch::ExItem(1);
    tBox->setMinimumSize(24, 24);
    tBox->setMaximumSize(24, 24);
    glayout->addItem(tBox, Toolbox_GridRow, Toolbox_GridColumn, Qt::Alignment());

    widget_->setLayout(glayout);

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::gray);
    vertical_cursor_ = new QGraphicsLineItem(scene_->width(), 0, scene_->width(), scene_->height());
    vertical_cursor_->setPen(pen);
    scene_->addItem(vertical_cursor_);
    horizontal_cursor_ = new QGraphicsLineItem(0, 0, scene_->width(), 0);
    horizontal_cursor_->setPen(pen);
    scene_->addItem(horizontal_cursor_);

    setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

//     Simulate candle data
//    simulateDataLine();
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
    //  setMaximumSize(maximumSize().boundedTo(widget_->minimumSize().toSize()));

    scene_->setSceneRect(QRectF(QPointF(0, 0), event->size()));
    widget_->resize(event->size());
}

// Functions

void ExChartView::simulateDataLine()
{
    candles_ = crex::data::DataSim::getTriangle(32, 4);

    const auto scene_width = scene_->width();
    const auto scene_height = scene_->height();
    const auto candle_width = crex::candle::ExCandle::width() + 1;

    int min = 0;
    int max = 0;
    for (int i = 1; i < candles_.size(); i++)
    {
        if (candles_[i].h > candles_[max].h)
            max = i;

        if (candles_[i].l < candles_[min].l)
            min = i;
    }

    for (int i = 0; i < candles_.size(); i++)
    {
        crex::candle::ExCandle *candle = new crex::candle::ExCandle();
        candle->setCandle(candles_[i].o, candles_[i].c, candles_[i].h, candles_[i].l);
        candle->item()->setPos(scene_width - candle_width * (i + 1), scene_height - candles_[i].h);
        scene_->addItem(candle->item());
    }
}

// Cursor

void ExChartView::moveCursorLines(int x, int y)
{
    if (chart_->contains(QPointF(x, y)))
    {
        vertical_cursor_->setLine(x, 0, x, scene_->height());
        horizontal_cursor_->setLine(0, y, scene_->width(), y);
    }
}

} // namespace crex::ch

