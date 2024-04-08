linux-buildroot-g++: QT += core gui widgets
else:QT += core gui widgets positioning sensors
CONFIG += c++2a
QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion

!linux-buildroot-g++: DEFINES += BUILD_WITH_SENSORS

SOURCES += \
 ../lib/kdatetime.cpp \
../lib/klocker.cpp \
../lib/kmap.cpp \
 ../lib/kportableobject.cpp \
../lib/krender.cpp \
../lib/kshape.cpp \
kautoscroll.cpp \
kcontrols.cpp \
 keditwidget.cpp \
 kfindwidget.cpp \
    kloginwidget.cpp \
 kmapwidget.cpp \
 knewobjectwidget.cpp \
    kportableobjectsender.cpp \
 kposgenerator.cpp \
    krosterwidget.cpp \
 kswitch.cpp \
 ktrackmanager.cpp \
 kxmppclient.cpp \
main.cpp

HEADERS += \
 ../lib/kdatetime.h \
../lib/klocker.h \
../lib/kmap.h \
 ../lib/kportableobject.h \
../lib/krender.h \
../lib/kshape.h \
kautoscroll.h \
kcontrols.h \
 keditwidget.h \
 kfindwidget.h \
    kloginwidget.h \
 kmapwidget.h \
 knewobjectwidget.h \
    kportableobjectsender.h \
 kposgenerator.h \
    krosterwidget.h \
 kswitch.h \
 ktrackmanager.h \
 kxmppclient.h

!linux-buildroot-g++: SOURCES += \
    kheading.cpp \
    kpositionlabel.cpp \

!linux-buildroot-g++: HEADERS += \
    kheading.h \
    kpositionlabel.h \

INCLUDEPATH += ../lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
 android/AndroidManifest.xml \
 android/build.gradle \
 android/gradle.properties \
 android/gradle/wrapper/gradle-wrapper.jar \
 android/gradle/wrapper/gradle-wrapper.properties \
 android/gradlew \
 android/gradlew.bat \
 android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
 images.qrc

unix|win32: LIBS += -lqxmpp
# for qxmpp
QT += network
