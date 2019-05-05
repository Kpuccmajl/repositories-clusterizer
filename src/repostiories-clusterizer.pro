#-------------------------------------------------
#
# Project created by QtCreator 2019-04-15T07:55:14
#
#-------------------------------------------------

QT       += core gui printsupport
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = repostiories-clusterizer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    config.cpp \
    clusterizer.cpp \
    analyzer.cpp \
    qcustomplot.cpp \
    repository.cpp

HEADERS  += mainwindow.h \
    config.h \
    clusterizer.h \
    analyzer.h \
    qcustomplot.h \
    repository.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc

DISTFILES += \
    todo \
    ../config.xml \
    ../checksupport.sh \
    ../cppcheck.sh \
    ../sloc.sh \
    ../sloc-lang.sh \
    scripts/checksupport.sh \
    scripts/cppcheck.sh \
    scripts/sloc.sh \
    scripts/sloc-lang.sh
