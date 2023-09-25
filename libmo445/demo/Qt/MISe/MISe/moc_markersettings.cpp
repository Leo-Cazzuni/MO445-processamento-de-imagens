/****************************************************************************
** Meta object code from reading C++ file 'markersettings.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "markersettings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'markersettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MarkerSettings_t {
    const uint offsetsAndSize[46];
    char stringdata0[354];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MarkerSettings_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MarkerSettings_t qt_meta_stringdata_MarkerSettings = {
    {
QT_MOC_LITERAL(0, 14), // "MarkerSettings"
QT_MOC_LITERAL(15, 15), // "StartAnnotation"
QT_MOC_LITERAL(31, 0), // ""
QT_MOC_LITERAL(32, 14), // "HaltAnnotation"
QT_MOC_LITERAL(47, 11), // "UpdateBrush"
QT_MOC_LITERAL(59, 11), // "eraseMarker"
QT_MOC_LITERAL(71, 16), // "UpdateMarkerName"
QT_MOC_LITERAL(88, 17), // "UpdateMarkerColor"
QT_MOC_LITERAL(106, 22), // "updateMarkerVisibility"
QT_MOC_LITERAL(129, 16), // "ChangeMarkerName"
QT_MOC_LITERAL(146, 17), // "ChangeMarkerColor"
QT_MOC_LITERAL(164, 9), // "iftColor*"
QT_MOC_LITERAL(174, 5), // "color"
QT_MOC_LITERAL(180, 17), // "ChangeMarkerLabel"
QT_MOC_LITERAL(198, 18), // "EraseMarkerClicked"
QT_MOC_LITERAL(217, 30), // "StartFreeFromAnnotationClicked"
QT_MOC_LITERAL(248, 25), // "StartBoxAnnotationClicked"
QT_MOC_LITERAL(274, 11), // "changeBrush"
QT_MOC_LITERAL(286, 5), // "value"
QT_MOC_LITERAL(292, 16), // "changeSphericity"
QT_MOC_LITERAL(309, 15), // "OkButtonClicked"
QT_MOC_LITERAL(325, 22), // "changeMarkerVisibility"
QT_MOC_LITERAL(348, 5) // "state"

    },
    "MarkerSettings\0StartAnnotation\0\0"
    "HaltAnnotation\0UpdateBrush\0eraseMarker\0"
    "UpdateMarkerName\0UpdateMarkerColor\0"
    "updateMarkerVisibility\0ChangeMarkerName\0"
    "ChangeMarkerColor\0iftColor*\0color\0"
    "ChangeMarkerLabel\0EraseMarkerClicked\0"
    "StartFreeFromAnnotationClicked\0"
    "StartBoxAnnotationClicked\0changeBrush\0"
    "value\0changeSphericity\0OkButtonClicked\0"
    "changeMarkerVisibility\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MarkerSettings[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  122,    2, 0x06,    1 /* Public */,
       3,    0,  125,    2, 0x06,    3 /* Public */,
       4,    1,  126,    2, 0x06,    4 /* Public */,
       5,    0,  129,    2, 0x06,    6 /* Public */,
       6,    2,  130,    2, 0x06,    7 /* Public */,
       7,    2,  135,    2, 0x06,   10 /* Public */,
       8,    2,  140,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,  145,    2, 0x08,   16 /* Private */,
      10,    1,  146,    2, 0x08,   17 /* Private */,
      10,    0,  149,    2, 0x28,   19 /* Private | MethodCloned */,
      13,    0,  150,    2, 0x08,   20 /* Private */,
      14,    0,  151,    2, 0x08,   21 /* Private */,
      15,    0,  152,    2, 0x08,   22 /* Private */,
      16,    0,  153,    2, 0x08,   23 /* Private */,
      17,    1,  154,    2, 0x08,   24 /* Private */,
      19,    0,  157,    2, 0x08,   26 /* Private */,
      20,    0,  158,    2, 0x08,   27 /* Private */,
      21,    1,  159,    2, 0x08,   28 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   18,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   22,

       0        // eod
};

void MarkerSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MarkerSettings *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->StartAnnotation((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->HaltAnnotation(); break;
        case 2: _t->UpdateBrush((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 3: _t->eraseMarker(); break;
        case 4: _t->UpdateMarkerName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->UpdateMarkerColor((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QColor>>(_a[2]))); break;
        case 6: _t->updateMarkerVisibility((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->ChangeMarkerName(); break;
        case 8: _t->ChangeMarkerColor((*reinterpret_cast< std::add_pointer_t<iftColor*>>(_a[1]))); break;
        case 9: _t->ChangeMarkerColor(); break;
        case 10: _t->ChangeMarkerLabel(); break;
        case 11: _t->EraseMarkerClicked(); break;
        case 12: _t->StartFreeFromAnnotationClicked(); break;
        case 13: _t->StartBoxAnnotationClicked(); break;
        case 14: _t->changeBrush((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 15: _t->changeSphericity(); break;
        case 16: _t->OkButtonClicked(); break;
        case 17: _t->changeMarkerVisibility((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MarkerSettings::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::StartAnnotation)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::HaltAnnotation)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::UpdateBrush)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::eraseMarker)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)(int , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::UpdateMarkerName)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)(int , QColor );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::UpdateMarkerColor)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MarkerSettings::*)(int , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MarkerSettings::updateMarkerVisibility)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject MarkerSettings::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MarkerSettings.offsetsAndSize,
    qt_meta_data_MarkerSettings,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MarkerSettings_t
, QtPrivate::TypeAndForceComplete<MarkerSettings, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QColor, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<iftColor *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<double, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *MarkerSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MarkerSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MarkerSettings.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MarkerSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void MarkerSettings::StartAnnotation(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MarkerSettings::HaltAnnotation()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MarkerSettings::UpdateBrush(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MarkerSettings::eraseMarker()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MarkerSettings::UpdateMarkerName(int _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MarkerSettings::UpdateMarkerColor(int _t1, QColor _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MarkerSettings::updateMarkerVisibility(int _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
