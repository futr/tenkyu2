#-------------------------------------------------
#
# Project created by QtCreator 2018-05-18T15:00:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32-msvc*: {
    message( "MSVC!" )
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}

RC_ICONS = icon.ico
RC_CODEPAGE = 65001
TRANSLATIONS = tenkyu_ja.ts

TARGET = tenkyu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        celestialsphereprinter.cpp \
        colorselectbutton.cpp \
        main.cpp \
        starmap.cpp \
        widget.cpp

HEADERS += \
        celestialsphereprinter.h \
        colorselectbutton.h \
        starmap.h \
        widget.h

FORMS += \
        widget.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    sofa/20210125_a/c/src/makefile
