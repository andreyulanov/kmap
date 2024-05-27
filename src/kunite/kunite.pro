QMAKE_CXXFLAGS += -std=c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion

INCLUDEPATH += ../lib
SOURCES += \
    ../lib/kbase.cpp \
    ../lib/klocker.cpp \
    ../lib/kpack.cpp \
    ../lib/kclass.cpp \
    ../lib/kclassmanager.cpp \
    main.cpp

HEADERS += \
 ../lib/kbase.h \
    ../lib/klocker.h \
    ../lib/kpack.h \
    ../lib/kserialize.h \
    ../lib/kclass.h \
    ../lib/kclassmanager.cpp



