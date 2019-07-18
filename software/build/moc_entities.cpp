/****************************************************************************
** Meta object code from reading C++ file 'entities.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../remote/sources/entities/entities.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'entities.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Entities_t {
    QByteArrayData data[23];
    char stringdata0[267];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Entities_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Entities_t qt_meta_stringdata_Entities = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Entities"
QT_MOC_LITERAL(1, 9, 9), // "getByType"
QT_MOC_LITERAL(2, 19, 15), // "QList<QObject*>"
QT_MOC_LITERAL(3, 35, 0), // ""
QT_MOC_LITERAL(4, 36, 4), // "type"
QT_MOC_LITERAL(5, 41, 9), // "getByArea"
QT_MOC_LITERAL(6, 51, 4), // "area"
QT_MOC_LITERAL(7, 56, 16), // "getByIntegration"
QT_MOC_LITERAL(8, 73, 11), // "integration"
QT_MOC_LITERAL(9, 85, 3), // "get"
QT_MOC_LITERAL(10, 89, 9), // "entity_id"
QT_MOC_LITERAL(11, 99, 3), // "add"
QT_MOC_LITERAL(12, 103, 6), // "config"
QT_MOC_LITERAL(13, 110, 14), // "integrationObj"
QT_MOC_LITERAL(14, 125, 6), // "update"
QT_MOC_LITERAL(15, 132, 10), // "attributes"
QT_MOC_LITERAL(16, 143, 15), // "addLoadedEntity"
QT_MOC_LITERAL(17, 159, 6), // "entity"
QT_MOC_LITERAL(18, 166, 29), // "getSupportedEntityTranslation"
QT_MOC_LITERAL(19, 196, 4), // "list"
QT_MOC_LITERAL(20, 201, 18), // "supported_entities"
QT_MOC_LITERAL(21, 220, 30), // "supported_entities_translation"
QT_MOC_LITERAL(22, 251, 15) // "loaded_entities"

    },
    "Entities\0getByType\0QList<QObject*>\0\0"
    "type\0getByArea\0area\0getByIntegration\0"
    "integration\0get\0entity_id\0add\0config\0"
    "integrationObj\0update\0attributes\0"
    "addLoadedEntity\0entity\0"
    "getSupportedEntityTranslation\0list\0"
    "supported_entities\0supported_entities_translation\0"
    "loaded_entities"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Entities[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       4,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   54,    3, 0x02 /* Public */,
       5,    1,   57,    3, 0x02 /* Public */,
       7,    1,   60,    3, 0x02 /* Public */,
       9,    1,   63,    3, 0x02 /* Public */,
      11,    2,   66,    3, 0x02 /* Public */,
      14,    2,   71,    3, 0x02 /* Public */,
      16,    1,   76,    3, 0x02 /* Public */,
      18,    1,   79,    3, 0x02 /* Public */,

 // methods: parameters
    0x80000000 | 2, QMetaType::QString,    4,
    0x80000000 | 2, QMetaType::QString,    6,
    0x80000000 | 2, QMetaType::QString,    8,
    QMetaType::QObjectStar, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QVariantMap, QMetaType::QObjectStar,   12,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariantMap,   10,   15,
    QMetaType::Void, QMetaType::QString,   17,
    QMetaType::QString, QMetaType::QString,    4,

 // properties: name, type, flags
      19, 0x80000000 | 2, 0x00095409,
      20, QMetaType::QStringList, 0x00095401,
      21, QMetaType::QStringList, 0x00095401,
      22, QMetaType::QStringList, 0x00095401,

       0        // eod
};

