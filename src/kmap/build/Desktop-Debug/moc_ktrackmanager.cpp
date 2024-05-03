/****************************************************************************
** Meta object code from reading C++ file 'ktrackmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ktrackmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ktrackmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KTrackManager_t {
    QByteArrayData data[6];
    char stringdata0[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KTrackManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KTrackManager_t qt_meta_stringdata_KTrackManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KTrackManager"
QT_MOC_LITERAL(1, 14, 7), // "deg2pix"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 8), // "KGeoCoor"
QT_MOC_LITERAL(4, 32, 7), // "updated"
QT_MOC_LITERAL(5, 40, 15) // "switchRecording"

    },
    "KTrackManager\0deg2pix\0\0KGeoCoor\0updated\0"
    "switchRecording"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KTrackManager[] = {

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
       4,    0,   32,    2, 0x06 /* Public */,
       5,    0,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::QPoint, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KTrackManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KTrackManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QPoint _r = _t->deg2pix((*reinterpret_cast< KGeoCoor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QPoint*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->updated(); break;
        case 2: _t->switchRecording(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = QPoint (KTrackManager::*)(KGeoCoor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KTrackManager::deg2pix)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KTrackManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KTrackManager::updated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KTrackManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KTrackManager::switchRecording)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KTrackManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KTrackManager.data,
    qt_meta_data_KTrackManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KTrackManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KTrackManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KTrackManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KTrackManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QPoint KTrackManager::deg2pix(KGeoCoor _t1)
{
    QPoint _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
void KTrackManager::updated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void KTrackManager::switchRecording()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
