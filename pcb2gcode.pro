
QT += core gui serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Name of the project
TARGET = FPTApplication
TEMPLATE = app

# Source, header, and UI file locations
SOURCES += \
    src/GerberFileManager.cpp \
    src/gcodeConverter.cpp \
    src/main.cpp \
    src/pcb2gcode.cpp \
    src/settings.cpp \
    src/uart.cpp

HEADERS += \
    include/GerberFileManager.h \
    include/pcb2gcode.h \
    include/settings.h \
    include/uart.h \
    include/gcodeConverter.h

FORMS += \
    ui/PCB2Gcode.ui

TRANSLATIONS += \
    translations/PCB2Gcode_en_US.ts

DISTFILES +=
