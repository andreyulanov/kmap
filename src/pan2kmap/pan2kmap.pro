QT       += widgets
QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion


INCLUDEPATH += /home/user/gisdesigner/14/include
INCLUDEPATH += ../lib
LIBS += -lmapcomponents -lqdmapacces
SOURCES += \
    ../lib/klocker.cpp \
    ../lib/kmap.cpp \
    ../lib/kshape.cpp \
    main.cpp

HEADERS += \
    ../lib/klocker.h \
    ../lib/kmap.h \
    ../lib/kserialize.h \
    ../lib/kshape.h \



