TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    encrypt.c \
    compress.c

INCLUDEPATH += /usr/local/ssl/include
LIBS += /usr/local/ssl/lib/libcrypto.a
LIBS += /usr/local/ssl/lib/libssl.a

INCLUDEPATH += /usr/local/lib
LIBS += /usr/local/lib/libz.a

HEADERS += \
    encrypt.h \
    compress.h
