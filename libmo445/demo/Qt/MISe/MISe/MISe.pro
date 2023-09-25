#-------------------------------------------------
#
# Project created by QtCreator 2018-03-15T15:40:57
#
#-------------------------------------------------

QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MISe
TEMPLATE = app

LC_NUMERIC = en_US.UTF-8

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
        main.cpp \
        mainwindow.cpp \
        global.cpp \
    markerinformation.cpp \
    postprocessing/method.cpp \
    postprocessing/method/dilation.cpp \
    postprocessing/method/erosion.cpp \
    postprocessing/method/largestcomponentselection.cpp \
    postprocessing/method/loadasmarkers.cpp \
    postprocessing/method/morphclosing.cpp \
    postprocessing/method/morphopening.cpp \
    postprocessing/methodchooser.cpp \
    postprocessing/postprocessing.cpp \
    segmentation/altis/altis.cpp \
    segmentation/autoece/autoece.cpp \
    segmentation/autoece/chartview.cpp \
    segmentation/autoece/curvesview.cpp \
    segmentation/controlled/controlled.cpp \
    segmentation/dynamicift/dynamicift.cpp \
    segmentation/executework.cpp \
    segmentation/forestwrapper.cpp \
    segmentation/gradient/arcweightfunction.cpp \
    segmentation/gradient/brain/brain.cpp \
    segmentation/gradient/flim/flimfeatures.cpp \
    segmentation/gradient/magnitude/magnitudegradient.cpp \
    segmentation/gradient/saliency/saliency.cpp \
    segmentation/gradient/manual/manualarcweight.cpp \
    segmentation/gradient/server/serverprocessing.cpp \
    segmentation/livewire/livewire.cpp \
    segmentation/manual/manualsegmentation.cpp \
    segmentation/segmentation.cpp \
    segmentation/semiautomatic/semiautomatic.cpp \
    segmentation/threshold/histogramview.cpp \
    segmentation/threshold/threshold.cpp \
    views/qmywidget.cpp \
    views/slice/sliceview.cpp \
    views/view.cpp \
    views/qmygraphicsview.cpp \
    views/rendering/rendering.cpp \
    views/rendering/qrenderinggraphicsview.cpp \
    views/rendering/renderingview.cpp \
    views/orientation.cpp \
    marker.cpp \
    markersettings.cpp \
    volumeinformation.cpp \
    segmentation/altis/flim/flim.cpp

HEADERS += \
    aboutdialog.h \
        mainwindow.h \
        global.h \
    markerinformation.h \
    postprocessing/method.h \
    postprocessing/method/dilation.h \
    postprocessing/method/erosion.h \
    postprocessing/method/largestcomponentselection.h \
    postprocessing/method/loadasmarkers.h \
    postprocessing/method/morphclosing.h \
    postprocessing/method/morphopening.h \
    postprocessing/methodchooser.h \
    postprocessing/postprocessing.h \
    segmentation/altis/altis.h \
    segmentation/autoece/autoece.h \
    segmentation/autoece/chartview.h \
    segmentation/autoece/curvesview.h \
    segmentation/controlled/controlled.h \
    segmentation/dynamicift/dynamicift.h \
    segmentation/executework.h \
    segmentation/forestwrapper.h \
    segmentation/gradient/arcweightfunction.h \
    segmentation/gradient/brain/brain.h \
    segmentation/gradient/flim/flimfeatures.h \
    segmentation/gradient/magnitude/magnitudegradient.h \
    segmentation/gradient/manual/manualarcweight.h \
    segmentation/gradient/saliency/saliency.h \
    segmentation/gradient/server/serverprocessing.h \
    segmentation/livewire/livewire.h \
    segmentation/manual/manualsegmentation.h \
    segmentation/segmentation.h \
    segmentation/semiautomatic/semiautomatic.h \
    segmentation/threshold/histogramview.h \
    segmentation/threshold/threshold.h \
    views/qmywidget.h \
    views/slice/sliceview.h \
    views/view.h \
    views/qmygraphicsview.h \
    views/orientation.h \
    views/rendering/renderingview.h \
    views/rendering/rendering.h \
    views/rendering/qrenderinggraphicsview.h \
    marker.h \
    markersettings.h \
    volumeinformation.h \
    segmentation/altis/flim/flim.h

FORMS += \
    aboutdialog.ui \
        mainwindow.ui \
    markersettings.ui \
    postprocessing/method/dilation.ui \
    postprocessing/method/erosion.ui \
    postprocessing/method/largestcomponentselection.ui \
    postprocessing/method/loadasmarkers.ui \
    postprocessing/method/morphclosing.ui \
    postprocessing/method/morphopening.ui \
    postprocessing/methodchooser.ui \
    postprocessing/postprocessing.ui \
    segmentation/autoece/autoece.ui \
    segmentation/autoece/curvesview.ui \
    segmentation/controlled/controlled.ui \
    segmentation/gradient/brain/brain.ui \
    segmentation/gradient/flim/flimfeatures.ui \
    segmentation/gradient/magnitude/magnitudegradient.ui \
    segmentation/gradient/manual/manualarcweight.ui \
    segmentation/gradient/server/serverprocessing.ui \
    segmentation/livewire/livewire.ui \
    segmentation/manual/manualsegmentation.ui \
    segmentation/gradient/saliency/saliency.ui \    
    segmentation/threshold/histogramview.ui \
    views/rendering/renderingview.ui \
    segmentation/altis/altis.ui \
    segmentation/dynamicift/dynamicift.ui \
    segmentation/semiautomatic/semiautomatic.ui \
    segmentation/threshold/threshold.ui \
    views/slice/sliceview.ui \
    volumeinformation.ui

IFT_DIR = $$PWD/../../../..

LIBS += -L$$IFT_DIR/lib/ -lift -llapack -lblas -lm -fopenmp -lgomp

LIBPATH  += $$IFT_DIR/lib

INCLUDEPATH += $$IFT_DIR/include
INCLUDEPATH += $$IFT_DIR/externals/libsvm/include
INCLUDEPATH += $$IFT_DIR/externals/libpng/include
INCLUDEPATH += $$IFT_DIR/externals/libjpeg/include
INCLUDEPATH += $$IFT_DIR/externals/zlib/include
INCLUDEPATH += $$IFT_DIR/externals/tsne/include

QMAKE_CFLAGS += -fopenmp -pthread -pedantic -std=c++17 -O3

QMAKE_CXXFLAGS += -fopenmp -pthread -pedantic -std=c++17 -O3

#GPU STUFFS
INCLUDEPATH += /usr/local/cuda/include
LIBS   += -L/usr/local/cuda/lib64 -lcublas -lcudart

RESOURCES += \
    qresources.qrc

QMAKE_POST_LINK += $$QMAKE_COPY $$TARGET $$IFT_DIR/bin
