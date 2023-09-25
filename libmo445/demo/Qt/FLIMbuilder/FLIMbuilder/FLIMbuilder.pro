#-------------------------------------------------
#
# Project created by QtCreator 2021-08-10T23:44:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FLIMbuilder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    common.cpp \
    mygraphicsview.cpp \
    markers.cpp \
    flim.cpp \
    mycombobox.cpp \
    orientation.cpp \
    runguard.cpp \
    myplaintextedit.cpp \
    graphnode.cpp \
    graphedge.cpp \
    projectiongraphicsview.cpp \
    projection.cpp

HEADERS += \
        mainwindow.h \
    common.h \
    mygraphicsview.h \
    markers.h \
    flim.h \
    mycombobox.h \
    orientation.h \
    runguard.h \
    myplaintextedit.h \
    graphnode.h \
    graphedge.h \
    projectiongraphicsview.h \
    projection.h

FORMS += \
        mainwindow.ui \
    projection.ui

IFT_DIR = $$PWD/../../../..

INCLUDEPATH += $$IFT_DIR/include

###INCLUDEPATH += /opt/local/include/       #macOS

INCLUDEPATH += $$IFT_DIR/externals/libsvm/include
INCLUDEPATH += $$IFT_DIR/externals/libpng/include
INCLUDEPATH += $$IFT_DIR/externals/libjpeg/include
INCLUDEPATH += $$IFT_DIR/externals/libtiff/include
INCLUDEPATH += $$IFT_DIR/externals/libnifti/include
INCLUDEPATH += $$IFT_DIR/externals/tsne/CPU/include
INCLUDEPATH += $$IFT_DIR/externals/tsne/GPU/src
INCLUDEPATH += $$IFT_DIR/externals/zlib/include
INCLUDEPATH += $$IFT_DIR/externals/ANTs/include
LIBS += -L $$IFT_DIR/lib -lift -llapack -lblas -lm -fopenmp -std=gnu++11

###QMAKE_CC = /opt/local/bin/gcc-mp-5       #macOS
###QMAKE_CXX = /opt/local/bin/g++-mp-5      #macOS
###QMAKE_CXXFLAGS_RELEASE = -pipe -stdlib=libstdc++ -g -std=gnu++11 $(EXPORT_ARCH_ARGS) -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk -mmacosx-version-min=10.10 -Wall -W -fPIC $(DEFINES)         #macOS

QMAKE_CXXFLAGS = -fopenmp -pthread -pedantic -std=c++17 -O3

#GPU STUFFS
INCLUDEPATH += /usr/local/cuda/include
LIBS   += -L/usr/local/cuda/lib64 -lcublas -lcudart

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myicons.qrc

DISTFILES += \
    feature_extraction.py

QMAKE_POST_LINK += $$QMAKE_COPY $$TARGET $$IFT_DIR/bin
