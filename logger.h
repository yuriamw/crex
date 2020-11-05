#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QString>

#define TRACE(msg) qDebug() << QString("%1():%2:").arg(__FUNCTION__).arg(__LINE__).toLatin1().data() << msg

#endif // LOGGER_H
