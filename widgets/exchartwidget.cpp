#include "exchartwidget.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>

#include <widgets/excandle.h>

ExChartWidget::ExChartWidget()
    : scene_(new QGraphicsScene())
    , view_(new QGraphicsView(scene_))
{
    const auto scene_width = 600.0;
    const auto scene_height = 400.0;
    const auto candle_width = crex::candle::ExCandle::width() + 1;
    const auto margin = 20 + candle_width;

    QGraphicsLineItem *line = new QGraphicsLineItem((scene_width - margin), 0, (scene_width - margin), scene_height);
    scene_->addItem(line);

    scene_->setSceneRect(0, 0, scene_width, scene_height);

//    view_->setBackgroundBrush(QBrush(QColor(Qt::magenta), Qt::SolidPattern));

    crex::candle::ExCandle *candle = new crex::candle::ExCandle();
    candle->setCandle(11, 19, 44, 9);
    candle->item()->moveBy((scene_width - margin) - candle_width * 1, scene_height - 44);
    scene_->addItem(candle->item());

    crex::candle::ExCandle *candle2 = new crex::candle::ExCandle();
    candle2->setCandle(19, 5, 21, 5);
    candle2->item()->moveBy((scene_width - margin) - candle_width * 2, scene_height - 21);
    scene_->addItem(candle2->item());

    crex::candle::ExCandle *candle3 = new crex::candle::ExCandle();
    candle3->setCandle(5, 150, 190, 5);
    candle3->item()->moveBy((scene_width - margin) - candle_width * 3, scene_height - 190);
    scene_->addItem(candle3->item());
}

QGraphicsScene *ExChartWidget::scene()
{
    return scene_;
}

QGraphicsView *ExChartWidget::view()
{
    return view_;
}

