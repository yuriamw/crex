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
    , chart_(new ExChart())
{
    setWindowTitle(symbol_name_);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFrameShape(QFrame::NoFrame);
    setBackgroundRole(QPalette::Window);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setScene(scene_);

    widget_->setWindowTitle("Widget");
    scene_->addItem(widget_);

    scene_->addItem(chart_);

    resize(size());

    setCursor(Qt::CrossCursor);

    QGraphicsGridLayout *glayout = new QGraphicsGridLayout();
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);
    glayout->setContentsMargins(0, 0, 0, 0);

    glayout->addItem(chart_, Chart_GridRow, Chart_GridColumn, Qt::Alignment());

    crex::ch::ExAxis *vScale = new crex::ch::ExAxis(Qt::Vertical);
    glayout->addItem(vScale, PriceAxis_GridRow, PriceAxis_GridColumn, Qt::Alignment());

    crex::ch::ExAxis *hScale = new crex::ch::ExAxis(Qt::Horizontal);
    glayout->addItem(hScale, DateAxis_GridRow, DateAxis_GridColumn, Qt::Alignment());

    crex::ch::ExItem *tBox = new crex::ch::ExItem();
    tBox->setMinimumSize(24, 24);
    tBox->setMaximumSize(24, 24);
    glayout->addItem(tBox, Toolbox_GridRow, Toolbox_GridColumn, Qt::Alignment());

    widget_->setLayout(glayout);

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
    //  setMaximumSize(maximumSize().boundedTo(widget_->minimumSize().toSize()));

    scene_->setSceneRect(QRectF(QPointF(0, 0), event->size()));
    widget_->resize(event->size());
    moveCursorLines(vertical_cursor_->line().x1(), horizontal_cursor_->line().y1());
}

// Functions

void ExChartView::simulateDataLine()
{
    candles_ = crex::data::DataSim::getTriangle(32, 4);

    for (int i = 0; i < candles_.size(); i++)
    {
        chart_->appendCandle(candles_[i].o, candles_[i].c, candles_[i].h, candles_[i].l);
    }
}

// Cursor

void ExChartView::setupCursorLines()
{
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::gray);
    vertical_cursor_ = new QGraphicsLineItem(chart_->geometry().width(), 0, chart_->geometry().width(), chart_->geometry().height());
    vertical_cursor_->setPen(pen);
    scene_->addItem(vertical_cursor_);
    horizontal_cursor_ = new QGraphicsLineItem(0, 0, chart_->geometry().width(), 0);
    horizontal_cursor_->setPen(pen);
    scene_->addItem(horizontal_cursor_);
}

void ExChartView::moveCursorLines(int x, int y)
{
    if (chart_->contains(QPointF(x, y)))
    {
        vertical_cursor_->setLine(x, 0, x, chart_->geometry().height());
        horizontal_cursor_->setLine(0, y, chart_->geometry().width(), y);
    }
}

} // namespace crex::ch

