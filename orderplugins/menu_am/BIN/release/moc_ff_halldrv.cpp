/****************************************************************************
** Meta object code from reading C++ file 'ff_halldrv.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ff_halldrv.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ff_halldrv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FF_HallDrv_t {
    QByteArrayData data[1];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FF_HallDrv_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FF_HallDrv_t qt_meta_stringdata_FF_HallDrv = {
    {
QT_MOC_LITERAL(0, 0, 10) // "FF_HallDrv"

    },
    "FF_HallDrv"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FF_HallDrv[] = {

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

void FF_HallDrv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject FF_HallDrv::staticMetaObject = {
    { &DbDriver::staticMetaObject, qt_meta_stringdata_FF_HallDrv.data,
      qt_meta_data_FF_HallDrv,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FF_HallDrv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FF_HallDrv::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FF_HallDrv.stringdata0))
        return static_cast<void*>(this);
    return DbDriver::qt_metacast(_clname);
}

int FF_HallDrv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DbDriver::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
