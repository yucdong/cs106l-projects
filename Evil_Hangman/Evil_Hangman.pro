TEMPLATE = app
CONFIG += console
CONFIG += C++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    Game.cpp \
    Utils.cpp

HEADERS += \
    Utils.h \
    Game.h

OTHER_FILES += \
    dictionary.txt

