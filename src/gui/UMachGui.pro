#-------------------------------------------------
#
# Project created by QtCreator 2012-10-29T10:40:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UMachGui
TEMPLATE = app



SOURCES += main.cpp\
        umachgui.cpp \
    umachregisters.cpp \
    umachbreakpoints.cpp \
    umachcodeeditor.cpp \
    uasmfile.cpp \
    project.cpp \
    umachmake.cpp \
    umachdebuginfo.cpp \
    umachsharedmemory.cpp

HEADERS  += umachgui.h \
    umachregisters.h \
    umachbreakpoints.h \
    umachcodeeditor.h \
    icodeeditor.h \
    IUasmFile.h \
    uasmfile.h \
    project.h \
    umachmake.h \
    umachdebuginfo.h \
    umachsharedmemory.h \
    ../gui_core/sharedmemorystructs.h

FORMS    +=

RESOURCES += \
    icons.qrc

OTHER_FILES +=
