TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    registers.c \
    commands.c \
    asm_formats.c \
    str_func.c \
    uasm.c \
    symbols.c \
    collect_data.c \
    assemble.c

HEADERS += \
    registers.h \
    commands.h \
    asm_formats.h \
    str_func.h \
    symbols.h \
    uasm.h \
    collect_data.h \
    assemble.h

QMAKE_CC = gcc

QMAKE_CFLAGS += -std=gnu99 -Wall

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += glib-2.0
