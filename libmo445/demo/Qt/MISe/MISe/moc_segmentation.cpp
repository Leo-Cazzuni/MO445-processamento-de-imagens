/****************************************************************************
** Meta object code from reading C++ file 'segmentation.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "segmentation/segmentation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'segmentation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Segmentation_t {
    const uint offsetsAndSize[20];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Segmentation_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Segmentation_t qt_meta_stringdata_Segmentation = {
    {
QT_MOC_LITERAL(0, 12), // "Segmentation"
QT_MOC_LITERAL(13, 20), // "segmentationFinished"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 8), // "progress"
QT_MOC_LITERAL(44, 1), // "p"
QT_MOC_LITERAL(46, 12), // "textProgress"
QT_MOC_LITERAL(59, 3), // "msg"
QT_MOC_LITERAL(63, 7), // "execute"
QT_MOC_LITERAL(71, 17), // "notifyImageUpdate"
QT_MOC_LITERAL(89, 18) // "finishSegmentation"

    },
    "Segmentation\0segmentationFinished\0\0"
    "progress\0p\0textProgress\0msg\0execute\0"
    "notifyImageUpdate\0finishSegmentation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Segmentation[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    1 /* Public */,
       3,    1,   51,    2, 0x06,    2 /* Public */,
       5,    1,   54,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   57,    2, 0x0a,    6 /* Public */,
       8,    0,   58,    2, 0x0a,    7 /* Public */,
       9,    0,   59,    2, 0x09,    8 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Segmentation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Segmentation *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->segmentationFinished(); break;
        case 1: _t->progress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->textProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->execute(); break;
        case 4: _t->notifyImageUpdate(); break;
        case 5: _t->finishSegmentation(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Segmentation::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Segmentation::segmentationFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Segmentation::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Segmentation::progress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Segmentation::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Segmentation::textProgress)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Segmentation::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Segmentation.offsetsAndSize,
    qt_meta_data_Segmentation,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Segmentation_t
, QtPrivate::TypeAndForceComplete<Segmentation, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *Segmentation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Segmentation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Segmentation.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Segmentation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Segmentation::segmentationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Segmentation::progress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Segmentation::textProgress(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
