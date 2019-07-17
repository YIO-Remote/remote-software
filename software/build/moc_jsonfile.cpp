/****************************************************************************
** Meta object code from reading C++ file 'jsonfile.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/jsonfile.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jsonfile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_JsonFile_t {
    QByteArrayData data[18];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_JsonFile_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_JsonFile_t qt_meta_stringdata_JsonFile = {
    {
QT_MOC_LITERAL(0, 0, 8), // "JsonFile"
QT_MOC_LITERAL(1, 9, 11), // "nameChanged"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "name"
QT_MOC_LITERAL(4, 27, 7), // "setName"
QT_MOC_LITERAL(5, 35, 6), // "rename"
QT_MOC_LITERAL(6, 42, 7), // "newName"
QT_MOC_LITERAL(7, 50, 4), // "copy"
QT_MOC_LITERAL(8, 55, 6), // "remove"
QT_MOC_LITERAL(9, 62, 5), // "write"
QT_MOC_LITERAL(10, 68, 4), // "data"
QT_MOC_LITERAL(11, 73, 4), // "read"
QT_MOC_LITERAL(12, 78, 8), // "fileName"
QT_MOC_LITERAL(13, 87, 6), // "exists"
QT_MOC_LITERAL(14, 94, 9), // "writeable"
QT_MOC_LITERAL(15, 104, 8), // "readable"
QT_MOC_LITERAL(16, 113, 4), // "size"
QT_MOC_LITERAL(17, 118, 5) // "error"

    },
    "JsonFile\0nameChanged\0\0name\0setName\0"
    "rename\0newName\0copy\0remove\0write\0data\0"
    "read\0fileName\0exists\0writeable\0readable\0"
    "size\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_JsonFile[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       7,   66, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   52,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
       5,    1,   55,    2, 0x02 /* Public */,
       7,    1,   58,    2, 0x02 /* Public */,
       8,    0,   61,    2, 0x02 /* Public */,
       9,    1,   62,    2, 0x02 /* Public */,
      11,    0,   65,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // methods: parameters
    QMetaType::Bool, QMetaType::QString,    6,
    QMetaType::Bool, QMetaType::QString,    6,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::QVariantMap,   10,
    QMetaType::QVariant,

 // properties: name, type, flags
       3, QMetaType::QString, 0x00495103,
      12, QMetaType::QString, 0x00495001,
      13, QMetaType::Bool, 0x00095001,
      14, QMetaType::Bool, 0x00095001,
      15, QMetaType::Bool, 0x00095001,
      16, QMetaType::LongLong, 0x00095001,
      17, QMetaType::QString, 0x00095001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void JsonFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        JsonFile *_t = static_cast<JsonFile *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: { bool _r = _t->rename((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { bool _r = _t->copy((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: { bool _r = _t->remove();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->write((*reinterpret_cast< const QVariantMap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { QVariant _r = _t->read();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (JsonFile::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&JsonFile::nameChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        JsonFile *_t = static_cast<JsonFile *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->name(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->fileName(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->exists(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->writeable(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->readable(); break;
        case 5: *reinterpret_cast< qint64*>(_v) = _t->size(); break;
        case 6: *reinterpret_cast< QString*>(_v) = _t->error(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        JsonFile *_t = static_cast<JsonFile *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setName(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject JsonFile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_JsonFile.data,
      qt_meta_data_JsonFile,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *JsonFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JsonFile::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_JsonFile.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int JsonFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void JsonFile::nameChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
