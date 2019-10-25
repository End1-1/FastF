/****************************************************************************
** Meta object code from reading C++ file 'ff_orderdrv.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ff_orderdrv.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ff_orderdrv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RemindThread_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RemindThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RemindThread_t qt_meta_stringdata_RemindThread = {
    {
QT_MOC_LITERAL(0, 0, 12) // "RemindThread"

    },
    "RemindThread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RemindThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void RemindThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject RemindThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_RemindThread.data,
      qt_meta_data_RemindThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RemindThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RemindThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RemindThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int RemindThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_FF_OrderDrv_t {
    QByteArrayData data[7];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FF_OrderDrv_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FF_OrderDrv_t qt_meta_stringdata_FF_OrderDrv = {
    {
QT_MOC_LITERAL(0, 0, 11), // "FF_OrderDrv"
QT_MOC_LITERAL(1, 12, 7), // "counted"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 11), // "dishRepaint"
QT_MOC_LITERAL(4, 33, 9), // "dishIndex"
QT_MOC_LITERAL(5, 43, 20), // "dishQtyEqualPrintQty"
QT_MOC_LITERAL(6, 64, 5) // "equal"

    },
    "FF_OrderDrv\0counted\0\0dishRepaint\0"
    "dishIndex\0dishQtyEqualPrintQty\0equal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FF_OrderDrv[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    1,   30,    2, 0x06 /* Public */,
       5,    1,   33,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Bool,    6,

       0        // eod
};

void FF_OrderDrv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FF_OrderDrv *_t = static_cast<FF_OrderDrv *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->counted(); break;
        case 1: _t->dishRepaint((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->dishQtyEqualPrintQty((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (FF_OrderDrv::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FF_OrderDrv::counted)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (FF_OrderDrv::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FF_OrderDrv::dishRepaint)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (FF_OrderDrv::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FF_OrderDrv::dishQtyEqualPrintQty)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject FF_OrderDrv::staticMetaObject = {
    { &QSqlDrv::staticMetaObject, qt_meta_stringdata_FF_OrderDrv.data,
      qt_meta_data_FF_OrderDrv,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FF_OrderDrv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FF_OrderDrv::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FF_OrderDrv.stringdata0))
        return static_cast<void*>(this);
    return QSqlDrv::qt_metacast(_clname);
}

int FF_OrderDrv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlDrv::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FF_OrderDrv::counted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FF_OrderDrv::dishRepaint(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FF_OrderDrv::dishQtyEqualPrintQty(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
