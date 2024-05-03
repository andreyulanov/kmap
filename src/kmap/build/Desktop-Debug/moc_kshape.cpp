/****************************************************************************
** Meta object code from reading C++ file 'kshape.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../lib/kshape.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kshape.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KShape_t {
    QByteArrayData data[15];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KShape_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KShape_t qt_meta_stringdata_KShape = {
    {
QT_MOC_LITERAL(0, 0, 6), // "KShape"
QT_MOC_LITERAL(1, 7, 4), // "Type"
QT_MOC_LITERAL(2, 12, 4), // "None"
QT_MOC_LITERAL(3, 17, 5), // "Point"
QT_MOC_LITERAL(4, 23, 4), // "Line"
QT_MOC_LITERAL(5, 28, 7), // "Polygon"
QT_MOC_LITERAL(6, 36, 5), // "Style"
QT_MOC_LITERAL(7, 42, 5), // "Solid"
QT_MOC_LITERAL(8, 48, 4), // "Dash"
QT_MOC_LITERAL(9, 53, 5), // "Hatch"
QT_MOC_LITERAL(10, 59, 5), // "BDiag"
QT_MOC_LITERAL(11, 65, 5), // "FDiag"
QT_MOC_LITERAL(12, 71, 5), // "Horiz"
QT_MOC_LITERAL(13, 77, 4), // "Vert"
QT_MOC_LITERAL(14, 82, 4) // "Dots"

    },
    "KShape\0Type\0None\0Point\0Line\0Polygon\0"
    "Style\0Solid\0Dash\0Hatch\0BDiag\0FDiag\0"
    "Horiz\0Vert\0Dots"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KShape[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       2,   14, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    4,   24,
       6,    6, 0x0,    8,   32,

 // enum data: key, value
       2, uint(KShape::None),
       3, uint(KShape::Point),
       4, uint(KShape::Line),
       5, uint(KShape::Polygon),
       7, uint(KShape::Solid),
       8, uint(KShape::Dash),
       9, uint(KShape::Hatch),
      10, uint(KShape::BDiag),
      11, uint(KShape::FDiag),
      12, uint(KShape::Horiz),
      13, uint(KShape::Vert),
      14, uint(KShape::Dots),

       0        // eod
};

QT_INIT_METAOBJECT const QMetaObject KShape::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_KShape.data,
    qt_meta_data_KShape,
    nullptr,
    nullptr,
    nullptr
} };

struct qt_meta_stringdata_KShapeManager_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KShapeManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KShapeManager_t qt_meta_stringdata_KShapeManager = {
    {
QT_MOC_LITERAL(0, 0, 13) // "KShapeManager"

    },
    "KShapeManager"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KShapeManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void KShapeManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject KShapeManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KShapeManager.data,
    qt_meta_data_KShapeManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KShapeManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KShapeManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KShapeManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KShapeManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
