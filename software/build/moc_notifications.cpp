/****************************************************************************
** Meta object code from reading C++ file 'notifications.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/notifications.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notifications.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Notifications_t {
    QByteArrayData data[13];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Notifications_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Notifications_t qt_meta_stringdata_Notifications = {
    {
QT_MOC_LITERAL(0, 0, 13), // "Notifications"
QT_MOC_LITERAL(1, 14, 11), // "listChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "errorChanged"
QT_MOC_LITERAL(4, 40, 3), // "add"
QT_MOC_LITERAL(5, 44, 4), // "type"
QT_MOC_LITERAL(6, 49, 4), // "text"
QT_MOC_LITERAL(7, 54, 11), // "actionlabel"
QT_MOC_LITERAL(8, 66, 6), // "action"
QT_MOC_LITERAL(9, 73, 6), // "remove"
QT_MOC_LITERAL(10, 80, 2), // "id"
QT_MOC_LITERAL(11, 83, 4), // "list"
QT_MOC_LITERAL(12, 88, 12) // "isThereError"

    },
    "Notifications\0listChanged\0\0errorChanged\0"
    "add\0type\0text\0actionlabel\0action\0"
    "remove\0id\0list\0isThereError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Notifications[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       2,   58, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       4,    4,   41,    2, 0x02 /* Public */,
       4,    2,   50,    2, 0x02 /* Public */,
       9,    1,   55,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::QString, QMetaType::QVariant,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::Int,   10,

 // properties: name, type, flags
      11, QMetaType::QVariantList, 0x00495001,
      12, QMetaType::Bool, 0x00495001,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void Notifications::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Notifications *_t = static_cast<Notifications *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->listChanged(); break;
        case 1: _t->errorChanged(); break;
        case 2: _t->add((*reinterpret_cast< const bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4]))); break;
        case 3: _t->add((*reinterpret_cast< const bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->remove((*reinterpret_cast< const int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Notifications::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Notifications::listChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Notifications::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Notifications::errorChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Notifications *_t = static_cast<Notifications *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = _t->list(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isThereError(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Notifications::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Notifications.data,
      qt_meta_data_Notifications,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Notifications::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Notifications::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Notifications.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Notifications::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Notifications::listChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Notifications::errorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
