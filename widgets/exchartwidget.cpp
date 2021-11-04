#include "exchartwidget.h"

#include <QResizeEvent>

#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>

#include <QGraphicsLinearLayout>

#include "logger.h"

#include "data/data.h"
#include "data/datasim.h"

#include "widgets/excandle.h"
#include "widgets/exitem.h"
#include "widgets/exchart.h"

namespace crex::ch {

/////////////////////////////////////////////////////////////////////////////
///
ExView::ExView(QGraphicsScene *scene, QGraphicsWidget *child, QWidget *parent)
    : QGraphicsView(scene, parent)
    , child_(child)
{}

// Protected Events

//void ExView::mouseMoveEvent(QMouseEvent *event)
//{
//    if (!event->buttons())
//    {
//        moveCursorLines(event->x(), event->y());
//    }
//}

void ExView::resizeEvent(QResizeEvent *event)
{
    child_->resize(event->size());
}

/////////////////////////////////////////////////////////////////////////////
///
ExChartWidget::ExChartWidget(QObject *parent)
    : QObject(parent)
    , scene_(new QGraphicsScene())
    , widget_(new QGraphicsWidget())
    , view_(new ExView(scene_, widget_))
//    , chart_(new ExChart(widget_))
{
    QGraphicsView *v = view_;
    widget_->setWindowTitle("Chart Widget");
    widget_->palette().setBrush(QPalette::Window, QBrush(Qt::cyan));

    const auto scene_width = 600.0;
    const auto scene_height = 400.0;

//    scene_->setSceneRect(0, 0, scene_width, scene_height);

    QGraphicsLinearLayout *window = new QGraphicsLinearLayout(Qt::Vertical);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, window);
    layout->setSpacing(5);

    crex::w::ExItem *eim = new crex::w::ExItem(1);
    eim->setMinimumSize(48, 128);
//    eim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    layout->addItem(eim);
    layout->setStretchFactor(eim, 1);

    eim = new crex::w::ExItem(2);
//    eim->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    eim->setMinimumSize(24, 128);
    layout->addItem(eim);
    layout->setStretchFactor(eim, 3);

    eim = new crex::w::ExItem(3);
//    eim->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    layout->addItem(eim);
    layout->setStretchFactor(eim, 15);

    window->addItem(layout);
    widget_->setLayout(window);

    scene_->addItem(widget_);

//    scene_->addItem(chart_);

    view_->resize(scene_width, scene_height);
    view_->setCursor(Qt::CrossCursor);
//    QPen pen;
//    pen.setStyle(Qt::DashLine);
//    pen.setColor(Qt::gray);
//    vertical_cursor_ = new QGraphicsLineItem(scene_->width(), 0, scene_->width(), scene_->height());
//    vertical_cursor_->setPen(pen);
//    scene_->addItem(vertical_cursor_);
//    horizontal_cursor_ = new QGraphicsLineItem(0, 0, scene_->width(), 0);
//    horizontal_cursor_->setPen(pen);
//    scene_->addItem(horizontal_cursor_);

//    view_->setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

    // Simulate candle data
//    simulateDataLine();
}

QGraphicsScene *ExChartWidget::scene()
{
    return scene_;
}

QGraphicsView *ExChartWidget::view()
{
    return view_;
}

ExChart *ExChartWidget::chart()
{
    return chart_;
}

void ExChartWidget::simulateDataLine()
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

void ExChartWidget::moveCursorLines(int x, int y)
{
    vertical_cursor_->setLine(x, 0, x, scene_->height());
    horizontal_cursor_->setLine(0, y, scene_->width(), y);
}

} // namespace crex::ch

