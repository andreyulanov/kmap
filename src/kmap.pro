TEMPLATE = subdirs

android: SUBDIRS += knav
else: SUBDIRS += osm2kmap kunite knav #pan2kmap
# pan2kmap закомментирован чтобы у меня не падала сборка без библиотеки от КБ панорама

HEADERS +=

SOURCES +=

CONFIG += qt

QT += widgets
