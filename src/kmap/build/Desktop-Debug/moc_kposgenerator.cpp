/****************************************************************************
** Meta object code from reading C++ file 'kposgenerator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../kposgenerator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kposgenerator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KPosGenerator_t {
    QByteArrayData data[8];
    char stringdata0[83];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KPosGenerator_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KPosGenerator_t qt_meta_stringdata_KPosGenerator = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KPosGenerator"
QT_MOC_LITERAL(1, 14, 13), // "generated_pos"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 9), // "KPosition"
QT_MOC_LITERAL(4, 39, 14), // "generated_coor"
QT_MOC_LITERAL(5, 54, 8), // "KGeoCoor"
QT_MOC_LITERAL(6, 63, 15), // "generated_angle"
QT_MOC_LITERAL(7, 79, 3) // "deg"

    },
    "KPosGenerator\0generated_pos\0\0KPosition\0"
    "generated_coor\0KGeoCoor\0generated_angle\0"
    "deg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KPosGenerator[] = {

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
       4,    1,   32,    2, 0x06 /* Public */,
       6,    1,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, QMetaType::Double,    7,

       0        // eod
};

void KPosGenerator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KPosGenerator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->generated_pos((*reinterpret_cast< KPosition(*)>(_a[1]))); break;
        case 1: _t->generated_coor((*reinterpret_cast< KGeoCoor(*)>(_a[1]))); break;
        case 2: _t->generated_angle((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KPosGenerator::*)(KPosition );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KPosGenerator::generated_pos)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KPosGenerator::*)(KGeoCoor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KPosGenerator::generated_coor)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KPosGenerator::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KPosGenerator::generated_angle)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KPosGenerator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KPosGenerator.data,
    qt_meta_data_KPosGenerator,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KPosGenerator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KPosGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KPosGenerator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KPosGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void KPosGenerator::generated_pos(KPosition _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KPosGenerator::generated_coor(KGeoCoor _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KPosGenerator::generated_angle(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
