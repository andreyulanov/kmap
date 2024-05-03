/****************************************************************************
** Meta object code from reading C++ file 'kxmppclient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../kxmppclient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kxmppclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KXmppClient_t {
    QByteArrayData data[14];
    char stringdata0[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KXmppClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KXmppClient_t qt_meta_stringdata_KXmppClient = {
    {
QT_MOC_LITERAL(0, 0, 11), // "KXmppClient"
QT_MOC_LITERAL(1, 12, 14), // "needConnection"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 14), // "fileDownloaded"
QT_MOC_LITERAL(4, 43, 4), // "path"
QT_MOC_LITERAL(5, 48, 8), // "sendFile"
QT_MOC_LITERAL(6, 57, 3), // "jid"
QT_MOC_LITERAL(7, 61, 8), // "filePath"
QT_MOC_LITERAL(8, 70, 11), // "description"
QT_MOC_LITERAL(9, 82, 17), // "reconnectToServer"
QT_MOC_LITERAL(10, 100, 8), // "password"
QT_MOC_LITERAL(11, 109, 16), // "slotFileReceived"
QT_MOC_LITERAL(12, 126, 17), // "QXmppTransferJob*"
QT_MOC_LITERAL(13, 144, 3) // "job"

    },
    "KXmppClient\0needConnection\0\0fileDownloaded\0"
    "path\0sendFile\0jid\0filePath\0description\0"
    "reconnectToServer\0password\0slotFileReceived\0"
    "QXmppTransferJob*\0job"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KXmppClient[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    3,   48,    2, 0x0a /* Public */,
       5,    2,   55,    2, 0x2a /* Public | MethodCloned */,
       9,    2,   60,    2, 0x0a /* Public */,
      11,    1,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,   10,
    QMetaType::Void, 0x80000000 | 12,   13,

       0        // eod
};

void KXmppClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KXmppClient *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->needConnection(); break;
        case 1: _t->fileDownloaded((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->sendFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->sendFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->reconnectToServer((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->slotFileReceived((*reinterpret_cast< QXmppTransferJob*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QXmppTransferJob* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KXmppClient::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KXmppClient::needConnection)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KXmppClient::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KXmppClient::fileDownloaded)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KXmppClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QXmppClient::staticMetaObject>(),
    qt_meta_stringdata_KXmppClient.data,
    qt_meta_data_KXmppClient,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KXmppClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KXmppClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KXmppClient.stringdata0))
        return static_cast<void*>(this);
    return QXmppClient::qt_metacast(_clname);
}

int KXmppClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QXmppClient::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void KXmppClient::needConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void KXmppClient::fileDownloaded(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_KXmppObjectReceiver_t {
    QByteArrayData data[15];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KXmppObjectReceiver_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KXmppObjectReceiver_t qt_meta_stringdata_KXmppObjectReceiver = {
    {
QT_MOC_LITERAL(0, 0, 19), // "KXmppObjectReceiver"
QT_MOC_LITERAL(1, 20, 19), // "finishedSucessfully"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 4), // "path"
QT_MOC_LITERAL(4, 46, 6), // "failed"
QT_MOC_LITERAL(5, 53, 12), // "slotFinished"
QT_MOC_LITERAL(6, 66, 12), // "slotProgress"
QT_MOC_LITERAL(7, 79, 4), // "done"
QT_MOC_LITERAL(8, 84, 5), // "total"
QT_MOC_LITERAL(9, 90, 9), // "slotState"
QT_MOC_LITERAL(10, 100, 23), // "QXmppTransferJob::State"
QT_MOC_LITERAL(11, 124, 5), // "state"
QT_MOC_LITERAL(12, 130, 9), // "slotError"
QT_MOC_LITERAL(13, 140, 23), // "QXmppTransferJob::Error"
QT_MOC_LITERAL(14, 164, 5) // "error"

    },
    "KXmppObjectReceiver\0finishedSucessfully\0"
    "\0path\0failed\0slotFinished\0slotProgress\0"
    "done\0total\0slotState\0QXmppTransferJob::State\0"
    "state\0slotError\0QXmppTransferJob::Error\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KXmppObjectReceiver[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   48,    2, 0x08 /* Private */,
       6,    2,   49,    2, 0x08 /* Private */,
       9,    1,   54,    2, 0x08 /* Private */,
      12,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 13,   14,

       0        // eod
};

void KXmppObjectReceiver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KXmppObjectReceiver *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->finishedSucessfully((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->failed(); break;
        case 2: _t->slotFinished(); break;
        case 3: _t->slotProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: _t->slotState((*reinterpret_cast< QXmppTransferJob::State(*)>(_a[1]))); break;
        case 5: _t->slotError((*reinterpret_cast< QXmppTransferJob::Error(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KXmppObjectReceiver::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KXmppObjectReceiver::finishedSucessfully)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KXmppObjectReceiver::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KXmppObjectReceiver::failed)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KXmppObjectReceiver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_KXmppObjectReceiver.data,
    qt_meta_data_KXmppObjectReceiver,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KXmppObjectReceiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KXmppObjectReceiver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KXmppObjectReceiver.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int KXmppObjectReceiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void KXmppObjectReceiver::finishedSucessfully(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KXmppObjectReceiver::failed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
