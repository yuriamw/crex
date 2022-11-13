/*****************************************************************************
 * Qwt Examples - Copyright (C) 2002 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "candledata.h"

#include <QwtOHLCSample>

#include <QVector>
#include <QList>
#include <QMutex>
#include <QReadWriteLock>
#include <QDateTime>

#include "logger.h"

namespace crex::data {

class CandleData::PrivateData
{
  public:
    PrivateData()
        : boundingRect( 1.0, 1.0, -2.0, -2.0 ) // invalid

    {
        values.reserve( 1000 );
    }

    void append(const QwtOHLCSample &sample)
    {
        values.append(sample);

        if (boundingRect.width() < 0 || boundingRect.height() < 0)
        {
            boundingRect.setBottomLeft(QPointF(sample.time, sample.low));
            boundingRect.setTopRight(QPointF(sample.time, sample.high));
        }
        else
        {
            if (boundingRect.right() < sample.time)
                boundingRect.setRight(sample.time);

            if (boundingRect.bottom() > sample.low)
                boundingRect.setBottom(sample.low);
            if (boundingRect.top() < sample.high)
                boundingRect.setTop(sample.high);
        }
//        TRACE("br") << sample.time <<  sample.low << sample.high << "|" << boundingRect.left() << boundingRect.right() << boundingRect.bottom() << boundingRect.top();
    }

    QRectF boundingRect;

    QReadWriteLock lock;
    QList<QwtOHLCSample> values;
};

CandleData::CandleData()
{
    m_data = new PrivateData();
}

CandleData::~CandleData()
{
    delete m_data;
}

int CandleData::size() const
{
    return m_data->values.size();
}

QwtOHLCSample CandleData::value(int index) const
{
    return m_data->values[index];
}

QRectF CandleData::boundingRect() const
{
    return m_data->boundingRect;
}

void CandleData::lock()
{
    m_data->lock.lockForRead();
}

void CandleData::unlock()
{
    m_data->lock.unlock();
}

void CandleData::append(const QwtOHLCSample &sample)
{
    m_data->lock.lockForWrite();

    if (!m_data->values.isEmpty())
    {
        auto it = findMatch(sample);

        if (it != m_data->values.end())
        {
            m_data->values.erase(it, m_data->values.end());
        }
    }

    m_data->append(sample);

    m_data->lock.unlock();
}

void CandleData::append(const QVector<QwtOHLCSample> &samples)
{
    if (samples.isEmpty())
        return;

    m_data->lock.lockForWrite();

    if (!m_data->values.isEmpty())
    {
        auto it = findMatch(samples.at(0));

        if (it != m_data->values.end())
        {
            m_data->values.erase(it, m_data->values.end());
        }
    }

    auto data = samples.data();
    auto len = samples.size();
    for (int i = 0; i < len; i++)
    {
        m_data->append(data[i]);
    }

    m_data->lock.unlock();
}

QList<QwtOHLCSample>::iterator CandleData::findMatch(const QwtOHLCSample & earliest)
{
    auto rmStart = QDateTime::fromMSecsSinceEpoch(earliest.time);
    auto it = m_data->values.rbegin();
    auto end = m_data->values.rend();
    for (;it != end; it++)
    {
        auto rmEnd = QDateTime::fromMSecsSinceEpoch(it->time);
        if (rmStart < rmEnd)
        {
            continue;
        }
        break;
    }

    return it != end ? (it+1).base() : m_data->values.end();
}

} // namespace crex::data
