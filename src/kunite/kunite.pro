QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion

INCLUDEPATH += ../lib
SOURCES += \
    ../lib/klocker.cpp \
    ../lib/kpack.cpp \
    ../lib/kshape.cpp \
    main.cpp

HEADERS += \
    ../lib/klocker.h \
    ../lib/kpack.h \
    ../lib/kserialize.h \
    ../lib/kshape.h \



