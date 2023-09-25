/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindow.h"
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
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    const uint offsetsAndSize[122];
    char stringdata0[1048];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 10), // "MainWindow"
QT_MOC_LITERAL(11, 23), // "updateAllGraphicalViews"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 21), // "slotCalculateGradient"
QT_MOC_LITERAL(58, 17), // "slotResetGradient"
QT_MOC_LITERAL(76, 6), // "zoomIn"
QT_MOC_LITERAL(83, 7), // "zoomOut"
QT_MOC_LITERAL(91, 10), // "normalSize"
QT_MOC_LITERAL(102, 24), // "slotChangeGradientMethod"
QT_MOC_LITERAL(127, 16), // "slotOpenImageSet"
QT_MOC_LITERAL(144, 12), // "slotOpenFile"
QT_MOC_LITERAL(157, 15), // "slotImportLabel"
QT_MOC_LITERAL(173, 15), // "slotExportLabel"
QT_MOC_LITERAL(189, 17), // "slotImportMarkers"
QT_MOC_LITERAL(207, 17), // "slotExportMarkers"
QT_MOC_LITERAL(225, 20), // "slotExportObjectsCSV"
QT_MOC_LITERAL(246, 23), // "slotExportImageSequence"
QT_MOC_LITERAL(270, 25), // "loadNextImageFromImageSet"
QT_MOC_LITERAL(296, 29), // "loadPreviousImageFromImageSet"
QT_MOC_LITERAL(326, 9), // "saveLabel"
QT_MOC_LITERAL(336, 25), // "slotOpenProcessingOptions"
QT_MOC_LITERAL(362, 9), // "slotAbout"
QT_MOC_LITERAL(372, 20), // "slotChangeBrightness"
QT_MOC_LITERAL(393, 18), // "slotChangeContrast"
QT_MOC_LITERAL(412, 25), // "slotShowVolumeInformation"
QT_MOC_LITERAL(438, 26), // "slotChangeObjectVisibility"
QT_MOC_LITERAL(465, 21), // "slotChangeObjectColor"
QT_MOC_LITERAL(487, 3), // "row"
QT_MOC_LITERAL(491, 3), // "col"
QT_MOC_LITERAL(495, 23), // "slotChangeObjectOpacity"
QT_MOC_LITERAL(519, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(537, 4), // "item"
QT_MOC_LITERAL(542, 11), // "slotMarkAll"
QT_MOC_LITERAL(554, 21), // "slotMarkerDoubleClick"
QT_MOC_LITERAL(576, 17), // "slotMarkerChanged"
QT_MOC_LITERAL(594, 21), // "slotChangeMarkerColor"
QT_MOC_LITERAL(616, 26), // "slotChangeMarkerVisibility"
QT_MOC_LITERAL(643, 7), // "visible"
QT_MOC_LITERAL(651, 16), // "slotAddNewMarker"
QT_MOC_LITERAL(668, 16), // "slotRemoveMarker"
QT_MOC_LITERAL(685, 14), // "slotUndoMarker"
QT_MOC_LITERAL(700, 31), // "slotDestroyMarkerSettingsWindow"
QT_MOC_LITERAL(732, 21), // "slotUpdateBrushRadius"
QT_MOC_LITERAL(754, 1), // "r"
QT_MOC_LITERAL(756, 20), // "slotUpdateMarkerName"
QT_MOC_LITERAL(777, 1), // "s"
QT_MOC_LITERAL(779, 21), // "slotUpdateMarkerColor"
QT_MOC_LITERAL(801, 5), // "color"
QT_MOC_LITERAL(807, 19), // "slotStartAnnotation"
QT_MOC_LITERAL(827, 4), // "mode"
QT_MOC_LITERAL(832, 19), // "slotEraseAnnotation"
QT_MOC_LITERAL(852, 18), // "slotHaltAnnotation"
QT_MOC_LITERAL(871, 26), // "slotTableOpenMarkerClicked"
QT_MOC_LITERAL(898, 28), // "slotTableDeleteObjectClicked"
QT_MOC_LITERAL(927, 28), // "slotChangeSegmentationMethod"
QT_MOC_LITERAL(956, 5), // "index"
QT_MOC_LITERAL(962, 27), // "slotShowVolumeTimeThumbnail"
QT_MOC_LITERAL(990, 4), // "time"
QT_MOC_LITERAL(995, 16), // "slotChangeVolume"
QT_MOC_LITERAL(1012, 29), // "slotClickOnRenderingOpenCurve"
QT_MOC_LITERAL(1042, 5) // "label"

    },
    "MainWindow\0updateAllGraphicalViews\0\0"
    "slotCalculateGradient\0slotResetGradient\0"
    "zoomIn\0zoomOut\0normalSize\0"
    "slotChangeGradientMethod\0slotOpenImageSet\0"
    "slotOpenFile\0slotImportLabel\0"
    "slotExportLabel\0slotImportMarkers\0"
    "slotExportMarkers\0slotExportObjectsCSV\0"
    "slotExportImageSequence\0"
    "loadNextImageFromImageSet\0"
    "loadPreviousImageFromImageSet\0saveLabel\0"
    "slotOpenProcessingOptions\0slotAbout\0"
    "slotChangeBrightness\0slotChangeContrast\0"
    "slotShowVolumeInformation\0"
    "slotChangeObjectVisibility\0"
    "slotChangeObjectColor\0row\0col\0"
    "slotChangeObjectOpacity\0QTableWidgetItem*\0"
    "item\0slotMarkAll\0slotMarkerDoubleClick\0"
    "slotMarkerChanged\0slotChangeMarkerColor\0"
    "slotChangeMarkerVisibility\0visible\0"
    "slotAddNewMarker\0slotRemoveMarker\0"
    "slotUndoMarker\0slotDestroyMarkerSettingsWindow\0"
    "slotUpdateBrushRadius\0r\0slotUpdateMarkerName\0"
    "s\0slotUpdateMarkerColor\0color\0"
    "slotStartAnnotation\0mode\0slotEraseAnnotation\0"
    "slotHaltAnnotation\0slotTableOpenMarkerClicked\0"
    "slotTableDeleteObjectClicked\0"
    "slotChangeSegmentationMethod\0index\0"
    "slotShowVolumeTimeThumbnail\0time\0"
    "slotChangeVolume\0slotClickOnRenderingOpenCurve\0"
    "label"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      48,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  302,    2, 0x0a,    1 /* Public */,
       3,    0,  303,    2, 0x0a,    2 /* Public */,
       4,    0,  304,    2, 0x0a,    3 /* Public */,
       5,    0,  305,    2, 0x08,    4 /* Private */,
       6,    0,  306,    2, 0x08,    5 /* Private */,
       7,    0,  307,    2, 0x08,    6 /* Private */,
       8,    0,  308,    2, 0x08,    7 /* Private */,
       9,    0,  309,    2, 0x08,    8 /* Private */,
      10,    0,  310,    2, 0x08,    9 /* Private */,
      11,    0,  311,    2, 0x08,   10 /* Private */,
      12,    0,  312,    2, 0x08,   11 /* Private */,
      13,    0,  313,    2, 0x08,   12 /* Private */,
      14,    0,  314,    2, 0x08,   13 /* Private */,
      15,    0,  315,    2, 0x08,   14 /* Private */,
      16,    0,  316,    2, 0x08,   15 /* Private */,
      17,    0,  317,    2, 0x08,   16 /* Private */,
      18,    0,  318,    2, 0x08,   17 /* Private */,
      19,    0,  319,    2, 0x08,   18 /* Private */,
      20,    0,  320,    2, 0x08,   19 /* Private */,
      21,    0,  321,    2, 0x08,   20 /* Private */,
      22,    0,  322,    2, 0x08,   21 /* Private */,
      23,    0,  323,    2, 0x08,   22 /* Private */,
      24,    0,  324,    2, 0x08,   23 /* Private */,
      25,    0,  325,    2, 0x08,   24 /* Private */,
      26,    2,  326,    2, 0x08,   25 /* Private */,
      29,    1,  331,    2, 0x08,   28 /* Private */,
      32,    0,  334,    2, 0x08,   30 /* Private */,
      33,    2,  335,    2, 0x08,   31 /* Private */,
      34,    2,  340,    2, 0x08,   34 /* Private */,
      35,    1,  345,    2, 0x08,   37 /* Private */,
      36,    0,  348,    2, 0x08,   39 /* Private */,
      36,    2,  349,    2, 0x08,   40 /* Private */,
      38,    0,  354,    2, 0x08,   43 /* Private */,
      39,    0,  355,    2, 0x08,   44 /* Private */,
      40,    0,  356,    2, 0x08,   45 /* Private */,
      41,    0,  357,    2, 0x08,   46 /* Private */,
      42,    1,  358,    2, 0x08,   47 /* Private */,
      44,    2,  361,    2, 0x08,   49 /* Private */,
      46,    2,  366,    2, 0x08,   52 /* Private */,
      48,    1,  371,    2, 0x08,   55 /* Private */,
      50,    0,  374,    2, 0x08,   57 /* Private */,
      51,    0,  375,    2, 0x08,   58 /* Private */,
      52,    0,  376,    2, 0x08,   59 /* Private */,
      53,    0,  377,    2, 0x08,   60 /* Private */,
      54,    1,  378,    2, 0x08,   61 /* Private */,
      56,    1,  381,    2, 0x08,   63 /* Private */,
      58,    0,  384,    2, 0x08,   65 /* Private */,
      59,    1,  385,    2, 0x08,   66 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   27,   28,
    QMetaType::Void, 0x80000000 | 30,   31,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   27,   28,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   27,   28,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   27,   37,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   43,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   27,   45,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,   27,   47,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   55,
    QMetaType::Void, QMetaType::Int,   57,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   60,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateAllGraphicalViews(); break;
        case 1: _t->slotCalculateGradient(); break;
        case 2: _t->slotResetGradient(); break;
        case 3: _t->zoomIn(); break;
        case 4: _t->zoomOut(); break;
        case 5: _t->normalSize(); break;
        case 6: _t->slotChangeGradientMethod(); break;
        case 7: _t->slotOpenImageSet(); break;
        case 8: _t->slotOpenFile(); break;
        case 9: _t->slotImportLabel(); break;
        case 10: _t->slotExportLabel(); break;
        case 11: _t->slotImportMarkers(); break;
        case 12: _t->slotExportMarkers(); break;
        case 13: _t->slotExportObjectsCSV(); break;
        case 14: _t->slotExportImageSequence(); break;
        case 15: _t->loadNextImageFromImageSet(); break;
        case 16: _t->loadPreviousImageFromImageSet(); break;
        case 17: _t->saveLabel(); break;
        case 18: _t->slotOpenProcessingOptions(); break;
        case 19: _t->slotAbout(); break;
        case 20: _t->slotChangeBrightness(); break;
        case 21: _t->slotChangeContrast(); break;
        case 22: _t->slotShowVolumeInformation(); break;
        case 23: _t->slotChangeObjectVisibility(); break;
        case 24: _t->slotChangeObjectColor((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->slotChangeObjectOpacity((*reinterpret_cast< std::add_pointer_t<QTableWidgetItem*>>(_a[1]))); break;
        case 26: _t->slotMarkAll(); break;
        case 27: _t->slotMarkerDoubleClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 28: _t->slotMarkerChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 29: _t->slotChangeMarkerColor((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 30: _t->slotChangeMarkerVisibility(); break;
        case 31: _t->slotChangeMarkerVisibility((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 32: _t->slotAddNewMarker(); break;
        case 33: _t->slotRemoveMarker(); break;
        case 34: _t->slotUndoMarker(); break;
        case 35: _t->slotDestroyMarkerSettingsWindow(); break;
        case 36: _t->slotUpdateBrushRadius((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 37: _t->slotUpdateMarkerName((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 38: _t->slotUpdateMarkerColor((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QColor>>(_a[2]))); break;
        case 39: _t->slotStartAnnotation((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 40: _t->slotEraseAnnotation(); break;
        case 41: _t->slotHaltAnnotation(); break;
        case 42: _t->slotTableOpenMarkerClicked(); break;
        case 43: _t->slotTableDeleteObjectClicked(); break;
        case 44: _t->slotChangeSegmentationMethod((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 45: _t->slotShowVolumeTimeThumbnail((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 46: _t->slotChangeVolume(); break;
        case 47: _t->slotClickOnRenderingOpenCurve((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSize,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t
, QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QTableWidgetItem *, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<float, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QColor, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>


>,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 48)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 48;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 48)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 48;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
