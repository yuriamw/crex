#ifndef EXCHART_H
#define EXCHART_H

#include <qcustomplot.h>

#include <QSharedPointer>
#include <QVector>
#include <QString>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QToolButton>
#include <QLabel>

namespace crex::chart {

class ExChart : public QCustomPlot
{
public:
    ExChart(QWidget *parent = nullptr);

    void setCandles(QSharedPointer<QCPFinancialDataContainer> cont);

public slots:
    void switchTF();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void createTimeFrameButton();
    void placeTimeFrameButton();
    void placeOlhcLabel(const QString &s = QString());

private:
    QCPItemLine *hCursorLine;
    QCPItemLine *vCursorLine;
    QVector<QCPItemLine *> cursorLines;

    QCPFinancial *financial;

    QString timeFrame;
    QToolButton *tfButton;
    QLabel *olhcDisplay;
};

} // namespace crex::chart

#endif // EXCHART_H
