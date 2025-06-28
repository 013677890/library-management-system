QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

include($$PWD/cell/cell.pri)
include($$PWD/lib/lib.pri)

DESTDIR=$$PWD/../bin
DISTFILES +=

RESOURCES += \
    qrc.qrc
RC_FILE += $$PWD/qrcpicture/app.rc
include($$PWD/QXlsx/QXlsx.pri)             # QXlsx源代码
INCLUDEPATH += $$PWD/QXlsx
