/****************************************************************************
** Meta object code from reading C++ file 'krosterwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../krosterwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'krosterwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KRosterWidget_t {
    QByteArrayData data[13];
    char stringdata0[141];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KRosterWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KRosterWidget_t qt_meta_stringdata_KRosterWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "KRosterWidget"
QT_MOC_LITERAL(1, 14, 11), // "jidSelected"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "QList<QString>"
QT_MOC_LITERAL(4, 42, 4), // "Jids"
QT_MOC_LITERAL(5, 47, 6), // "cancel"
QT_MOC_LITERAL(6, 54, 10), // "initialise"
QT_MOC_LITERAL(7, 65, 14), // "changePresence"
QT_MOC_LITERAL(8, 80, 7), // "bareJid"
QT_MOC_LITERAL(9, 88, 8), // "resource"
QT_MOC_LITERAL(10, 97, 18), // "submitButtonPushed"
QT_MOC_LITERAL(11, 116, 18), // "cancelButtonPushed"
QT_MOC_LITERAL(12, 135, 5) // "clear"

    },
    "KRosterWidget\0jidSelected\0\0QList<QString>\0"
    "Jids\0cancel\0initialise\0changePresence\0"
    "bareJid\0resource\0submitButtonPushed\0"
    "cancelButtonPushed\0clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KRosterWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   53,    2, 0x0a /* Public */,
       7,    2,   54,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,
      11,    0,   60,    2, 0x0a /* Public */,
      12,    0,   61,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KRosterWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KRosterWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->jidSelected((*reinterpret_cast< QList<QString>(*)>(_a[1]))); break;
        case 1: _t->cancel(); break;
        case 2: _t->initialise(); break;
        case 3: _t->changePresence((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->submitButtonPushed(); break;
        case 5: _t->cancelButtonPushed(); break;
        case 6: _t->clear(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QString> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KRosterWidget::*)(QList<QString> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRosterWidget::jidSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KRosterWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KRosterWidget::cancel)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KRosterWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KRosterWidget.data,
    qt_meta_data_KRosterWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KRosterWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KRosterWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KRosterWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KRosterWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void KRosterWidget::jidSelected(QList<QString> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KRosterWidget::cancel()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
