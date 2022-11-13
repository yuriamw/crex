#pragma once

#include <QDebug>
#include <QString>

#define TRACE(msg) qDebug() << QString("%1: %2():%3:").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).toLatin1().data() << msg
