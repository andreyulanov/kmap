/****************************************************************************
** Meta object code from reading C++ file 'kportableobjectsender.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../kportableobjectsender.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kportableobjectsender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KPortableObjectSender_t {
    QByteArrayData data[14];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KPortableObjectSender_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KPortableObjectSender_t qt_meta_stringdata_KPortableObjectSender = {
    {
QT_MOC_LITERAL(0, 0, 21), // "KPortableObjectSender"
QT_MOC_LITERAL(1, 22, 4), // "send"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 3), // "jid"
QT_MOC_LITERAL(4, 32, 8), // "filePath"
QT_MOC_LITERAL(5, 41, 11), // "description"
QT_MOC_LITERAL(6, 53, 17), // "turnOnSendOnReady"
QT_MOC_LITERAL(7, 71, 18), // "turnOffSendOnReady"
QT_MOC_LITERAL(8, 90, 6), // "setJid"
QT_MOC_LITERAL(9, 97, 14), // "QList<QString>"
QT_MOC_LITERAL(10, 112, 4), // "jids"
QT_MOC_LITERAL(11, 117, 11), // "setFilename"
QT_MOC_LITERAL(12, 129, 5), // "clear"
QT_MOC_LITERAL(13, 135, 11) // "sendIfReady"

    },
    "KPortableObjectSender\0send\0\0jid\0"
    "filePath\0description\0turnOnSendOnReady\0"
    "turnOffSendOnReady\0setJid\0QList<QString>\0"
    "jids\0setFilename\0clear\0sendIfReady"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KPortableObjectSender[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   56,    2, 0x0a /* Public */,
       7,    0,   57,    2, 0x0a /* Public */,
       8,    1,   58,    2, 0x0a /* Public */,
      11,    1,   61,    2, 0x0a /* Public */,
      12,    0,   64,    2, 0x0a /* Public */,
      13,    0,   65,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Bool,

       0        // eod
};

void KPortableObjectSender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KPortableObjectSender *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->send((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: _t->turnOnSendOnReady(); break;
        case 2: _t->turnOffSendOnReady(); break;
        case 3: _t->setJid((*reinterpret_cast< QList<QString>(*)>(_a[1]))); break;
        case 4: _t->setFilename((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->clear(); break;
        case 6: { bool _r = _t->sendIfReady();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
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
            using _t = void (KPortableObjectSender::*)(QString , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KPortableObjectSender::send)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KPortableObjectSender::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KPortableObjectSender.data,
    qt_meta_data_KPortableObjectSender,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KPortableObjectSender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KPortableObjectSender::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KPortableObjectSender.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KPortableObjectSender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void KPortableObjectSender::send(QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
