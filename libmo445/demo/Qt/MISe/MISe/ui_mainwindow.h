/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "views/rendering/renderingview.h"
#include "views/slice/sliceview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSegmentation;
    QWidget *centralWidget;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout;
    RenderingView *widgetRendering;
    SliceView *sliceAxial;
    SliceView *sliceCoronal;
    SliceView *sliceSagittal;
    QWidget *vlOptions;
    QVBoxLayout *_vlOptions;
    QGroupBox *gbImageSet;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pbPrev;
    QPushButton *pbNext;
    QPushButton *pbSaveLabel;
    QGroupBox *gpTimeControl;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_6;
    QSlider *sliderTime;
    QLabel *lbTime;
    QGroupBox *gbDisplay;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbZoomIn;
    QPushButton *pbZoomOut;
    QPushButton *pbNormalSize;
    QLabel *lblBrightness;
    QHBoxLayout *horizontalLayout_2;
    QSlider *hsBrightness;
    QLabel *lblBrightCount;
    QLabel *lblContrast;
    QHBoxLayout *horizontalLayout_3;
    QSlider *hsContrast;
    QLabel *lblContrastCount;
    QGroupBox *gbGradient;
    QVBoxLayout *verticalLayout_2;
    QComboBox *cbGradient;
    QVBoxLayout *widgetGradientModule;
    QCheckBox *cbShowGradient;
    QGroupBox *gbSegmentation;
    QVBoxLayout *verticalLayout_3;
    QComboBox *cbSegmentation;
    QFrame *line;
    QVBoxLayout *gbSegmModule;
    QWidget *layoutAnnotation;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *gbAnnotation;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *pbAddMarker;
    QToolButton *pbRemoveMarker;
    QToolButton *pbUndoMarker;
    QLabel *lblAnnotationMode;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QTableWidget *twMarkers;
    QGroupBox *gpObjects;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *cbMarkAll;
    QTableWidget *twObjects;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pbExportObjects;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1231, 895);
        actionSegmentation = new QAction(MainWindow);
        actionSegmentation->setObjectName(QString::fromUtf8("actionSegmentation"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/segmentation.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSegmentation->setIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_9 = new QGridLayout(centralWidget);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widgetRendering = new RenderingView(centralWidget);
        widgetRendering->setObjectName(QString::fromUtf8("widgetRendering"));
        widgetRendering->setMinimumSize(QSize(0, 200));

        gridLayout->addWidget(widgetRendering, 2, 1, 1, 1);

        sliceAxial = new SliceView(centralWidget);
        sliceAxial->setObjectName(QString::fromUtf8("sliceAxial"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sliceAxial->sizePolicy().hasHeightForWidth());
        sliceAxial->setSizePolicy(sizePolicy);
        sliceAxial->setMinimumSize(QSize(200, 200));

        gridLayout->addWidget(sliceAxial, 1, 0, 1, 1);

        sliceCoronal = new SliceView(centralWidget);
        sliceCoronal->setObjectName(QString::fromUtf8("sliceCoronal"));
        sizePolicy.setHeightForWidth(sliceCoronal->sizePolicy().hasHeightForWidth());
        sliceCoronal->setSizePolicy(sizePolicy);
        sliceCoronal->setMinimumSize(QSize(200, 200));
        sliceCoronal->setBaseSize(QSize(0, 0));

        gridLayout->addWidget(sliceCoronal, 1, 1, 1, 1);

        sliceSagittal = new SliceView(centralWidget);
        sliceSagittal->setObjectName(QString::fromUtf8("sliceSagittal"));
        sliceSagittal->setMinimumSize(QSize(200, 200));

        gridLayout->addWidget(sliceSagittal, 2, 0, 1, 1);


        gridLayout_9->addLayout(gridLayout, 0, 1, 1, 1);

        vlOptions = new QWidget(centralWidget);
        vlOptions->setObjectName(QString::fromUtf8("vlOptions"));
        vlOptions->setMaximumSize(QSize(500, 16777215));
        _vlOptions = new QVBoxLayout(vlOptions);
        _vlOptions->setSpacing(0);
        _vlOptions->setContentsMargins(11, 11, 11, 11);
        _vlOptions->setObjectName(QString::fromUtf8("_vlOptions"));
        _vlOptions->setSizeConstraint(QLayout::SetDefaultConstraint);
        _vlOptions->setContentsMargins(2, 1, -1, 1);
        gbImageSet = new QGroupBox(vlOptions);
        gbImageSet->setObjectName(QString::fromUtf8("gbImageSet"));
        horizontalLayout_7 = new QHBoxLayout(gbImageSet);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        pbPrev = new QPushButton(gbImageSet);
        pbPrev->setObjectName(QString::fromUtf8("pbPrev"));

        horizontalLayout_8->addWidget(pbPrev);

        pbNext = new QPushButton(gbImageSet);
        pbNext->setObjectName(QString::fromUtf8("pbNext"));

        horizontalLayout_8->addWidget(pbNext);


        verticalLayout_7->addLayout(horizontalLayout_8);

        pbSaveLabel = new QPushButton(gbImageSet);
        pbSaveLabel->setObjectName(QString::fromUtf8("pbSaveLabel"));

        verticalLayout_7->addWidget(pbSaveLabel);


        horizontalLayout_7->addLayout(verticalLayout_7);


        _vlOptions->addWidget(gbImageSet);

        gpTimeControl = new QGroupBox(vlOptions);
        gpTimeControl->setObjectName(QString::fromUtf8("gpTimeControl"));
        verticalLayout_6 = new QVBoxLayout(gpTimeControl);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        sliderTime = new QSlider(gpTimeControl);
        sliderTime->setObjectName(QString::fromUtf8("sliderTime"));
        sliderTime->setMaximum(10);
        sliderTime->setOrientation(Qt::Horizontal);
        sliderTime->setTickPosition(QSlider::TicksAbove);

        horizontalLayout_6->addWidget(sliderTime);

        lbTime = new QLabel(gpTimeControl);
        lbTime->setObjectName(QString::fromUtf8("lbTime"));

        horizontalLayout_6->addWidget(lbTime);


        verticalLayout_6->addLayout(horizontalLayout_6);


        _vlOptions->addWidget(gpTimeControl);

        gbDisplay = new QGroupBox(vlOptions);
        gbDisplay->setObjectName(QString::fromUtf8("gbDisplay"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(gbDisplay->sizePolicy().hasHeightForWidth());
        gbDisplay->setSizePolicy(sizePolicy1);
        gbDisplay->setMinimumSize(QSize(150, 150));
        gbDisplay->setMaximumSize(QSize(16777215, 200));
        verticalLayout = new QVBoxLayout(gbDisplay);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbZoomIn = new QPushButton(gbDisplay);
        pbZoomIn->setObjectName(QString::fromUtf8("pbZoomIn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../../../../../../Downloads/1452218513_magnifier_zoom_in.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbZoomIn->setIcon(icon1);

        horizontalLayout->addWidget(pbZoomIn);

        pbZoomOut = new QPushButton(gbDisplay);
        pbZoomOut->setObjectName(QString::fromUtf8("pbZoomOut"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("../../../../../../../Downloads/1452218519_magnifier_zoom_out.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbZoomOut->setIcon(icon2);

        horizontalLayout->addWidget(pbZoomOut);

        pbNormalSize = new QPushButton(gbDisplay);
        pbNormalSize->setObjectName(QString::fromUtf8("pbNormalSize"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("../../../../../../../Documents/Contrast_Brightness3.ico"), QSize(), QIcon::Normal, QIcon::Off);
        pbNormalSize->setIcon(icon3);
        pbNormalSize->setIconSize(QSize(64, 16));

        horizontalLayout->addWidget(pbNormalSize);


        verticalLayout->addLayout(horizontalLayout);

        lblBrightness = new QLabel(gbDisplay);
        lblBrightness->setObjectName(QString::fromUtf8("lblBrightness"));

        verticalLayout->addWidget(lblBrightness);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        hsBrightness = new QSlider(gbDisplay);
        hsBrightness->setObjectName(QString::fromUtf8("hsBrightness"));
        hsBrightness->setEnabled(false);
        hsBrightness->setValue(50);
        hsBrightness->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(hsBrightness);

        lblBrightCount = new QLabel(gbDisplay);
        lblBrightCount->setObjectName(QString::fromUtf8("lblBrightCount"));

        horizontalLayout_2->addWidget(lblBrightCount);


        verticalLayout->addLayout(horizontalLayout_2);

        lblContrast = new QLabel(gbDisplay);
        lblContrast->setObjectName(QString::fromUtf8("lblContrast"));

        verticalLayout->addWidget(lblContrast);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        hsContrast = new QSlider(gbDisplay);
        hsContrast->setObjectName(QString::fromUtf8("hsContrast"));
        hsContrast->setEnabled(false);
        hsContrast->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(hsContrast);

        lblContrastCount = new QLabel(gbDisplay);
        lblContrastCount->setObjectName(QString::fromUtf8("lblContrastCount"));

        horizontalLayout_3->addWidget(lblContrastCount);


        verticalLayout->addLayout(horizontalLayout_3);


        _vlOptions->addWidget(gbDisplay);

        gbGradient = new QGroupBox(vlOptions);
        gbGradient->setObjectName(QString::fromUtf8("gbGradient"));
        verticalLayout_2 = new QVBoxLayout(gbGradient);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        cbGradient = new QComboBox(gbGradient);
        cbGradient->setObjectName(QString::fromUtf8("cbGradient"));

        verticalLayout_2->addWidget(cbGradient);

        widgetGradientModule = new QVBoxLayout();
        widgetGradientModule->setSpacing(6);
        widgetGradientModule->setObjectName(QString::fromUtf8("widgetGradientModule"));

        verticalLayout_2->addLayout(widgetGradientModule);

        cbShowGradient = new QCheckBox(gbGradient);
        cbShowGradient->setObjectName(QString::fromUtf8("cbShowGradient"));

        verticalLayout_2->addWidget(cbShowGradient);


        _vlOptions->addWidget(gbGradient);

        gbSegmentation = new QGroupBox(vlOptions);
        gbSegmentation->setObjectName(QString::fromUtf8("gbSegmentation"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(gbSegmentation->sizePolicy().hasHeightForWidth());
        gbSegmentation->setSizePolicy(sizePolicy2);
        verticalLayout_3 = new QVBoxLayout(gbSegmentation);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        cbSegmentation = new QComboBox(gbSegmentation);
        cbSegmentation->setObjectName(QString::fromUtf8("cbSegmentation"));

        verticalLayout_3->addWidget(cbSegmentation);

        line = new QFrame(gbSegmentation);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line);

        gbSegmModule = new QVBoxLayout();
        gbSegmModule->setSpacing(6);
        gbSegmModule->setObjectName(QString::fromUtf8("gbSegmModule"));

        verticalLayout_3->addLayout(gbSegmModule);

        layoutAnnotation = new QWidget(gbSegmentation);
        layoutAnnotation->setObjectName(QString::fromUtf8("layoutAnnotation"));
        verticalLayout_4 = new QVBoxLayout(layoutAnnotation);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        gbAnnotation = new QGroupBox(layoutAnnotation);
        gbAnnotation->setObjectName(QString::fromUtf8("gbAnnotation"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(gbAnnotation->sizePolicy().hasHeightForWidth());
        gbAnnotation->setSizePolicy(sizePolicy3);
        gbAnnotation->setMinimumSize(QSize(0, 70));
        gbAnnotation->setMaximumSize(QSize(16777215, 70));
        horizontalLayout_4 = new QHBoxLayout(gbAnnotation);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pbAddMarker = new QToolButton(gbAnnotation);
        pbAddMarker->setObjectName(QString::fromUtf8("pbAddMarker"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/icons/add.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbAddMarker->setIcon(icon4);

        horizontalLayout_4->addWidget(pbAddMarker);

        pbRemoveMarker = new QToolButton(gbAnnotation);
        pbRemoveMarker->setObjectName(QString::fromUtf8("pbRemoveMarker"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/icons/remove.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbRemoveMarker->setIcon(icon5);

        horizontalLayout_4->addWidget(pbRemoveMarker);

        pbUndoMarker = new QToolButton(gbAnnotation);
        pbUndoMarker->setObjectName(QString::fromUtf8("pbUndoMarker"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/icons/turn.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbUndoMarker->setIcon(icon6);

        horizontalLayout_4->addWidget(pbUndoMarker);

        lblAnnotationMode = new QLabel(gbAnnotation);
        lblAnnotationMode->setObjectName(QString::fromUtf8("lblAnnotationMode"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(lblAnnotationMode->sizePolicy().hasHeightForWidth());
        lblAnnotationMode->setSizePolicy(sizePolicy4);
        lblAnnotationMode->setStyleSheet(QString::fromUtf8("color: rgb(204, 0, 0);"));

        horizontalLayout_4->addWidget(lblAnnotationMode);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout_4->addWidget(gbAnnotation);

        label_2 = new QLabel(layoutAnnotation);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_4->addWidget(label_2);

        twMarkers = new QTableWidget(layoutAnnotation);
        if (twMarkers->columnCount() < 4)
            twMarkers->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        twMarkers->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        twMarkers->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        twMarkers->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        twMarkers->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        twMarkers->setObjectName(QString::fromUtf8("twMarkers"));
        QSizePolicy sizePolicy5(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(twMarkers->sizePolicy().hasHeightForWidth());
        twMarkers->setSizePolicy(sizePolicy5);
        twMarkers->setFocusPolicy(Qt::NoFocus);
        twMarkers->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        twMarkers->horizontalHeader()->setMinimumSectionSize(45);
        twMarkers->horizontalHeader()->setDefaultSectionSize(45);
        twMarkers->horizontalHeader()->setStretchLastSection(false);
        twMarkers->verticalHeader()->setVisible(false);

        verticalLayout_4->addWidget(twMarkers);


        verticalLayout_3->addWidget(layoutAnnotation);


        _vlOptions->addWidget(gbSegmentation);

        gpObjects = new QGroupBox(vlOptions);
        gpObjects->setObjectName(QString::fromUtf8("gpObjects"));
        verticalLayout_5 = new QVBoxLayout(gpObjects);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        cbMarkAll = new QCheckBox(gpObjects);
        cbMarkAll->setObjectName(QString::fromUtf8("cbMarkAll"));
        cbMarkAll->setEnabled(false);
        cbMarkAll->setChecked(true);

        verticalLayout_5->addWidget(cbMarkAll);

        twObjects = new QTableWidget(gpObjects);
        if (twObjects->columnCount() < 6)
            twObjects->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(0, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(1, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(2, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(3, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(4, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        twObjects->setHorizontalHeaderItem(5, __qtablewidgetitem9);
        twObjects->setObjectName(QString::fromUtf8("twObjects"));
        sizePolicy5.setHeightForWidth(twObjects->sizePolicy().hasHeightForWidth());
        twObjects->setSizePolicy(sizePolicy5);
        twObjects->setFocusPolicy(Qt::NoFocus);
        twObjects->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        twObjects->setColumnCount(6);
        twObjects->horizontalHeader()->setCascadingSectionResizes(false);
        twObjects->horizontalHeader()->setMinimumSectionSize(65);
        twObjects->horizontalHeader()->setDefaultSectionSize(65);
        twObjects->horizontalHeader()->setStretchLastSection(false);
        twObjects->verticalHeader()->setVisible(false);

        verticalLayout_5->addWidget(twObjects);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        pbExportObjects = new QPushButton(gpObjects);
        pbExportObjects->setObjectName(QString::fromUtf8("pbExportObjects"));

        horizontalLayout_5->addWidget(pbExportObjects);


        verticalLayout_5->addLayout(horizontalLayout_5);


        _vlOptions->addWidget(gpObjects);


        gridLayout_9->addWidget(vlOptions, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1231, 22));
        menuBar->setMouseTracking(false);
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setMovable(true);
        mainToolBar->setOrientation(Qt::Vertical);
        mainToolBar->setIconSize(QSize(48, 48));
        mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        mainToolBar->setFloatable(true);
        MainWindow->addToolBar(Qt::RightToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MISe", nullptr));
        actionSegmentation->setText(QCoreApplication::translate("MainWindow", "Segmentation", nullptr));
        gbImageSet->setTitle(QCoreApplication::translate("MainWindow", "Image Set (0/0)", nullptr));
        pbPrev->setText(QCoreApplication::translate("MainWindow", "Previous Image", nullptr));
        pbNext->setText(QCoreApplication::translate("MainWindow", "Next Image", nullptr));
        pbSaveLabel->setText(QCoreApplication::translate("MainWindow", "Save Label", nullptr));
        gpTimeControl->setTitle(QCoreApplication::translate("MainWindow", "Time", nullptr));
        lbTime->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        gbDisplay->setTitle(QCoreApplication::translate("MainWindow", "Display", nullptr));
        pbZoomIn->setText(QString());
        pbZoomOut->setText(QString());
        pbNormalSize->setText(QString());
        lblBrightness->setText(QCoreApplication::translate("MainWindow", "Brightness", nullptr));
        lblBrightCount->setText(QCoreApplication::translate("MainWindow", "50", nullptr));
        lblContrast->setText(QCoreApplication::translate("MainWindow", "Contrast", nullptr));
        lblContrastCount->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        gbGradient->setTitle(QCoreApplication::translate("MainWindow", "Arc-Weight Function", nullptr));
        cbShowGradient->setText(QCoreApplication::translate("MainWindow", "Show arc-weights", nullptr));
        gbSegmentation->setTitle(QCoreApplication::translate("MainWindow", "Segmentation", nullptr));
        gbAnnotation->setTitle(QCoreApplication::translate("MainWindow", "Annotation", nullptr));
        pbAddMarker->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        pbRemoveMarker->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        pbUndoMarker->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        lblAnnotationMode->setText(QCoreApplication::translate("MainWindow", "Status: Halted", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "List of Objects", nullptr));
        QTableWidgetItem *___qtablewidgetitem = twMarkers->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "#", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = twMarkers->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Color", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = twMarkers->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Object", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = twMarkers->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        gpObjects->setTitle(QCoreApplication::translate("MainWindow", "List Of Segmented Objects", nullptr));
        cbMarkAll->setText(QCoreApplication::translate("MainWindow", "Mark/Unmark all", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = twObjects->horizontalHeaderItem(0);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "#", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = twObjects->horizontalHeaderItem(1);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Color", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = twObjects->horizontalHeaderItem(2);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Object", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = twObjects->horizontalHeaderItem(3);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Opac.", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = twObjects->horizontalHeaderItem(4);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Area (units\302\262)", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = twObjects->horizontalHeaderItem(5);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        pbExportObjects->setText(QCoreApplication::translate("MainWindow", "Export", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
