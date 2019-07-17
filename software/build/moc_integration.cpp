/****************************************************************************
** Meta object code from reading C++ file 'integration.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/integrations/integration.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'integration.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Integration_t {
    QByteArrayData data[14];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Integration_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Integration_t qt_meta_stringdata_Integration = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Integration"
QT_MOC_LITERAL(1, 12, 20), // "integrationIdChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 9), // "connected"
QT_MOC_LITERAL(4, 44, 10), // "connecting"
QT_MOC_LITERAL(5, 55, 12), // "disconnected"
QT_MOC_LITERAL(6, 68, 5), // "state"
QT_MOC_LITERAL(7, 74, 6), // "states"
QT_MOC_LITERAL(8, 81, 13), // "integrationId"
QT_MOC_LITERAL(9, 95, 4), // "type"
QT_MOC_LITERAL(10, 100, 12), // "friendlyName"
QT_MOC_LITERAL(11, 113, 9), // "CONNECTED"
QT_MOC_LITERAL(12, 123, 10), // "CONNECTING"
QT_MOC_LITERAL(13, 134, 12) // "DISCONNECTED"

    },
    "Integration\0integrationIdChanged\0\0"
    "connected\0connecting\0disconnected\0"
    "state\0states\0integrationId\0type\0"
    "friendlyName\0CONNECTED\0CONNECTING\0"
    "DISCONNECTED"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Integration[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       4,   38, // properties
       1,   54, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       6, 0x80000000 | 7, 0x0009510b,
       8, QMetaType::Int, 0x00495103,
       9, QMetaType::QString, 0x00095103,
      10, QMetaType::QString, 0x00095103,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,

 // enums: name, flags, count, data
       7, 0x0,    3,   58,

 // enum data: key, value
      11, uint(Integration::CONNECTED),
      12, uint(Integration::CONNECTING),
      13, uint(Integration::DISCONNECTED),

       0        // eod
};

void Integration::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Integration *_t = static_cast<Integration *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->integrationIdChanged(); break;
        case 1: _t->connected(); break;
        case 2: _t->connecting(); break;
        case 3: _t->disconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Integration::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Integration::integrationIdChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Integration::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Integration::connected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Integration::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Integration::connecting)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Integration::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Integration::disconnected)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Integration *_t = static_cast<Integration *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< states*>(_v) = _t->state(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->integrationId(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->type(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->friendlyName(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Integration *_t = static_cast<Integration *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setState(*reinterpret_cast< states*>(_v)); break;
        case 1: _t->setIntegrationId(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setType(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setFriendlyName(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Integration::staticMetaObject = {
    { &QQuickItem::staticMetaObject, qt_meta_stringdata_Integration.data,
      qt_meta_data_Integration,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Integration::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Integration::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Integration.stringdata0))
        return static_cast<void*>(this);
    return QQuickItem::qt_metacast(_clname);
}

int Integration::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Integration::integrationIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Integration::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Integration::connecting()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Integration::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
