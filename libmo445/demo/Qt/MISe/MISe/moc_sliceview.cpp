/****************************************************************************
** Meta object code from reading C++ file 'sliceview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "views/slice/sliceview.h"
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
#error "The header file 'sliceview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SliceView_t {
    const uint offsetsAndSize[42];
    char stringdata0[328];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_SliceView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_SliceView_t qt_meta_stringdata_SliceView = {
    {
QT_MOC_LITERAL(0, 9), // "SliceView"
QT_MOC_LITERAL(10, 26), // "requestGradientCalculation"
QT_MOC_LITERAL(37, 0), // ""
QT_MOC_LITERAL(38, 10), // "slotZoomIn"
QT_MOC_LITERAL(49, 11), // "slotZoomOut"
QT_MOC_LITERAL(61, 14), // "slotNormalSize"
QT_MOC_LITERAL(76, 25), // "slotToogleLinesVisibility"
QT_MOC_LITERAL(102, 25), // "slotSetGradientVisibility"
QT_MOC_LITERAL(128, 5), // "state"
QT_MOC_LITERAL(134, 28), // "slotUpdateSliceMousePosition"
QT_MOC_LITERAL(163, 1), // "x"
QT_MOC_LITERAL(165, 1), // "y"
QT_MOC_LITERAL(167, 15), // "slotReleaseView"
QT_MOC_LITERAL(183, 24), // "slotGraphicalViewClicked"
QT_MOC_LITERAL(208, 16), // "Qt::MouseButtons"
QT_MOC_LITERAL(225, 21), // "Qt::KeyboardModifiers"
QT_MOC_LITERAL(247, 9), // "modifiers"
QT_MOC_LITERAL(257, 20), // "slotChangeSliceImage"
QT_MOC_LITERAL(278, 21), // "slotSaveGraphicalView"
QT_MOC_LITERAL(300, 13), // "increaseBrush"
QT_MOC_LITERAL(314, 13) // "decreaseBrush"

    },
    "SliceView\0requestGradientCalculation\0"
    "\0slotZoomIn\0slotZoomOut\0slotNormalSize\0"
    "slotToogleLinesVisibility\0"
    "slotSetGradientVisibility\0state\0"
    "slotUpdateSliceMousePosition\0x\0y\0"
    "slotReleaseView\0slotGraphicalViewClicked\0"
    "Qt::MouseButtons\0Qt::KeyboardModifiers\0"
    "modifiers\0slotChangeSliceImage\0"
    "slotSaveGraphicalView\0increaseBrush\0"
    "decreaseBrush"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SliceView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   93,    2, 0x0a,    2 /* Public */,
       4,    0,   94,    2, 0x0a,    3 /* Public */,
       5,    0,   95,    2, 0x0a,    4 /* Public */,
       6,    0,   96,    2, 0x0a,    5 /* Public */,
       7,    1,   97,    2, 0x0a,    6 /* Public */,
       9,    2,  100,    2, 0x08,    8 /* Private */,
      12,    2,  105,    2, 0x08,   11 /* Private */,
      13,    4,  110,    2, 0x08,   14 /* Private */,
      17,    0,  119,    2, 0x08,   19 /* Private */,
      18,    0,  120,    2, 0x08,   20 /* Private */,
      19,    0,  121,    2, 0x08,   21 /* Private */,
      20,    0,  122,    2, 0x08,   22 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 14, 0x80000000 | 15,   10,   11,    2,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SliceView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SliceView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->requestGradientCalculation(); break;
        case 1: _t->slotZoomIn(); break;
        case 2: _t->slotZoomOut(); break;
        case 3: _t->slotNormalSize(); break;
        case 4: _t->slotToogleLinesVisibility(); break;
        case 5: _t->slotSetGradientVisibility((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->slotUpdateSliceMousePosition((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->slotReleaseView((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->slotGraphicalViewClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<Qt::MouseButtons>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<Qt::KeyboardModifiers>>(_a[4]))); break;
        case 9: _t->slotChangeSliceImage(); break;
        case 10: _t->slotSaveGraphicalView(); break;
        case 11: _t->increaseBrush(); break;
        case 12: _t->decreaseBrush(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SliceView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SliceView::requestGradientCalculation)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject SliceView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SliceView.offsetsAndSize,
    qt_meta_data_SliceView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_SliceView_t
, QtPrivate::TypeAndForceComplete<SliceView, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<Qt::MouseButtons, std::false_type>, QtPrivate::TypeAndForceComplete<Qt::KeyboardModifiers, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *SliceView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SliceView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SliceView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SliceView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void SliceView::requestGradientCalculation()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
