#pragma once

#include <QwtOHLCSample>

#include <QRect>
#include <QListIterator>

namespace crex::data {

class CandleData
{
  public:
    CandleData();
    ~CandleData();

    void append(const QwtOHLCSample &sample);
    void append(const QVector<QwtOHLCSample> &samples);
//    void clearStaleValues(double min);

    int size() const;
    QwtOHLCSample value(int index) const;

    QRectF boundingRect() const;

    void lock();
    void unlock();

  private:
    Q_DISABLE_COPY(CandleData)

    QList<QwtOHLCSample>::iterator findMatch(const QwtOHLCSample & earlyest);

    class PrivateData;
    PrivateData* m_data;
};

} // namespace crex::data
