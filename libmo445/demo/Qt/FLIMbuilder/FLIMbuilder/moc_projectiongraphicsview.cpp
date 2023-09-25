/****************************************************************************
** Meta object code from reading C++ file 'projectiongraphicsview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "projectiongraphicsview.h"
#include <QtGui/qtextcursor.h>
#include <QScreen>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'projectiongraphicsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ProjectionGraphicsView_t {
    const uint offsetsAndSize[18];
    char stringdata0[180];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ProjectionGraphicsView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ProjectionGraphicsView_t qt_meta_stringdata_ProjectionGraphicsView = {
    {
QT_MOC_LITERAL(0, 22), // "ProjectionGraphicsView"
QT_MOC_LITERAL(23, 30), // "mouseContextMenuRequest_signal"
QT_MOC_LITERAL(54, 0), // ""
QT_MOC_LITERAL(55, 18), // "QContextMenuEvent*"
QT_MOC_LITERAL(74, 5), // "event"
QT_MOC_LITERAL(80, 27), // "sceneAreaSizeChanged_signal"
QT_MOC_LITERAL(108, 32), // "updateSceneAutoReprojection_slot"
QT_MOC_LITERAL(141, 13), // "autoReproject"
QT_MOC_LITERAL(155, 24) // "updateNodesPosition_slot"

    },
    "ProjectionGraphicsView\0"
    "mouseContextMenuRequest_signal\0\0"
    "QContextMenuEvent*\0event\0"
    "sceneAreaSizeChanged_signal\0"
    "updateSceneAutoReprojection_slot\0"
    "autoReproject\0updateNodesPosition_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProjectionGraphicsView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       5,    0,   41,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    1,   42,    2, 0x0a,    4 /* Public */,
       8,    0,   45,    2, 0x0a,    6 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,

       0        // eod
};

void ProjectionGraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProjectionGraphicsView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->mouseContextMenuRequest_signal((*reinterpret_cast< std::add_pointer_t<QContextMenuEvent*>>(_a[1]))); break;
        case 1: _t->sceneAreaSizeChanged_signal(); break;
        case 2: _t->updateSceneAutoReprojection_slot((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->updateNodesPosition_slot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ProjectionGraphicsView::*)(QContextMenuEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectionGraphicsView::mouseContextMenuRequest_signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ProjectionGraphicsView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ProjectionGraphicsView::sceneAreaSizeChanged_signal)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject ProjectionGraphicsView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_meta_stringdata_ProjectionGraphicsView.offsetsAndSize,
    qt_meta_data_ProjectionGraphicsView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ProjectionGraphicsView_t
, QtPrivate::TypeAndForceComplete<ProjectionGraphicsView, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QContextMenuEvent *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *ProjectionGraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProjectionGraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProjectionGraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int ProjectionGraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ProjectionGraphicsView::mouseContextMenuRequest_signal(QContextMenuEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProjectionGraphicsView::sceneAreaSizeChanged_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
