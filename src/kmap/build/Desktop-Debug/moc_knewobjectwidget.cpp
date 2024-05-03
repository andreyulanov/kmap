/****************************************************************************
** Meta object code from reading C++ file 'knewobjectwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../knewobjectwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'knewobjectwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KNewObjectWidget_t {
    QByteArrayData data[9];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KNewObjectWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KNewObjectWidget_t qt_meta_stringdata_KNewObjectWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "KNewObjectWidget"
QT_MOC_LITERAL(1, 17, 21), // "getUserShapeImageList"
QT_MOC_LITERAL(2, 39, 15), // "KShapeImageList"
QT_MOC_LITERAL(3, 55, 0), // ""
QT_MOC_LITERAL(4, 56, 12), // "getShapeById"
QT_MOC_LITERAL(5, 69, 6), // "KShape"
QT_MOC_LITERAL(6, 76, 13), // "selectedShape"
QT_MOC_LITERAL(7, 90, 10), // "sendObject"
QT_MOC_LITERAL(8, 101, 15) // "doNotSendObject"

    },
    "KNewObjectWidget\0getUserShapeImageList\0"
    "KShapeImageList\0\0getShapeById\0KShape\0"
    "selectedShape\0sendObject\0doNotSendObject"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KNewObjectWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    3, 0x06 /* Public */,
       4,    1,   40,    3, 0x06 /* Public */,
       6,    1,   43,    3, 0x06 /* Public */,
       7,    0,   46,    3, 0x06 /* Public */,
       8,    0,   47,    3, 0x06 /* Public */,

 // signals: parameters
    0x80000000 | 2,
    0x80000000 | 5, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KNewObjectWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KNewObjectWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { KShapeImageList _r = _t->getUserShapeImageList();
            if (_a[0]) *reinterpret_cast< KShapeImageList*>(_a[0]) = std::move(_r); }  break;
        case 1: { KShape _r = _t->getShapeById((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< KShape*>(_a[0]) = std::move(_r); }  break;
        case 2: _t->selectedShape((*reinterpret_cast< KShape(*)>(_a[1]))); break;
        case 3: _t->sendObject(); break;
        case 4: _t->doNotSendObject(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = KShapeImageList (KNewObjectWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KNewObjectWidget::getUserShapeImageList)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = KShape (KNewObjectWidget::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KNewObjectWidget::getShapeById)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KNewObjectWidget::*)(KShape );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KNewObjectWidget::selectedShape)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KNewObjectWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KNewObjectWidget::sendObject)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KNewObjectWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KNewObjectWidget::doNotSendObject)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KNewObjectWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KNewObjectWidget.data,
    qt_meta_data_KNewObjectWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KNewObjectWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KNewObjectWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KNewObjectWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KNewObjectWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
KShapeImageList KNewObjectWidget::getUserShapeImageList()
{
    KShapeImageList _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
KShape KNewObjectWidget::getShapeById(QString _t1)
{
    KShape _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}

// SIGNAL 2
void KNewObjectWidget::selectedShape(KShape _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KNewObjectWidget::sendObject()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void KNewObjectWidget::doNotSendObject()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
