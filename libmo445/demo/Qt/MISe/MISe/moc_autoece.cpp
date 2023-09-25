/****************************************************************************
** Meta object code from reading C++ file 'autoece.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "segmentation/autoece/autoece.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qabstractbarseries.h>
#include <QtCharts/qvbarmodelmapper.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCharts/qcandlestickseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'autoece.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AutoECE_t {
    const uint offsetsAndSize[20];
    char stringdata0[148];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_AutoECE_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_AutoECE_t qt_meta_stringdata_AutoECE = {
    {
QT_MOC_LITERAL(0, 7), // "AutoECE"
QT_MOC_LITERAL(8, 21), // "toggleAdvancedOptions"
QT_MOC_LITERAL(30, 0), // ""
QT_MOC_LITERAL(31, 7), // "checked"
QT_MOC_LITERAL(39, 21), // "changeValueSliderTime"
QT_MOC_LITERAL(61, 4), // "time"
QT_MOC_LITERAL(66, 23), // "changeTimeUserTimerEdit"
QT_MOC_LITERAL(90, 13), // "viewECECurves"
QT_MOC_LITERAL(104, 21), // "toggleBackgroundSeeds"
QT_MOC_LITERAL(126, 21) // "on_pushButton_clicked"

    },
    "AutoECE\0toggleAdvancedOptions\0\0checked\0"
    "changeValueSliderTime\0time\0"
    "changeTimeUserTimerEdit\0viewECECurves\0"
    "toggleBackgroundSeeds\0on_pushButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutoECE[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x08,    1 /* Private */,
       4,    1,   53,    2, 0x08,    3 /* Private */,
       6,    1,   56,    2, 0x08,    5 /* Private */,
       7,    0,   59,    2, 0x08,    7 /* Private */,
       8,    1,   60,    2, 0x08,    8 /* Private */,
       9,    0,   63,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::QTime,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

       0        // eod
};

void AutoECE::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AutoECE *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->toggleAdvancedOptions((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->changeValueSliderTime((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->changeTimeUserTimerEdit((*reinterpret_cast< std::add_pointer_t<QTime>>(_a[1]))); break;
        case 3: _t->viewECECurves(); break;
        case 4: _t->toggleBackgroundSeeds((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->on_pushButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject AutoECE::staticMetaObject = { {
    QMetaObject::SuperData::link<Segmentation::staticMetaObject>(),
    qt_meta_stringdata_AutoECE.offsetsAndSize,
    qt_meta_data_AutoECE,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_AutoECE_t
, QtPrivate::TypeAndForceComplete<AutoECE, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QTime, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *AutoECE::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoECE::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoECE.stringdata0))
        return static_cast<void*>(this);
    return Segmentation::qt_metacast(_clname);
}

int AutoECE::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Segmentation::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
