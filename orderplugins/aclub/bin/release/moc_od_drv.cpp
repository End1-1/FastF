/****************************************************************************
** Meta object code from reading C++ file 'od_drv.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../orderdrv/od_drv.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'od_drv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_OD_Drv_t {
    QByteArrayData data[7];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OD_Drv_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OD_Drv_t qt_meta_stringdata_OD_Drv = {
    {
QT_MOC_LITERAL(0, 0, 6), // "OD_Drv"
QT_MOC_LITERAL(1, 7, 7), // "counted"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 21), // "QMap<QString,QString>"
QT_MOC_LITERAL(4, 38, 6), // "values"
QT_MOC_LITERAL(5, 45, 11), // "dishRepaint"
QT_MOC_LITERAL(6, 57, 5) // "index"

    },
    "OD_Drv\0counted\0\0QMap<QString,QString>\0"
    "values\0dishRepaint\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OD_Drv[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       5,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,

       0        // eod
};

void OD_Drv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OD_Drv *_t = static_cast<OD_Drv *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->counted((*reinterpret_cast< const QMap<QString,QString>(*)>(_a[1]))); break;
        case 1: _t->dishRepaint((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OD_Drv::*_t)(const QMap<QString,QString> & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OD_Drv::counted)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (OD_Drv::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OD_Drv::dishRepaint)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject OD_Drv::staticMetaObject = {
    { &OD_Base::staticMetaObject, qt_meta_stringdata_OD_Drv.data,
      qt_meta_data_OD_Drv,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *OD_Drv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OD_Drv::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_OD_Drv.stringdata0))
        return static_cast<void*>(const_cast< OD_Drv*>(this));
    return OD_Base::qt_metacast(_clname);
}

int OD_Drv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OD_Base::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void OD_Drv::counted(const QMap<QString,QString> & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OD_Drv::dishRepaint(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
