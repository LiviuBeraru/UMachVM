#-------------------------------------------------
#
# Project created by QtCreator 2012-11-04T14:00:36
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = UMachCore
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../vm/system.c \
    ../vm/subroutine.c \
    ../vm/strings.c \
    ../vm/registers.c \
    ../vm/options.c \
    ../vm/memory.c \
    ../vm/logmsg.c \
    ../vm/logic.c \
    ../vm/loadstore.c \
    ../vm/io.c \
    ../vm/disassemble.c \
    ../vm/debugger.c \
    ../vm/core.c \
    ../vm/controll.c \
    ../vm/compare.c \
    ../vm/command.c \
    ../vm/branch.c \
    ../vm/arithm.c \
    umachcore.cpp

HEADERS += \
    ../vm/system.h \
    ../vm/subroutine.h \
    ../vm/strings.h \
    ../vm/registers.h \
    ../vm/options.h \
    ../vm/memory.h \
    ../vm/logmsg.h \
    ../vm/logic.h \
    ../vm/loadstore.h \
    ../vm/io.h \
    ../vm/interrupts.h \
    ../vm/disassemble.h \
    ../vm/debugger.h \
    ../vm/core.h \
    ../vm/controll.h \
    ../vm/compare.h \
    ../vm/command.h \
    ../vm/branch.h \
    ../vm/arithm.h \
    umachcore.h \
    sharedmemorystructs.h
