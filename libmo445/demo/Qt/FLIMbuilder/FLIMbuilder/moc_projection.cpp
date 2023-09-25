/****************************************************************************
** Meta object code from reading C++ file 'projection.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "projection.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Projection_t {
    const uint offsetsAndSize[50];
    char stringdata0[500];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Projection_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Projection_t qt_meta_stringdata_Projection = {
    {
QT_MOC_LITERAL(0, 10), // "Projection"
QT_MOC_LITERAL(11, 15), // "kernelsSelected"
QT_MOC_LITERAL(27, 0), // ""
QT_MOC_LITERAL(28, 7), // "iftSet*"
QT_MOC_LITERAL(36, 14), // "kernel_indexes"
QT_MOC_LITERAL(51, 19), // "kernelDoubleClicked"
QT_MOC_LITERAL(71, 5), // "index"
QT_MOC_LITERAL(77, 16), // "projectionClosed"
QT_MOC_LITERAL(94, 28), // "on_hsPerplexity_valueChanged"
QT_MOC_LITERAL(123, 1), // "i"
QT_MOC_LITERAL(125, 29), // "on_hsNumIterTSNE_valueChanged"
QT_MOC_LITERAL(155, 28), // "on_sbPerplexity_valueChanged"
QT_MOC_LITERAL(184, 29), // "on_sbNumIterTSNE_valueChanged"
QT_MOC_LITERAL(214, 44), // "on_cbNodeColorByRelevance_cur..."
QT_MOC_LITERAL(259, 4), // "text"
QT_MOC_LITERAL(264, 32), // "on_cbNodeType_currentTextChanged"
QT_MOC_LITERAL(297, 36), // "on_cbFilterClass_currentIndex..."
QT_MOC_LITERAL(334, 23), // "projectionFinished_slot"
QT_MOC_LITERAL(358, 12), // "tsneExecuted"
QT_MOC_LITERAL(371, 20), // "on_pbProject_clicked"
QT_MOC_LITERAL(392, 27), // "on_pbSaveProjection_clicked"
QT_MOC_LITERAL(420, 10), // "timer_slot"
QT_MOC_LITERAL(431, 16), // "on_doubleClicked"
QT_MOC_LITERAL(448, 24), // "on_pbSaveKernels_clicked"
QT_MOC_LITERAL(473, 26) // "on_pbSaveRelevance_clicked"

    },
    "Projection\0kernelsSelected\0\0iftSet*\0"
    "kernel_indexes\0kernelDoubleClicked\0"
    "index\0projectionClosed\0"
    "on_hsPerplexity_valueChanged\0i\0"
    "on_hsNumIterTSNE_valueChanged\0"
    "on_sbPerplexity_valueChanged\0"
    "on_sbNumIterTSNE_valueChanged\0"
    "on_cbNodeColorByRelevance_currentTextChanged\0"
    "text\0on_cbNodeType_currentTextChanged\0"
    "on_cbFilterClass_currentIndexChanged\0"
    "projectionFinished_slot\0tsneExecuted\0"
    "on_pbProject_clicked\0on_pbSaveProjection_clicked\0"
    "timer_slot\0on_doubleClicked\0"
    "on_pbSaveKernels_clicked\0"
    "on_pbSaveRelevance_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Projection[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  116,    2, 0x06,    1 /* Public */,
       5,    1,  119,    2, 0x06,    3 /* Public */,
       7,    0,  122,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    1,  123,    2, 0x0a,    6 /* Public */,
      10,    1,  126,    2, 0x0a,    8 /* Public */,
      11,    1,  129,    2, 0x0a,   10 /* Public */,
      12,    1,  132,    2, 0x0a,   12 /* Public */,
      13,    1,  135,    2, 0x0a,   14 /* Public */,
      15,    1,  138,    2, 0x0a,   16 /* Public */,
      16,    1,  141,    2, 0x0a,   18 /* Public */,
      17,    1,  144,    2, 0x0a,   20 /* Public */,
      19,    0,  147,    2, 0x0a,   22 /* Public */,
      20,    0,  148,    2, 0x0a,   23 /* Public */,
      21,    0,  149,    2, 0x0a,   24 /* Public */,
      22,    1,  150,    2, 0x0a,   25 /* Public */,
      23,    0,  153,    2, 0x0a,   27 /* Public */,
      24,    0,  154,    2, 0x0a,   28 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Projection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Projection *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->kernelsSelected((*reinterpret_cast< std::add_pointer_t<iftSet*>>(_a[1]))); break;
        case 1: _t->kernelDoubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->projectionClosed(); break;
        case 3: _t->on_hsPerplexity_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->on_hsNumIterTSNE_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->on_sbPerplexity_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->on_sbNumIterTSNE_valueChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->on_cbNodeColorByRelevance_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->on_cbNodeType_currentTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->on_cbFilterClass_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->projectionFinished_slot((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->on_pbProject_clicked(); break;
        case 12: _t->on_pbSaveProjection_clicked(); break;
        case 13: _t->timer_slot(); break;
        case 14: _t->on_doubleClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->on_pbSaveKernels_clicked(); break;
        case 16: _t->on_pbSaveRelevance_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Projection::*)(iftSet * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Projection::kernelsSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Projection::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Projection::kernelDoubleClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Projection::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Projection::projectionClosed)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Projection::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_Projection.offsetsAndSize,
    qt_meta_data_Projection,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Projection_t
, QtPrivate::TypeAndForceComplete<Projection, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<iftSet *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *Projection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Projection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Projection.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Projection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void Projection::kernelsSelected(iftSet * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Projection::kernelDoubleClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Projection::projectionClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_MyThread_t {
    const uint offsetsAndSize[8];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MyThread_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MyThread_t qt_meta_stringdata_MyThread = {
    {
QT_MOC_LITERAL(0, 8), // "MyThread"
QT_MOC_LITERAL(9, 25), // "projectionFinished_signal"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 12) // "tsneExecuted"

    },
    "MyThread\0projectionFinished_signal\0\0"
    "tsneExecuted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyThread[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void MyThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->projectionFinished_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MyThread::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MyThread::projectionFinished_signal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MyThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_MyThread.offsetsAndSize,
    qt_meta_data_MyThread,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MyThread_t
, QtPrivate::TypeAndForceComplete<MyThread, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>



>,
    nullptr
} };


const QMetaObject *MyThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int MyThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void MyThread::projectionFinished_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
