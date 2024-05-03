/****************************************************************************
** Meta object code from reading C++ file 'kcontrols.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../kcontrols.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kcontrols.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KControls_t {
    QByteArrayData data[13];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KControls_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KControls_t qt_meta_stringdata_KControls = {
    {
QT_MOC_LITERAL(0, 0, 9), // "KControls"
QT_MOC_LITERAL(1, 10, 6), // "zoomIn"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 7), // "zoomOut"
QT_MOC_LITERAL(4, 26, 12), // "zoomReleased"
QT_MOC_LITERAL(5, 39, 8), // "scrollTo"
QT_MOC_LITERAL(6, 48, 8), // "KGeoCoor"
QT_MOC_LITERAL(7, 57, 15), // "switchRecording"
QT_MOC_LITERAL(8, 73, 11), // "isRecording"
QT_MOC_LITERAL(9, 85, 11), // "selectShape"
QT_MOC_LITERAL(10, 97, 12), // "acceptObject"
QT_MOC_LITERAL(11, 110, 5), // "login"
QT_MOC_LITERAL(12, 116, 12) // "removeObject"

    },
    "KControls\0zoomIn\0\0zoomOut\0zoomReleased\0"
    "scrollTo\0KGeoCoor\0switchRecording\0"
    "isRecording\0selectShape\0acceptObject\0"
    "login\0removeObject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KControls[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    1,   67,    2, 0x06 /* Public */,
       7,    0,   70,    2, 0x06 /* Public */,
       8,    0,   71,    2, 0x06 /* Public */,
       9,    0,   72,    2, 0x06 /* Public */,
      10,    0,   73,    2, 0x06 /* Public */,
      11,    0,   74,    2, 0x06 /* Public */,
      12,    0,   75,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KControls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KControls *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->zoomIn(); break;
        case 1: _t->zoomOut(); break;
        case 2: _t->zoomReleased(); break;
        case 3: _t->scrollTo((*reinterpret_cast< const KGeoCoor(*)>(_a[1]))); break;
        case 4: _t->switchRecording(); break;
        case 5: { bool _r = _t->isRecording();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->selectShape(); break;
        case 7: _t->acceptObject(); break;
        case 8: _t->login(); break;
        case 9: _t->removeObject(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::zoomIn)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::zoomOut)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::zoomReleased)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KControls::*)(const KGeoCoor & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::scrollTo)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::switchRecording)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = bool (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::isRecording)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::selectShape)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::acceptObject)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::login)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (KControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KControls::removeObject)) {
                *result = 9;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KControls::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KControls.data,
    qt_meta_data_KControls,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KControls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KControls::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KControls.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KControls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void KControls::zoomIn()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KControls::zoomOut()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KControls::zoomReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void KControls::scrollTo(const KGeoCoor & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KControls::switchRecording()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
bool KControls::isRecording()
{
    bool _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
    return _t0;
}

// SIGNAL 6
void KControls::selectShape()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void KControls::acceptObject()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void KControls::login()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void KControls::removeObject()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
