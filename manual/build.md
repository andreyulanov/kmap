# Build {#build}
### Build kmap 
You need <a href="https://doc.qt.io/qt-5/get-and-install-qt.html">Qt5.15</a> and <a href="https://github.com/qxmpp-project/qxmpp">qxmpp</a> library installed. 
```
mkdir build 
cd build
qmake ../src/kmap
make
```
### Build whole project
You additionly need closed panorama library (<a href="https://www.gisinfo.net/download/download_linux.htm">en</a>,
<a href="https://gisinfo.ru/download/doc_linux.htm">ru</a>) to build entire project.
```
mkdir build 
cd build
qmake ../src
make
```

