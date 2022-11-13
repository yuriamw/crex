QT       += core gui network charts websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Tell the qcustomplot header that it will be used as library:
DEFINES += QCUSTOMPLOT_USE_LIBRARY

# Link with debug version of qcustomplot if compiling in debug mode, else with release library:
CONFIG(debug, release|debug) {
  win32:QCPLIB = qcustomplotd2
  else: QCPLIB = qcustomplotd
} else {
  win32:QCPLIB = qcustomplot2
  else: QCPLIB = qcustomplot
}
LIBS += -L/usr/local/lib/ -l$$QCPLIB

# Qwt library
INCLUDEPATH += /usr/local/qwt-6.2.0-dev/include
LIBS += -L/usr/local/qwt-6.2.0-dev/lib -lqwt
#QWT_ROOT = /usr/local/qwt-6.2.0-dev
#include( $${QWT_ROOT}/features/qwtconfig.pri )

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/core.cpp \
    data/candledata.cpp \
    data/curvedata.cpp \
    data/data.cpp \
    data/datasim.cpp \
    exchange/exchangerequest.cpp \
    exchange/exwssprotocol.cpp \
    exchange/exwssrequest.cpp \
    exchangeinfo.cpp \
    exchangeprotocol.cpp \
    exorderbook.cpp \
    exqchart.cpp \
    main.cpp \
    mw.cpp \
    symbol.cpp \
    symbolitem.cpp \
    symbolmodel.cpp \
    widgets/excharttracker.cpp \
    widgets/exqcpchart.cpp \
    widgets/exqwtchart.cpp

HEADERS += \
    core/core.h \
    data/candledata.h \
    data/curvedata.h \
    data/data.h \
    data/datasim.h \
    exchange/exchangerequest.h \
    exchange/exwssprotocol.h \
    exchange/exwssrequest.h \
    exchangeinfo.h \
    exchangeprotocol.h \
    exorderbook.h \
    exqchart.h \
    logger.h \
    mdichild.h \
    mw.h \
    symbol.h \
    symbolitem.h \
    symbolmodel.h \
    widgets/excharttracker.h \
    widgets/exqcpchart.h \
    widgets/exqwtchart.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
