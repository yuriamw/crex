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
//    if (!chart)
//        chart_ = new ExChart();
//    scene_->addItem(chart_);

    scene_ = new QGraphicsScene();
    setScene(scene_);

    widget_ = new QGraphicsWidget(nullptr, Qt::Window);
    widget_->setWindowTitle("Widget");
    scene_->addItem(widget_);

    const auto scene_width = 600.0;
    const auto scene_height = 400.0;

    resize(scene_width, scene_height);
    scene_->setSceneRect(0, 0, scene_width, scene_height);

    crex::ch::ExItem *eim;

//    QGraphicsLinearLayout *vlayout = new QGraphicsLinearLayout(Qt::Vertical);
//    QGraphicsLinearLayout *hlayout = new QGraphicsLinearLayout(Qt::Horizontal, vlayout);
//    hlayout->setSpacing(5);

//    ExChart *ch = new ExChart(0);
//    ch->setMinimumSize(32, 44);
//    ch->setMaximumSize(32, 44);
//    hlayout->addItem(ch);
//    hlayout->setStretchFactor(ch, 1);

//    eim = new crex::ch::ExItem(1);
//    eim->setMinimumSize(48, 128);
////    eim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
//    hlayout->addItem(eim);
//    hlayout->setStretchFactor(eim, 1);

//    eim = new crex::ch::ExItem(2);
////    eim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
//    eim->setMinimumSize(24, 128);
//    hlayout->addItem(eim);
//    hlayout->setStretchFactor(eim, 3);

//    eim = new crex::ch::ExItem(3);
////    eim->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
//    hlayout->addItem(eim);
//    hlayout->setStretchFactor(eim, 15);

//    vlayout->addItem(hlayout);

    const auto bigW = 120;
    const auto bigH = 100;
    QGraphicsGridLayout *glayout = new QGraphicsGridLayout(/*vlayout*/);

    eim = new crex::ch::ExItem(1);
    eim->setMinimumSize(32, 16);
    glayout->addItem(eim, 0, 0, Qt::Alignment());

    eim = new crex::ch::ExItem(1);
    eim->setMinimumSize(bigW, 16);
    glayout->addItem(eim, 0, 1, Qt::Alignment());

    eim = new crex::ch::ExItem(1);
    eim->setMinimumSize(32, 16);
    glayout->addItem(eim, 0, 2, Qt::Alignment());

    eim = new crex::ch::ExItem(2);
    eim->setMinimumSize(32, bigH);
    glayout->addItem(eim, 1, 0, Qt::Alignment());

    eim = new crex::ch::ExItem(2);
    eim->setMinimumSize(bigW, bigH);
    glayout->addItem(eim, 1, 1, Qt::Alignment());

    eim = new crex::ch::ExItem(2);
    eim->setMinimumSize(32, bigH);
    glayout->addItem(eim, 1, 2, Qt::Alignment());

    eim = new crex::ch::ExItem(3);
    eim->setMinimumSize(32, 16);
    glayout->addItem(eim, 2, 0, Qt::Alignment());

    eim = new crex::ch::ExItem(3);
    eim->setMinimumSize(bigW, 16);
    glayout->addItem(eim, 2, 1, Qt::Alignment());

    eim = new crex::ch::ExItem(3);
    eim->setMinimumSize(32, 16);
    glayout->addItem(eim, 2, 2, Qt::Alignment());

//    vlayout->addItem(glayout);

//    widget_->setLayout(vlayout);
    widget_->setLayout(glayout);

//    scene_->addItem(chart_);

    setCursor(Qt::CrossCursor);
//    QPen pen;
//    pen.setStyle(Qt::DashLine);
//    pen.setColor(Qt::gray);
//    vertical_cursor_ = new QGraphicsLineItem(scene_->width(), 0, scene_->width(), scene_->height());
//    vertical_cursor_->setPen(pen);
//    scene_->addItem(vertical_cursor_);
//    horizontal_cursor_ = new QGraphicsLineItem(0, 0, scene_->width(), 0);
//    horizontal_cursor_->setPen(pen);
//    scene_->addItem(horizontal_cursor_);

//    setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

    // Simulate candle data
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

//void ExView::mouseMoveEvent(QMouseEvent *event)
//{
//    if (!event->buttons())
//    {
//        moveCursorLines(event->x(), event->y());
//    }
//}

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
    vertical_cursor_->setLine(x, 0, x, scene_->height());
    horizontal_cursor_->setLine(0, y, scene_->width(), y);
}

} // namespace crex::ch