void Entities::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Entities *_t = static_cast<Entities *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QList<QObject*> _r = _t->getByType((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 1: { QList<QObject*> _r = _t->getByArea((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 2: { QList<QObject*> _r = _t->getByIntegration((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 3: { QObject* _r = _t->get((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = std::move(_r); }  break;
        case 4: _t->add((*reinterpret_cast< const QVariantMap(*)>(_a[1])),(*reinterpret_cast< QObject*(*)>(_a[2]))); break;
        case 5: _t->update((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariantMap(*)>(_a[2]))); break;
        case 6: _t->addLoadedEntity((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: { QString _r = _t->getSupportedEntityTranslation((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QObject*> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Entities *_t = static_cast<Entities *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QList<QObject*>*>(_v) = _t->list(); break;
        case 1: *reinterpret_cast< QStringList*>(_v) = _t->supported_entities(); break;
        case 2: *reinterpret_cast< QStringList*>(_v) = _t->supported_entities_translation(); break;
        case 3: *reinterpret_cast< QStringList*>(_v) = _t->loaded_entities(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Entities::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Entities.data,
      qt_meta_data_Entities,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Entities::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Entities::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Entities.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "EntitiesInterface"))
        return static_cast< EntitiesInterface*>(this);
    if (!strcmp(_clname, "YIO.EntitiesInterface"))
        return static_cast< EntitiesInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int Entities::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

QT_PLUGIN_METADATA_SECTION const uint qt_section_alignment_dummy = 42;

#ifdef QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0xb0, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x9c, 0x00, 0x00, 0x00, 0x13, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x15, 0x00, 0x00, 0x00, 'Y',  0x00, 'I',  0x00,
    'O',  0x00, '.',  0x00, 'E',  0x00, 'n',  0x00,
    't',  0x00, 'i',  0x00, 't',  0x00, 'i',  0x00,
    'e',  0x00, 's',  0x00, 'I',  0x00, 'n',  0x00,
    't',  0x00, 'e',  0x00, 'r',  0x00, 'f',  0x00,
    'a',  0x00, 'c',  0x00, 'e',  0x00, 0x00, 0x00,
    0x1b, 0x0b, 0x00, 0x00, 0x09, 0x00, 'c',  'l', 
    'a',  's',  's',  'N',  'a',  'm',  'e',  0x00,
    0x08, 0x00, 'E',  'n',  't',  'i',  't',  'i', 
    'e',  's',  0x00, 0x00, ':',  '`',  0xa1, 0x00,
    0x07, 0x00, 'v',  'e',  'r',  's',  'i',  'o', 
    'n',  0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00,
    0x05, 0x00, 'd',  'e',  'b',  'u',  'g',  0x00,
    0x15, 0x12, 0x00, 0x00, 0x08, 0x00, 'M',  'e', 
    't',  'a',  'D',  'a',  't',  'a',  0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 'H',  0x00, 0x00, 0x00,
    't',  0x00, 0x00, 0x00, 'd',  0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0xb0, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    0x9c, 0x00, 0x00, 0x00, 0x13, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x15, 0x00, 0x00, 0x00, 'Y',  0x00, 'I',  0x00,
    'O',  0x00, '.',  0x00, 'E',  0x00, 'n',  0x00,
    't',  0x00, 'i',  0x00, 't',  0x00, 'i',  0x00,
    'e',  0x00, 's',  0x00, 'I',  0x00, 'n',  0x00,
    't',  0x00, 'e',  0x00, 'r',  0x00, 'f',  0x00,
    'a',  0x00, 'c',  0x00, 'e',  0x00, 0x00, 0x00,
    0x15, 0x0b, 0x00, 0x00, 0x08, 0x00, 'M',  'e', 
    't',  'a',  'D',  'a',  't',  'a',  0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9b, 0x0e, 0x00, 0x00,
    0x09, 0x00, 'c',  'l',  'a',  's',  's',  'N', 
    'a',  'm',  'e',  0x00, 0x08, 0x00, 'E',  'n', 
    't',  'i',  't',  'i',  'e',  's',  0x00, 0x00,
    '1',  0x00, 0x00, 0x00, 0x05, 0x00, 'd',  'e', 
    'b',  'u',  'g',  0x00, ':',  '`',  0xa1, 0x00,
    0x07, 0x00, 'v',  'e',  'r',  's',  'i',  'o', 
    'n',  0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    'H',  0x00, 0x00, 0x00, 'd',  0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

QT_MOC_EXPORT_PLUGIN(Entities, Entities)

QT_WARNING_POP
QT_END_MOC_NAMESPACE
