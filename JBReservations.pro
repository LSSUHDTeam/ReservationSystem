#-------------------------------------------------
#
# Project created by QtCreator 2014-11-19T13:42:25
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JBReservations
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newreservation.cpp \
    structures.cpp \
    editreservation.cpp \
    socketclient.cpp \
    scheduler.cpp \
    noteswindow.cpp \
    addlocation.cpp \
    errwindow.cpp \
    pickupwindow.cpp \
    deliverywindow.cpp \
    reservationviewer.cpp \
    jobalert.cpp \
    adminlogin.cpp \
    adminpanel.cpp \
    adminconflicthandler.cpp \
    logwriter.cpp

HEADERS  += mainwindow.h \
    newreservation.h \
    structures.h \
    editreservation.h \
    socketclient.h \
    globals.h \
    scheduler.h \
    noteswindow.h \
    addlocation.h \
    errwindow.h \
    pickupwindow.h \
    deliverywindow.h \
    reservationviewer.h \
    jobalert.h \
    adminlogin.h \
    adminpanel.h \
    adminconflicthandler.h \
    logwriter.h

FORMS    += mainwindow.ui \
    newreservation.ui \
    editreservation.ui \
    editwindow.ui \
    noteswindow.ui \
    addlocation.ui \
    errwindow.ui \
    pickupwindow.ui \
    deliverywindow.ui \
    reservationviewer.ui \
    jobalert.ui \
    adminlogin.ui \
    adminpanel.ui \
    adminconflicthandler.ui

RESOURCES += \
    Alerts.qrc \
    Icons.qrc
