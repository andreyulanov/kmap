/****************************************************************************
** Meta object code from reading C++ file 'kobject.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../lib/kobject.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kobject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KObjectManager_t {
    QByteArrayData data[14];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KObjectManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KObjectManager_t qt_meta_stringdata_KObjectManager = {
    {
QT_MOC_LITERAL(0, 0, 14), // "KObjectManager"
QT_MOC_LITERAL(1, 15, 7), // "deg2pix"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "KGeoCoor"
QT_MOC_LITERAL(4, 33, 7), // "deg2scr"
QT_MOC_LITERAL(5, 41, 7), // "scr2deg"
QT_MOC_LITERAL(6, 49, 7), // "updated"
QT_MOC_LITERAL(7, 57, 9), // "startEdit"
QT_MOC_LITERAL(8, 67, 10), // "finishEdit"
QT_MOC_LITERAL(9, 78, 5), // "saved"
QT_MOC_LITERAL(10, 84, 21), // "loadFileWithoutUpdate"
QT_MOC_LITERAL(11, 106, 9), // "QFileInfo"
QT_MOC_LITERAL(12, 116, 9), // "file_info"
QT_MOC_LITERAL(13, 126, 4) // "path"

    },
    "KObjectManager\0deg2pix\0\0KGeoCoor\0"
    "deg2scr\0scr2deg\0updated\0startEdit\0"
    "finishEdit\0saved\0loadFileWithoutUpdate\0"
    "QFileInfo\0file_info\0path"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KObjectManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       5,    1,   65,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    0,   69,    2, 0x06 /* Public */,
       8,    0,   70,    2, 0x06 /* Public */,
       9,    1,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   74,    2, 0x08 /* Private */,
      10,    1,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::QPoint, 0x80000000 | 3,    2,
    QMetaType::QPoint, 0x80000000 | 3,    2,
    0x80000000 | 3, QMetaType::QPoint,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::QString,   13,

       0        // eod
};

void KObjectManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KObjectManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { QPoint _r = _t->deg2pix((*reinterpret_cast< KGeoCoor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QPoint*>(_a[0]) = std::move(_r); }  break;
        case 1: { QPoint _r = _t->deg2scr((*reinterpret_cast< KGeoCoor(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QPoint*>(_a[0]) = std::move(_r); }  break;
        case 2: { KGeoCoor _r = _t->scr2deg((*reinterpret_cast< QPoint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< KGeoCoor*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->updated(); break;
        case 4: _t->startEdit(); break;
        case 5: _t->finishEdit(); break;
        case 6: _t->saved((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->loadFileWithoutUpdate((*reinterpret_cast< QFileInfo(*)>(_a[1]))); break;
        case 8: _t->loadFileWithoutUpdate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QFileInfo >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = QPoint (KObjectManager::*)(KGeoCoor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::deg2pix)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = QPoint (KObjectManager::*)(KGeoCoor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::deg2scr)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = KGeoCoor (KObjectManager::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::scr2deg)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KObjectManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::updated)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KObjectManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::startEdit)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (KObjectManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::finishEdit)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (KObjectManager::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KObjectManager::saved)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KObjectManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KObjectManager.data,
    qt_meta_data_KObjectManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KObjectManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KObjectManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KObjectManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KObjectManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
QPoint KObjectManager::deg2pix(KGeoCoor _t1)
{
    QPoint _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
QPoint KObjectManager::deg2scr(KGeoCoor _t1)
{
    QPoint _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}

// SIGNAL 2
KGeoCoor KObjectManager::scr2deg(QPoint _t1)
{
    KGeoCoor _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
    return _t0;
}

// SIGNAL 3
void KObjectManager::updated()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void KObjectManager::startEdit()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void KObjectManager::finishEdit()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void KObjectManager::saved(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
