TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    LFSortedList.c \
    Interlocked.c \
    LFSkipList.c

HEADERS += \
    LFSortedList.h \
    Interlocked.h \
    LFSkipList.h \
    Commons.h

LIBS += -lpthread
