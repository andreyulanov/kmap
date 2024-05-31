QT       += widgets
QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion


INCLUDEPATH += /home/user/gisdesigner/14/include
INCLUDEPATH += ../lib
LIBS += -lmapcomponents -lqdmapacces
SOURCES += \
    ../lib/kbase.cpp \
    ../lib/klocker.cpp \
    ../lib/kpack.cpp \
    ../lib/krenderpack.cpp \
    ../lib/kclass.cpp \
    ../lib/kclassmanager.cpp \
    kpanclassmanager.cpp \
    main.cpp

HEADERS += \
 ../lib/kbase.h \
    ../lib/klocker.h \
    ../lib/kpack.h \
    ../lib/krenderpack.h \
    ../lib/kserialize.h \
    ../lib/kclass.h \
    ../lib/kclassmanager.h \
 kpanclass.h \
 kpanclassmanager.h



