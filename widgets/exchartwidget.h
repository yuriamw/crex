#ifndef EXCHARTWIDGET_H
#define EXCHARTWIDGET_H

#include <QObject>
#include <QMouseEvent>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>

#include "data/data.h"

class ExChartWidget : public QObject
{
    Q_OBJECT
public:
    ExChartWidget(QObject *parent = nullptr);

    QGraphicsScene *scene();
    QGraphicsView *view();

private:
    void simulateDataLine();

    void moveCursorLines(int x, int y);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene_;
    QGraphicsView *view_;
    QGraphicsLineItem *vertical_cursor_;
    QGraphicsLineItem *horizontal_cursor_;

    QVector<crex::data::candle_data> candles_;
};

#endif // EXCHARTWIDGET_H
