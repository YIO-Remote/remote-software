/****************************************************************************
** Meta object code from reading C++ file 'bq27441.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/hardware/bq27441.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bq27441.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BQ27441_t {
    QByteArrayData data[12];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BQ27441_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BQ27441_t qt_meta_stringdata_BQ27441 = {
    {
QT_MOC_LITERAL(0, 0, 7), // "BQ27441"
QT_MOC_LITERAL(1, 8, 5), // "begin"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 10), // "getVoltage"
QT_MOC_LITERAL(4, 26, 21), // "getFullChargeCapacity"
QT_MOC_LITERAL(5, 48, 17), // "getAverageCurrent"
QT_MOC_LITERAL(6, 66, 15), // "getAveragePower"
QT_MOC_LITERAL(7, 82, 16), // "getStateOfCharge"
QT_MOC_LITERAL(8, 99, 23), // "getInternalTemperatureC"
QT_MOC_LITERAL(9, 123, 7), // "int16_t"
QT_MOC_LITERAL(10, 131, 16), // "getStateOfHealth"
QT_MOC_LITERAL(11, 148, 8) // "capacity"

    },
    "BQ27441\0begin\0\0getVoltage\0"
    "getFullChargeCapacity\0getAverageCurrent\0"
    "getAveragePower\0getStateOfCharge\0"
    "getInternalTemperatureC\0int16_t\0"
    "getStateOfHealth\0capacity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BQ27441[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       1,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x02 /* Public */,
       3,    0,   55,    2, 0x02 /* Public */,
       4,    0,   56,    2, 0x02 /* Public */,
       5,    0,   57,    2, 0x02 /* Public */,
       6,    0,   58,    2, 0x02 /* Public */,
       7,    0,   59,    2, 0x02 /* Public */,
       8,    0,   60,    2, 0x02 /* Public */,
      10,    0,   61,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    0x80000000 | 9,
    QMetaType::Int,

 // properties: name, type, flags
      11, QMetaType::Int, 0x00095103,

       0        // eod
};

void BQ27441::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BQ27441 *_t = static_cast<BQ27441 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->begin(); break;
        case 1: { int _r = _t->getVoltage();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 2: { int _r = _t->getFullChargeCapacity();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 3: { int _r = _t->getAverageCurrent();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 4: { int _r = _t->getAveragePower();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 5: { int _r = _t->getStateOfCharge();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 6: { int16_t _r = _t->getInternalTemperatureC();
            if (_a[0]) *reinterpret_cast< int16_t*>(_a[0]) = std::move(_r); }  break;
        case 7: { int _r = _t->getStateOfHealth();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        BQ27441 *_t = static_cast<BQ27441 *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getCapacity(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        BQ27441 *_t = static_cast<BQ27441 *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setCapacity(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject BQ27441::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BQ27441.data,
      qt_meta_data_BQ27441,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BQ27441::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BQ27441::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BQ27441.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BQ27441::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
