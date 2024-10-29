QT += core
QT -= gui
QT       += core network

CONFIG += c++11

TARGET = vsdc3ConsGetOsc
CONFIG += console
CONFIG -= app_bundle


TEMPLATE = app

SOURCES += main.cpp \
#    ../VsDC3/MovingCoilsServer.cpp \
    ../VsDC3/processData.cpp \
#    vsdc3noiseofintegrals.cpp \
#    vsdc3readosc.cpp \
#    vsdc3abserror.cpp \
    vsdc3_meas_header.cpp \
    ../VsDC3/vmecardbaseport.cpp \
    ../VsDC3/vsdccardtest.cpp \
    ../VsDC3/vsdc3v2.cpp \
    ../VsDC3/movingcoilsvmecardport.cpp# \
#    ../VsDC3/vme_cards_updater.cpp

HEADERS += \
#    ../VsDC3/MovingCoilsServer.h \
    ../VsDC3/vsdc3RegDefs.h \
    ../VsDC3/processData.h \
#    vsdc3noiseofintegral.h \
#    vsdc3readosc.h \
#    vsdc3abserror.h \
    vsdc3_meas_header.h \
    ../VsDC3/vmecardbaseport.h \
    ../VsDC3/vsdccardtest.h \
    ../VsDC3/vsdc3v2.h \
    ../VsDC3/movingcoilsvmecardport.h# \
#    ../VsDC3/vme_cards_updater.h

INCLUDEPATH += \
    ../VsDC3
