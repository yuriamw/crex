#ifndef EXCHARTWIDGET_H
#define EXCHARTWIDGET_H

#include <QGraphicsScene>
#include <QGraphicsView>

class ExChartWidget
{
public:
    ExChartWidget();

    QGraphicsScene *scene();
    QGraphicsView *view();

private:
    QGraphicsScene *scene_;
    QGraphicsView *view_;
};

#endif // EXCHARTWIDGET_H
