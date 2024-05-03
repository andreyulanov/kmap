/****************************************************************************
** Meta object code from reading C++ file 'krenderwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../krenderwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'krenderwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRenderWidget_t {
    QByteArrayData data[15];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRenderWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRenderWidget_t qt_meta_stringdata_KRenderWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KRenderWidget"
QT_MOC_LITERAL(1, 14, 8), // "modified"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 12), // "mousePressed"
QT_MOC_LITERAL(4, 37, 10), // "mouseMoved"
QT_MOC_LITERAL(5, 48, 13), // "mouseReleased"
QT_MOC_LITERAL(6, 62, 12), // "pinchStarted"
QT_MOC_LITERAL(7, 75, 12), // "zoomFinished"
QT_MOC_LITERAL(8, 88, 13), // "movedCenterTo"
QT_MOC_LITERAL(9, 102, 8), // "KGeoCoor"
QT_MOC_LITERAL(10, 111, 16), // "paintUserObjects"
QT_MOC_LITERAL(11, 128, 9), // "QPainter*"
QT_MOC_LITERAL(12, 138, 6), // "tapped"
QT_MOC_LITERAL(13, 145, 13), // "startedRender"
QT_MOC_LITERAL(14, 159, 9) // "canScroll"

    },
    "KRenderWidget\0modified\0\0mousePressed\0"
    "mouseMoved\0mouseReleased\0pinchStarted\0"
    "zoomFinished\0movedCenterTo\0KGeoCoor\0"
    "paintUserObjects\0QPainter*\0tapped\0"
    "startedRender\0canScroll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRenderWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    1,   70,    2, 0x06 /* Public */,
       4,    1,   73,    2, 0x06 /* Public */,
       5,    0,   76,    2, 0x06 /* Public */,
       6,    0,   77,    2, 0x06 /* Public */,
       7,    0,   78,    2, 0x06 /* Public */,
       8,    1,   79,    2, 0x06 /* Public */,
      10,    1,   82,    2, 0x06 /* Public */,
      12,    1,   85,    2, 0x06 /* Public */,
      13,    1,   88,    2, 0x06 /* Public */,
      14,    0,   91,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, QMetaType::QRectF,    2,
    QMetaType::Bool,

       0        // eod
};

void KRenderWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KRenderWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->modified(); break;
        case 1: _t->mousePressed((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 2: _t->mouseMoved((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->mouseReleased(); break;
        case 4: _t->pinchStarted(); break;
        case 5: _t->zoomFinished(); break;
        case 6: _t->movedCenterTo((*reinterpret_cast< const KGeoCoor(*)>(_a[1]))); break;
        case 7: _t->paintUserObjects((*reinterpret_cast< QPainter*(*)>(_a[1]))); break;
        case 8: _t->tapped((*reinterpret_cast< KGeoCoor(*)>(_a[1]))); break;
        case 9: _t->startedRender((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 10: { bool _r = _t->canScroll();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::modified)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::mousePressed)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::mouseMoved)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::mouseReleased)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::pinchStarted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::zoomFinished)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(const KGeoCoor & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::movedCenterTo)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(QPainter * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::paintUserObjects)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(KGeoCoor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::tapped)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (KRenderWidget::*)(QRectF );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::startedRender)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = bool (KRenderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRenderWidget::canScroll)) {
                *result = 10;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRenderWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRenderWidget.data,
    qt_meta_data_KRenderWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRenderWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRenderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRenderWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRenderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void KRenderWidget::modified()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KRenderWidget::mousePressed(QPoint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KRenderWidget::mouseMoved(QPoint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KRenderWidget::mouseReleased()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void KRenderWidget::pinchStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void KRenderWidget::zoomFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void KRenderWidget::movedCenterTo(const KGeoCoor & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KRenderWidget::paintUserObjects(QPainter * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KRenderWidget::tapped(KGeoCoor _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void KRenderWidget::startedRender(QRectF _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
bool KRenderWidget::canScroll()
{
    bool _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
    return _t0;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
