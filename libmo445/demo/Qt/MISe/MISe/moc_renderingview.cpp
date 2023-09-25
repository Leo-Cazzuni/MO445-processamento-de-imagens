/****************************************************************************
** Meta object code from reading C++ file 'renderingview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "views/rendering/renderingview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'renderingview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RenderingView_t {
    const uint offsetsAndSize[38];
    char stringdata0[244];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_RenderingView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_RenderingView_t qt_meta_stringdata_RenderingView = {
    {
QT_MOC_LITERAL(0, 13), // "RenderingView"
QT_MOC_LITERAL(14, 20), // "updatedGraphicsViews"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 12), // "labelClicked"
QT_MOC_LITERAL(49, 10), // "slotZoomIn"
QT_MOC_LITERAL(60, 11), // "slotZoomOut"
QT_MOC_LITERAL(72, 14), // "slotNormalSize"
QT_MOC_LITERAL(87, 24), // "slotUpdateRenderingAngle"
QT_MOC_LITERAL(112, 4), // "tilt"
QT_MOC_LITERAL(117, 4), // "spin"
QT_MOC_LITERAL(122, 23), // "slotUpdateRenderingMode"
QT_MOC_LITERAL(146, 4), // "mode"
QT_MOC_LITERAL(151, 20), // "slotRenderingClicked"
QT_MOC_LITERAL(172, 1), // "x"
QT_MOC_LITERAL(174, 1), // "y"
QT_MOC_LITERAL(176, 15), // "slotReleaseView"
QT_MOC_LITERAL(192, 17), // "slotDrawWireframe"
QT_MOC_LITERAL(210, 14), // "slotDrawPlanes"
QT_MOC_LITERAL(225, 18) // "slotSaveProjection"

    },
    "RenderingView\0updatedGraphicsViews\0\0"
    "labelClicked\0slotZoomIn\0slotZoomOut\0"
    "slotNormalSize\0slotUpdateRenderingAngle\0"
    "tilt\0spin\0slotUpdateRenderingMode\0"
    "mode\0slotRenderingClicked\0x\0y\0"
    "slotReleaseView\0slotDrawWireframe\0"
    "slotDrawPlanes\0slotSaveProjection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RenderingView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,
       3,    1,   87,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   90,    2, 0x0a,    4 /* Public */,
       5,    0,   91,    2, 0x0a,    5 /* Public */,
       6,    0,   92,    2, 0x0a,    6 /* Public */,
       7,    2,   93,    2, 0x08,    7 /* Private */,
      10,    1,   98,    2, 0x08,   10 /* Private */,
      12,    2,  101,    2, 0x08,   12 /* Private */,
      15,    0,  106,    2, 0x08,   15 /* Private */,
      16,    0,  107,    2, 0x08,   16 /* Private */,
      17,    0,  108,    2, 0x08,   17 /* Private */,
      18,    0,  109,    2, 0x08,   18 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,    8,    9,
    QMetaType::Void, QMetaType::Char,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RenderingView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RenderingView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updatedGraphicsViews(); break;
        case 1: _t->labelClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->slotZoomIn(); break;
        case 3: _t->slotZoomOut(); break;
        case 4: _t->slotNormalSize(); break;
        case 5: _t->slotUpdateRenderingAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 6: _t->slotUpdateRenderingMode((*reinterpret_cast< std::add_pointer_t<char>>(_a[1]))); break;
        case 7: _t->slotRenderingClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->slotReleaseView(); break;
        case 9: _t->slotDrawWireframe(); break;
        case 10: _t->slotDrawPlanes(); break;
        case 11: _t->slotSaveProjection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RenderingView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RenderingView::updatedGraphicsViews)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RenderingView::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RenderingView::labelClicked)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject RenderingView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RenderingView.offsetsAndSize,
    qt_meta_data_RenderingView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_RenderingView_t
, QtPrivate::TypeAndForceComplete<RenderingView, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<char, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *RenderingView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RenderingView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RenderingView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RenderingView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void RenderingView::updatedGraphicsViews()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RenderingView::labelClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
