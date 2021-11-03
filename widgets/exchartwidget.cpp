#include "exchartwidget.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>

#include "widgets/excandle.h"
#include "data/data.h"
#include "data/datasim.h"

ExChartWidget::ExChartWidget(QObject *parent)
    : QObject(parent)
    , scene_(new QGraphicsScene())
    , view_(new QGraphicsView(scene_))
{

    const auto scene_width = 600.0;
    const auto scene_height = 400.0;
    const auto candle_width = crex::candle::ExCandle::width() + 1;
//    const auto margin = 20 + candle_width;

    view_->setCursor(Qt::CrossCursor);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::gray);
    vertical_cursor_ = new QGraphicsLineItem(scene_width - 5, 0, scene_width - 5, scene_height);
    vertical_cursor_->setPen(pen);
    scene_->addItem(vertical_cursor_);
    horizontal_cursor_ = new QGraphicsLineItem(0, 5, scene_width, 5);
    horizontal_cursor_->setPen(pen);
    scene_->addItem(horizontal_cursor_);

//    QGraphicsLineItem *line = new QGraphicsLineItem((scene_width - margin), 0, (scene_width - margin), scene_height);
//    scene_->addItem(line);

    scene_->setSceneRect(0, 0, scene_width, scene_height);

//    view_->setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

    // Simulate candle data
    simulateDataLine();
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

QGraphicsScene *ExChartWidget::scene()
{
    return scene_;
}

QGraphicsView *ExChartWidget::view()
{
    return view_;
}

void ExChartWidget::simulateDataLine()
{
    candles_ = crex::data::DataSim::getTriangle(32, 4);
}

// Protected Events

void ExChartWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons())
    {
        moveCursorLines(event->x(), event->y());
    }
}

// Cursor

void ExChartWidget::moveCursorLines(int x, int y)
{
    vertical_cursor_->setLine(x, 0, x, scene_->height());
    horizontal_cursor_->setLine(0, y, scene_->width(), y);
}

