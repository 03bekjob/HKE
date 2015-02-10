#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T17:56:00
#
#-------------------------------------------------

QT       += core gui\
#            printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets\
                                        printsupport \
                                        sql

TARGET = HKE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wgtgennum.cpp \
    hketablemodel.cpp \
    mdiarea.cpp \
    wgtshowtable.cpp

HEADERS  += mainwindow.h \
    wgtgennum.h \
    hketablemodel.h \
    seasonTicket.h \
    global.h \
    mdiarea.h \
    wgtshowtable.h

RESOURCES += \
    res.qrc

FORMS += \
    wgtgennum.ui \
    wgtshowtable.ui
