QT += core
QT -= gui

TARGET = test3
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.c \
    compression.c

LIBS += /home/guest/senseThink/zlib-1.2.8/libz.a
LIBS += /home/guest/senseThink/openssl-1.0.2h/libcrypto.a

INCLUDEPATH += /home/guest/senseThink/zlib-1.2.8/libz.a
INCLUDEPATH += /home/guest/senseThink/openssl-1.0.2h/libcrypto.a

HEADERS += \
    compression.h
