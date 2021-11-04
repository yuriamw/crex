#ifndef EXCHARTWIDGET_H
#define EXCHARTWIDGET_H

#include <QObject>
#include <QMouseEvent>
#include <QResizeEvent>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLineItem>

#include "data/data.h"

#include "widgets/exchart.h"

namespace crex::ch {

class ExView : public QGraphicsView
{
public:
    ExView(QGraphicsScene *scene, QGraphicsWidget *child, QWidget *parent = nullptr);

protected:
//    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsWidget *child_;
};

class ExChartWidget : public QObject
{
    Q_OBJECT
public:
    ExChartWidget(QObject *parent = nullptr);

    QGraphicsScene *scene();
    QGraphicsWidget *widget();
    QGraphicsView *view();
    ExChart *chart();

private:
    void simulateDataLine();

    void moveCursorLines(int x, int y);

private:
    QGraphicsScene *scene_;
    QGraphicsWidget *widget_;
    ExView *view_;
    ExChart *chart_;
    QGraphicsLineItem *vertical_cursor_;
    QGraphicsLineItem *horizontal_cursor_;

    QVector<crex::data::candle_data> candles_;
};

} // namespace crex::ch

#endif // EXCHARTWIDGET_H
