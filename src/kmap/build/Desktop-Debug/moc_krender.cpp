/****************************************************************************
** Meta object code from reading C++ file 'krender.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../lib/krender.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'krender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRender_t {
    QByteArrayData data[8];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRender_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRender_t qt_meta_stringdata_KRender = {
    {
QT_MOC_LITERAL(0, 0, 7), // "KRender"
QT_MOC_LITERAL(1, 8, 7), // "started"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 16), // "paintUserObjects"
QT_MOC_LITERAL(4, 34, 9), // "QPainter*"
QT_MOC_LITERAL(5, 44, 1), // "p"
QT_MOC_LITERAL(6, 46, 8), // "rendered"
QT_MOC_LITERAL(7, 55, 10) // "ms_elapsed"

    },
    "KRender\0started\0\0paintUserObjects\0"
    "QPainter*\0p\0rendered\0ms_elapsed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRender[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    1,   32,    2, 0x06 /* Public */,
       6,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QRectF,    2,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void KRender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KRender *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->started((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 1: _t->paintUserObjects((*reinterpret_cast< QPainter*(*)>(_a[1]))); break;
        case 2: _t->rendered((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KRender::*)(QRectF );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRender::started)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KRender::*)(QPainter * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRender::paintUserObjects)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KRender::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRender::rendered)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRender::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_KRender.data,
    qt_meta_data_KRender,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRender::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRender.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int KRender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void KRender::started(QRectF _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KRender::paintUserObjects(QPainter * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KRender::rendered(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
