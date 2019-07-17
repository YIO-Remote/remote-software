/****************************************************************************
** Meta object code from reading C++ file 'entity.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/entities/entity.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'entity.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Entity_t {
    QByteArrayData data[16];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Entity_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Entity_t qt_meta_stringdata_Entity = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Entity"
QT_MOC_LITERAL(1, 7, 14), // "featureChanged"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 15), // "favoriteChanged"
QT_MOC_LITERAL(4, 39, 17), // "attributesChanged"
QT_MOC_LITERAL(5, 57, 7), // "command"
QT_MOC_LITERAL(6, 65, 5), // "param"
QT_MOC_LITERAL(7, 71, 13), // "getDataToSave"
QT_MOC_LITERAL(8, 85, 4), // "type"
QT_MOC_LITERAL(9, 90, 13), // "friendly_name"
QT_MOC_LITERAL(10, 104, 9), // "entity_id"
QT_MOC_LITERAL(11, 114, 4), // "area"
QT_MOC_LITERAL(12, 119, 11), // "integration"
QT_MOC_LITERAL(13, 131, 8), // "favorite"
QT_MOC_LITERAL(14, 140, 18), // "supported_features"
QT_MOC_LITERAL(15, 159, 10) // "attributes"

    },
    "Entity\0featureChanged\0\0favoriteChanged\0"
    "attributesChanged\0command\0param\0"
    "getDataToSave\0type\0friendly_name\0"
    "entity_id\0area\0integration\0favorite\0"
    "supported_features\0attributes"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Entity[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       8,   48, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       5,    2,   42,    2, 0x02 /* Public */,
       7,    0,   47,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    5,    6,
    QMetaType::QVariantMap,

 // properties: name, type, flags
       8, QMetaType::QString, 0x00095401,
       9, QMetaType::QString, 0x00095401,
      10, QMetaType::QString, 0x00095401,
      11, QMetaType::QString, 0x00095401,
      12, QMetaType::QString, 0x00095401,
      13, QMetaType::Bool, 0x00495103,
      14, QMetaType::QStringList, 0x00495001,
      15, QMetaType::QVariant, 0x00495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       1,
       0,
       2,

       0        // eod
};

void Entity::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Entity *_t = static_cast<Entity *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->featureChanged(); break;
        case 1: _t->favoriteChanged(); break;
        case 2: _t->attributesChanged(); break;
        case 3: _t->command((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 4: { QVariantMap _r = _t->getDataToSave();
            if (_a[0]) *reinterpret_cast< QVariantMap*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Entity::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Entity::featureChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Entity::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Entity::favoriteChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Entity::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Entity::attributesChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Entity *_t = static_cast<Entity *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->type(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->friendly_name(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->entity_id(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->area(); break;
        case 4: *reinterpret_cast< QString*>(_v) = _t->integration(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->favorite(); break;
        case 6: *reinterpret_cast< QStringList*>(_v) = _t->supported_features(); break;
        case 7: *reinterpret_cast< QVariant*>(_v) = _t->attributes(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Entity *_t = static_cast<Entity *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 5: _t->setFavorite(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Entity::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Entity.data,
      qt_meta_data_Entity,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Entity::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Entity::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Entity.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Entity::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Entity::featureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Entity::favoriteChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Entity::attributesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
