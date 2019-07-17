/****************************************************************************
** Meta object code from reading C++ file 'light.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/entities/light.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'light.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Light_t {
    QByteArrayData data[19];
    char stringdata0[183];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Light_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Light_t qt_meta_stringdata_Light = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Light"
QT_MOC_LITERAL(1, 6, 12), // "stateChanged"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 17), // "brightnessChanged"
QT_MOC_LITERAL(4, 38, 12), // "colorChanged"
QT_MOC_LITERAL(5, 51, 16), // "colorTempChanged"
QT_MOC_LITERAL(6, 68, 6), // "update"
QT_MOC_LITERAL(7, 75, 10), // "attributes"
QT_MOC_LITERAL(8, 86, 6), // "toggle"
QT_MOC_LITERAL(9, 93, 6), // "turnOn"
QT_MOC_LITERAL(10, 100, 7), // "turnOff"
QT_MOC_LITERAL(11, 108, 13), // "setBrightness"
QT_MOC_LITERAL(12, 122, 5), // "value"
QT_MOC_LITERAL(13, 128, 8), // "setColor"
QT_MOC_LITERAL(14, 137, 12), // "setColorTemp"
QT_MOC_LITERAL(15, 150, 5), // "state"
QT_MOC_LITERAL(16, 156, 10), // "brightness"
QT_MOC_LITERAL(17, 167, 5), // "color"
QT_MOC_LITERAL(18, 173, 9) // "colorTemp"

    },
    "Light\0stateChanged\0\0brightnessChanged\0"
    "colorChanged\0colorTempChanged\0update\0"
    "attributes\0toggle\0turnOn\0turnOff\0"
    "setBrightness\0value\0setColor\0setColorTemp\0"
    "state\0brightness\0color\0colorTemp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Light[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       4,   88, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,
       4,    0,   71,    2, 0x06 /* Public */,
       5,    0,   72,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       6,    1,   73,    2, 0x02 /* Public */,
       8,    0,   76,    2, 0x02 /* Public */,
       9,    0,   77,    2, 0x02 /* Public */,
      10,    0,   78,    2, 0x02 /* Public */,
      11,    1,   79,    2, 0x02 /* Public */,
      13,    1,   82,    2, 0x02 /* Public */,
      14,    1,   85,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Bool, QMetaType::QVariantMap,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QColor,   12,
    QMetaType::Void, QMetaType::Int,   12,

 // properties: name, type, flags
      15, QMetaType::Bool, 0x00495001,
      16, QMetaType::Int, 0x00495001,
      17, QMetaType::QColor, 0x00495001,
      18, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,

       0        // eod
};

void Light::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Light *_t = static_cast<Light *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stateChanged(); break;
        case 1: _t->brightnessChanged(); break;
        case 2: _t->colorChanged(); break;
        case 3: _t->colorTempChanged(); break;
        case 4: { bool _r = _t->update((*reinterpret_cast< const QVariantMap(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->toggle(); break;
        case 6: _t->turnOn(); break;
        case 7: _t->turnOff(); break;
        case 8: _t->setBrightness((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->setColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 10: _t->setColorTemp((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Light::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Light::stateChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Light::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Light::brightnessChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Light::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Light::colorChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Light::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Light::colorTempChanged)) {
                *result = 3;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Light *_t = static_cast<Light *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->state(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->brightness(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->color(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->colorTemp(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Light::staticMetaObject = {
    { &Entity::staticMetaObject, qt_meta_stringdata_Light.data,
      qt_meta_data_Light,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Light::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Light::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Light.stringdata0))
        return static_cast<void*>(this);
    return Entity::qt_metacast(_clname);
}

int Light::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Entity::qt_metacall(_c, _id, _a);
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
void Light::stateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Light::brightnessChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Light::colorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Light::colorTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
