#ifndef EXCHARTWIDGET_H
#define EXCHARTWIDGET_H

#include <QString>
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLineItem>

#include "data/data.h"

#include "widgets/exchart.h"

namespace crex::ch {

const auto Chart_GridColumn = 1;
const auto Chart_GridRow = 1;

const auto DateScale_GridColumn = 1;
const auto DateScale_GridRow = 2;

const auto PriceScale_GridColumn = 2;
const auto PriceScale_GridRow = 1;

const auto Toolbox_GridColumn = 2;
const auto Toolbox_GridRow = 2;

class ExChartView : public QGraphicsView
{
public:
    ExChartView(const QString & symbol_name, QWidget *parent = nullptr);

    QGraphicsScene *scene();
    QGraphicsWidget *widget();
    ExChart *chart();

protected:
    void setWindowTitle(const QString & title);
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void simulateDataLine();

    void moveCursorLines(int x, int y);

private:
    QString symbol_name_;
    QGraphicsScene *scene_;
    QGraphicsWidget *widget_;
    ExChart *chart_;
    QGraphicsLineItem *vertical_cursor_;
    QGraphicsLineItem *horizontal_cursor_;

    QVector<crex::data::candle_data> candles_;
};

} // namespace crex::ch

#endif // EXCHARTWIDGET_H
