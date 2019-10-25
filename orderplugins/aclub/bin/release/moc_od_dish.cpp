/****************************************************************************
** Meta object code from reading C++ file 'od_dish.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../orderdrv/od_dish.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'od_dish.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_OD_Dish_t {
    QByteArrayData data[10];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OD_Dish_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OD_Dish_t qt_meta_stringdata_OD_Dish = {
    {
QT_MOC_LITERAL(0, 0, 7), // "OD_Dish"
QT_MOC_LITERAL(1, 8, 6), // "update"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 5), // "index"
QT_MOC_LITERAL(4, 22, 7), // "removed"
QT_MOC_LITERAL(5, 30, 3), // "qty"
QT_MOC_LITERAL(6, 34, 7), // "message"
QT_MOC_LITERAL(7, 42, 3), // "msg"
QT_MOC_LITERAL(8, 46, 6), // "incQty"
QT_MOC_LITERAL(9, 53, 6) // "decQty"

    },
    "OD_Dish\0update\0\0index\0removed\0qty\0"
    "message\0msg\0incQty\0decQty"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OD_Dish[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    2,   42,    2, 0x06 /* Public */,
       6,    1,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   50,    2, 0x0a /* Public */,
       9,    1,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    3,    5,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Float,    5,
    QMetaType::Void, QMetaType::Float,    5,

       0        // eod
};

void OD_Dish::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OD_Dish *_t = static_cast<OD_Dish *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->update((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->removed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 2: _t->message((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->incQty((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->decQty((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OD_Dish::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OD_Dish::update)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (OD_Dish::*_t)(int , float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OD_Dish::removed)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (OD_Dish::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OD_Dish::message)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject OD_Dish::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_OD_Dish.data,
      qt_meta_data_OD_Dish,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *OD_Dish::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OD_Dish::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_OD_Dish.stringdata0))
        return static_cast<void*>(const_cast< OD_Dish*>(this));
    return QObject::qt_metacast(_clname);
}

int OD_Dish::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
    return _id;
}

// SIGNAL 0
void OD_Dish::update(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OD_Dish::removed(int _t1, float _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void OD_Dish::message(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
