/****************************************************************************
** Meta object code from reading C++ file 'qmygraphicsview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "views/qmygraphicsview.h"
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
#error "The header file 'qmygraphicsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QMyGraphicsView_t {
    const uint offsetsAndSize[30];
    char stringdata0[155];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_QMyGraphicsView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_QMyGraphicsView_t qt_meta_stringdata_QMyGraphicsView = {
    {
QT_MOC_LITERAL(0, 15), // "QMyGraphicsView"
QT_MOC_LITERAL(16, 15), // "positionChanged"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 1), // "x"
QT_MOC_LITERAL(35, 1), // "y"
QT_MOC_LITERAL(37, 7), // "clicked"
QT_MOC_LITERAL(45, 16), // "Qt::MouseButtons"
QT_MOC_LITERAL(62, 3), // "bts"
QT_MOC_LITERAL(66, 21), // "Qt::KeyboardModifiers"
QT_MOC_LITERAL(88, 9), // "modifiers"
QT_MOC_LITERAL(98, 13), // "signalRelease"
QT_MOC_LITERAL(112, 6), // "zoomIn"
QT_MOC_LITERAL(119, 7), // "zoomOut"
QT_MOC_LITERAL(127, 13), // "increaseBrush"
QT_MOC_LITERAL(141, 13) // "decreaseBrush"

    },
    "QMyGraphicsView\0positionChanged\0\0x\0y\0"
    "clicked\0Qt::MouseButtons\0bts\0"
    "Qt::KeyboardModifiers\0modifiers\0"
    "signalRelease\0zoomIn\0zoomOut\0increaseBrush\0"
    "decreaseBrush"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QMyGraphicsView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   56,    2, 0x06,    1 /* Public */,
       5,    4,   61,    2, 0x06,    4 /* Public */,
      10,    2,   70,    2, 0x06,    9 /* Public */,
      11,    0,   75,    2, 0x06,   12 /* Public */,
      12,    0,   76,    2, 0x06,   13 /* Public */,
      13,    0,   77,    2, 0x06,   14 /* Public */,
      14,    0,   78,    2, 0x06,   15 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 6, 0x80000000 | 8,    3,    4,    7,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QMyGraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QMyGraphicsView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->positionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->clicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<Qt::MouseButtons>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<Qt::KeyboardModifiers>>(_a[4]))); break;
        case 2: _t->signalRelease((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->zoomIn(); break;
        case 4: _t->zoomOut(); break;
        case 5: _t->increaseBrush(); break;
        case 6: _t->decreaseBrush(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QMyGraphicsView::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)(int , int , Qt::MouseButtons , Qt::KeyboardModifiers );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::clicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::signalRelease)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::zoomIn)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::zoomOut)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::increaseBrush)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (QMyGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QMyGraphicsView::decreaseBrush)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject QMyGraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_QMyGraphicsView.offsetsAndSize,
    qt_meta_data_QMyGraphicsView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_QMyGraphicsView_t
, QtPrivate::TypeAndForceComplete<QMyGraphicsView, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<Qt::MouseButtons, std::false_type>, QtPrivate::TypeAndForceComplete<Qt::KeyboardModifiers, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *QMyGraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QMyGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QMyGraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int QMyGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QMyGraphicsView::positionChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QMyGraphicsView::clicked(int _t1, int _t2, Qt::MouseButtons _t3, Qt::KeyboardModifiers _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QMyGraphicsView::signalRelease(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QMyGraphicsView::zoomIn()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void QMyGraphicsView::zoomOut()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void QMyGraphicsView::increaseBrush()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void QMyGraphicsView::decreaseBrush()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
