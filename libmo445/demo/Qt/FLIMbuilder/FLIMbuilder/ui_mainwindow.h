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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "mycombobox.h"
#include "mygraphicsview.h"
#include "myplaintextedit.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_Project;
    QAction *actionOpen_Project;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QFrame *LeftFrame;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pbLoadArch;
    QPushButton *pbSaveArch;
    QSpacerItem *horizontalSpacer_3;
    MyPlainTextEdit *teFLIM_Arch;
    QFrame *frame_6;
    QLabel *label_6;
    QFormLayout *formLayout;
    QLabel *lblModelDir;
    QLabel *lblGPU;
    QSpinBox *sbDevice;
    QLabel *lblTraining;
    MyComboBox *cbSelectTrainingLayers;
    QLabel *lblRunType;
    QComboBox *cbRunMode;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *leModelDir;
    QPushButton *pbOpenModelDir;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pbRun;
    QFrame *centralFrame;
    QVBoxLayout *verticalLayout_5;
    MyGraphicsView *gvDisplay;
    QHBoxLayout *horizontalLayout_11;
    QLabel *lblIntensityAtPoint;
    QFrame *frame_8;
    QLabel *lblActivationAtPoint;
    QSpacerItem *horizontalSpacer_2;
    QFrame *ImageSetupFrame;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout_3;
    QPushButton *pbZoomIn;
    QPushButton *pbZoomOut;
    QPushButton *pbFitWindow;
    QPushButton *pbOriginalSize;
    QVBoxLayout *verticalLayout_6;
    QLabel *lblContrast;
    QSlider *hsContrast;
    QVBoxLayout *verticalLayout_8;
    QLabel *lblBrightness;
    QSlider *hsBrightness;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_7;
    QComboBox *cb3DPlane;
    QHBoxLayout *horizontalLayout_4;
    QLabel *lblSlice;
    QSpinBox *sbSliceNum;
    QHBoxLayout *horizontalLayout_9;
    QLabel *lblBand;
    QSpinBox *sbBandNum;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pbAddMarker;
    QPushButton *pbRemoveMarker;
    QWidget *wBrush;
    QSpinBox *sbBrush;
    QWidget *wMarkerColor;
    QComboBox *cbMarkers;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_9;
    QFormLayout *formLayout_2;
    QLabel *label_5;
    QComboBox *cbMarkersVisibility;
    QLabel *label_8;
    QComboBox *cbActivationVisibility;
    QComboBox *cbGtVisibility;
    QLabel *label_10;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_7;
    QLabel *lblSize;
    QLabel *lblMinMax;
    QLabel *lblBPP;
    QSpacerItem *horizontalSpacer_6;
    QFrame *RightFrame;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pbOpenTrainFolder;
    QFrame *frame_5;
    QPushButton *pbOpenMarkers;
    QPushButton *pbSaveMarkers;
    QPushButton *pbRemoveDrawnMarkers;
    QFrame *frame_9;
    QPushButton *pbLoadGroundTruth;
    QPushButton *pbRemoveGroundTruth;
    QSpacerItem *horizontalSpacer_5;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_12;
    QListWidget *lwTrain;
    QVBoxLayout *verticalLayout_10;
    QLabel *lblNumMarkers;
    QListWidget *lwListOfMarkers;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_10;
    QComboBox *cbLayer;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbInvertKernels;
    QPushButton *pbProjectKernels;
    QPushButton *pbSelectKernelsManual;
    QListWidget *lwActivation;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1654, 675);
        MainWindow->setToolButtonStyle(Qt::ToolButtonIconOnly);
        actionLoad_Project = new QAction(MainWindow);
        actionLoad_Project->setObjectName(QString::fromUtf8("actionLoad_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        LeftFrame = new QFrame(centralWidget);
        LeftFrame->setObjectName(QString::fromUtf8("LeftFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LeftFrame->sizePolicy().hasHeightForWidth());
        LeftFrame->setSizePolicy(sizePolicy);
        LeftFrame->setFrameShape(QFrame::Box);
        verticalLayout_3 = new QVBoxLayout(LeftFrame);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(LeftFrame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(label_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pbLoadArch = new QPushButton(LeftFrame);
        pbLoadArch->setObjectName(QString::fromUtf8("pbLoadArch"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pbLoadArch->sizePolicy().hasHeightForWidth());
        pbLoadArch->setSizePolicy(sizePolicy2);
        pbLoadArch->setToolTipDuration(10000);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/open_folder.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbLoadArch->setIcon(icon);

        horizontalLayout_7->addWidget(pbLoadArch);

        pbSaveArch = new QPushButton(LeftFrame);
        pbSaveArch->setObjectName(QString::fromUtf8("pbSaveArch"));
        sizePolicy2.setHeightForWidth(pbSaveArch->sizePolicy().hasHeightForWidth());
        pbSaveArch->setSizePolicy(sizePolicy2);
        pbSaveArch->setToolTipDuration(10000);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/save2.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbSaveArch->setIcon(icon1);

        horizontalLayout_7->addWidget(pbSaveArch);

        horizontalSpacer_3 = new QSpacerItem(265, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_3);


        verticalLayout_3->addLayout(horizontalLayout_7);

        teFLIM_Arch = new MyPlainTextEdit(LeftFrame);
        teFLIM_Arch->setObjectName(QString::fromUtf8("teFLIM_Arch"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(teFLIM_Arch->sizePolicy().hasHeightForWidth());
        teFLIM_Arch->setSizePolicy(sizePolicy3);
        teFLIM_Arch->setTabStopDistance(14.000000000000000);

        verticalLayout_3->addWidget(teFLIM_Arch);

        frame_6 = new QFrame(LeftFrame);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setFrameShape(QFrame::HLine);
        frame_6->setFrameShadow(QFrame::Raised);

        verticalLayout_3->addWidget(frame_6);

        label_6 = new QLabel(LeftFrame);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(label_6);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lblModelDir = new QLabel(LeftFrame);
        lblModelDir->setObjectName(QString::fromUtf8("lblModelDir"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lblModelDir);

        lblGPU = new QLabel(LeftFrame);
        lblGPU->setObjectName(QString::fromUtf8("lblGPU"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lblGPU);

        sbDevice = new QSpinBox(LeftFrame);
        sbDevice->setObjectName(QString::fromUtf8("sbDevice"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(sbDevice->sizePolicy().hasHeightForWidth());
        sbDevice->setSizePolicy(sizePolicy4);
        sbDevice->setToolTipDuration(10000);
        sbDevice->setMinimum(-1);
        sbDevice->setValue(-1);

        formLayout->setWidget(1, QFormLayout::FieldRole, sbDevice);

        lblTraining = new QLabel(LeftFrame);
        lblTraining->setObjectName(QString::fromUtf8("lblTraining"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lblTraining);

        cbSelectTrainingLayers = new MyComboBox(LeftFrame);
        cbSelectTrainingLayers->setObjectName(QString::fromUtf8("cbSelectTrainingLayers"));
        cbSelectTrainingLayers->setToolTipDuration(10000);

        formLayout->setWidget(2, QFormLayout::FieldRole, cbSelectTrainingLayers);

        lblRunType = new QLabel(LeftFrame);
        lblRunType->setObjectName(QString::fromUtf8("lblRunType"));

        formLayout->setWidget(3, QFormLayout::LabelRole, lblRunType);

        cbRunMode = new QComboBox(LeftFrame);
        cbRunMode->addItem(QString());
        cbRunMode->addItem(QString());
        cbRunMode->addItem(QString());
        cbRunMode->setObjectName(QString::fromUtf8("cbRunMode"));
        cbRunMode->setEnabled(true);
        cbRunMode->setToolTipDuration(10000);

        formLayout->setWidget(3, QFormLayout::FieldRole, cbRunMode);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        leModelDir = new QLineEdit(LeftFrame);
        leModelDir->setObjectName(QString::fromUtf8("leModelDir"));
        leModelDir->setEnabled(false);
        sizePolicy4.setHeightForWidth(leModelDir->sizePolicy().hasHeightForWidth());
        leModelDir->setSizePolicy(sizePolicy4);
        leModelDir->setToolTipDuration(10000);

        horizontalLayout_8->addWidget(leModelDir);

        pbOpenModelDir = new QPushButton(LeftFrame);
        pbOpenModelDir->setObjectName(QString::fromUtf8("pbOpenModelDir"));
        sizePolicy2.setHeightForWidth(pbOpenModelDir->sizePolicy().hasHeightForWidth());
        pbOpenModelDir->setSizePolicy(sizePolicy2);
        pbOpenModelDir->setMinimumSize(QSize(10, 0));
        pbOpenModelDir->setMaximumSize(QSize(16, 16777215));
        pbOpenModelDir->setToolTipDuration(10000);

        horizontalLayout_8->addWidget(pbOpenModelDir);


        formLayout->setLayout(0, QFormLayout::FieldRole, horizontalLayout_8);


        verticalLayout_3->addLayout(formLayout);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_4 = new QSpacerItem(130, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        pbRun = new QPushButton(LeftFrame);
        pbRun->setObjectName(QString::fromUtf8("pbRun"));
        sizePolicy2.setHeightForWidth(pbRun->sizePolicy().hasHeightForWidth());
        pbRun->setSizePolicy(sizePolicy2);
        pbRun->setToolTipDuration(10000);

        horizontalLayout_6->addWidget(pbRun);


        verticalLayout_3->addLayout(horizontalLayout_6);


        horizontalLayout->addWidget(LeftFrame);

        centralFrame = new QFrame(centralWidget);
        centralFrame->setObjectName(QString::fromUtf8("centralFrame"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(centralFrame->sizePolicy().hasHeightForWidth());
        centralFrame->setSizePolicy(sizePolicy5);
        centralFrame->setFrameShape(QFrame::Box);
        verticalLayout_5 = new QVBoxLayout(centralFrame);
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        gvDisplay = new MyGraphicsView(centralFrame);
        gvDisplay->setObjectName(QString::fromUtf8("gvDisplay"));

        verticalLayout_5->addWidget(gvDisplay);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        lblIntensityAtPoint = new QLabel(centralFrame);
        lblIntensityAtPoint->setObjectName(QString::fromUtf8("lblIntensityAtPoint"));

        horizontalLayout_11->addWidget(lblIntensityAtPoint);

        frame_8 = new QFrame(centralFrame);
        frame_8->setObjectName(QString::fromUtf8("frame_8"));
        frame_8->setFrameShape(QFrame::VLine);
        frame_8->setFrameShadow(QFrame::Raised);

        horizontalLayout_11->addWidget(frame_8);

        lblActivationAtPoint = new QLabel(centralFrame);
        lblActivationAtPoint->setObjectName(QString::fromUtf8("lblActivationAtPoint"));

        horizontalLayout_11->addWidget(lblActivationAtPoint);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_11);

        ImageSetupFrame = new QFrame(centralFrame);
        ImageSetupFrame->setObjectName(QString::fromUtf8("ImageSetupFrame"));
        ImageSetupFrame->setFrameShape(QFrame::Box);
        ImageSetupFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(ImageSetupFrame);
        horizontalLayout_3->setSpacing(3);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(1, 1, 1, 1);
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        pbZoomIn = new QPushButton(ImageSetupFrame);
        pbZoomIn->setObjectName(QString::fromUtf8("pbZoomIn"));
        pbZoomIn->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbZoomIn->sizePolicy().hasHeightForWidth());
        pbZoomIn->setSizePolicy(sizePolicy2);
        pbZoomIn->setToolTipDuration(10000);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/zoom_in.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbZoomIn->setIcon(icon2);

        gridLayout_3->addWidget(pbZoomIn, 0, 0, 1, 1);

        pbZoomOut = new QPushButton(ImageSetupFrame);
        pbZoomOut->setObjectName(QString::fromUtf8("pbZoomOut"));
        pbZoomOut->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbZoomOut->sizePolicy().hasHeightForWidth());
        pbZoomOut->setSizePolicy(sizePolicy2);
        pbZoomOut->setToolTipDuration(10000);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/zoom_out.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbZoomOut->setIcon(icon3);

        gridLayout_3->addWidget(pbZoomOut, 0, 1, 1, 1);

        pbFitWindow = new QPushButton(ImageSetupFrame);
        pbFitWindow->setObjectName(QString::fromUtf8("pbFitWindow"));
        pbFitWindow->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbFitWindow->sizePolicy().hasHeightForWidth());
        pbFitWindow->setSizePolicy(sizePolicy2);
        pbFitWindow->setToolTipDuration(10000);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/expand.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbFitWindow->setIcon(icon4);

        gridLayout_3->addWidget(pbFitWindow, 1, 0, 1, 1);

        pbOriginalSize = new QPushButton(ImageSetupFrame);
        pbOriginalSize->setObjectName(QString::fromUtf8("pbOriginalSize"));
        pbOriginalSize->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbOriginalSize->sizePolicy().hasHeightForWidth());
        pbOriginalSize->setSizePolicy(sizePolicy2);
        pbOriginalSize->setToolTipDuration(10000);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/shrink.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbOriginalSize->setIcon(icon5);

        gridLayout_3->addWidget(pbOriginalSize, 1, 1, 1, 1);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(1);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        lblContrast = new QLabel(ImageSetupFrame);
        lblContrast->setObjectName(QString::fromUtf8("lblContrast"));

        verticalLayout_6->addWidget(lblContrast);

        hsContrast = new QSlider(ImageSetupFrame);
        hsContrast->setObjectName(QString::fromUtf8("hsContrast"));
        hsContrast->setEnabled(false);
        sizePolicy4.setHeightForWidth(hsContrast->sizePolicy().hasHeightForWidth());
        hsContrast->setSizePolicy(sizePolicy4);
        hsContrast->setMinimum(1);
        hsContrast->setMaximum(100);
        hsContrast->setValue(1);
        hsContrast->setTracking(true);
        hsContrast->setOrientation(Qt::Horizontal);

        verticalLayout_6->addWidget(hsContrast);


        gridLayout_3->addLayout(verticalLayout_6, 1, 2, 1, 1);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(1);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        lblBrightness = new QLabel(ImageSetupFrame);
        lblBrightness->setObjectName(QString::fromUtf8("lblBrightness"));

        verticalLayout_8->addWidget(lblBrightness);

        hsBrightness = new QSlider(ImageSetupFrame);
        hsBrightness->setObjectName(QString::fromUtf8("hsBrightness"));
        hsBrightness->setEnabled(false);
        sizePolicy4.setHeightForWidth(hsBrightness->sizePolicy().hasHeightForWidth());
        hsBrightness->setSizePolicy(sizePolicy4);
        hsBrightness->setMinimum(1);
        hsBrightness->setMaximum(100);
        hsBrightness->setValue(50);
        hsBrightness->setTracking(true);
        hsBrightness->setOrientation(Qt::Horizontal);

        verticalLayout_8->addWidget(hsBrightness);


        gridLayout_3->addLayout(verticalLayout_8, 0, 2, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_3);

        frame_2 = new QFrame(ImageSetupFrame);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::VLine);
        frame_2->setFrameShadow(QFrame::Raised);

        horizontalLayout_3->addWidget(frame_2);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(1);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        cb3DPlane = new QComboBox(ImageSetupFrame);
        cb3DPlane->addItem(QString());
        cb3DPlane->addItem(QString());
        cb3DPlane->addItem(QString());
        cb3DPlane->setObjectName(QString::fromUtf8("cb3DPlane"));
        cb3DPlane->setEnabled(false);
        cb3DPlane->setToolTipDuration(10000);

        verticalLayout_7->addWidget(cb3DPlane);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        lblSlice = new QLabel(ImageSetupFrame);
        lblSlice->setObjectName(QString::fromUtf8("lblSlice"));

        horizontalLayout_4->addWidget(lblSlice);

        sbSliceNum = new QSpinBox(ImageSetupFrame);
        sbSliceNum->setObjectName(QString::fromUtf8("sbSliceNum"));
        sbSliceNum->setEnabled(false);
        sbSliceNum->setToolTipDuration(10000);

        horizontalLayout_4->addWidget(sbSliceNum);


        verticalLayout_7->addLayout(horizontalLayout_4);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        lblBand = new QLabel(ImageSetupFrame);
        lblBand->setObjectName(QString::fromUtf8("lblBand"));

        horizontalLayout_9->addWidget(lblBand);

        sbBandNum = new QSpinBox(ImageSetupFrame);
        sbBandNum->setObjectName(QString::fromUtf8("sbBandNum"));
        sbBandNum->setEnabled(false);

        horizontalLayout_9->addWidget(sbBandNum);


        verticalLayout_7->addLayout(horizontalLayout_9);


        horizontalLayout_3->addLayout(verticalLayout_7);

        frame = new QFrame(ImageSetupFrame);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::VLine);
        frame->setFrameShadow(QFrame::Raised);

        horizontalLayout_3->addWidget(frame);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_4 = new QLabel(ImageSetupFrame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy6);
        label_4->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(label_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 5, -1, 5);
        pbAddMarker = new QPushButton(ImageSetupFrame);
        pbAddMarker->setObjectName(QString::fromUtf8("pbAddMarker"));
        sizePolicy2.setHeightForWidth(pbAddMarker->sizePolicy().hasHeightForWidth());
        pbAddMarker->setSizePolicy(sizePolicy2);
        pbAddMarker->setToolTipDuration(10000);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/plus.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbAddMarker->setIcon(icon6);

        horizontalLayout_2->addWidget(pbAddMarker);

        pbRemoveMarker = new QPushButton(ImageSetupFrame);
        pbRemoveMarker->setObjectName(QString::fromUtf8("pbRemoveMarker"));
        sizePolicy2.setHeightForWidth(pbRemoveMarker->sizePolicy().hasHeightForWidth());
        pbRemoveMarker->setSizePolicy(sizePolicy2);
        pbRemoveMarker->setToolTipDuration(10000);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/minus.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbRemoveMarker->setIcon(icon7);

        horizontalLayout_2->addWidget(pbRemoveMarker);

        wBrush = new QWidget(ImageSetupFrame);
        wBrush->setObjectName(QString::fromUtf8("wBrush"));
        sizePolicy1.setHeightForWidth(wBrush->sizePolicy().hasHeightForWidth());
        wBrush->setSizePolicy(sizePolicy1);
        wBrush->setMinimumSize(QSize(35, 0));
        wBrush->setToolTipDuration(10000);
        wBrush->setStyleSheet(QString::fromUtf8("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.25, fx:0.5, fy:0.5, stop:0.970149 rgba(0, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"));

        horizontalLayout_2->addWidget(wBrush);

        sbBrush = new QSpinBox(ImageSetupFrame);
        sbBrush->setObjectName(QString::fromUtf8("sbBrush"));
        sbBrush->setToolTipDuration(10000);
        sbBrush->setWrapping(false);
        sbBrush->setFrame(true);
        sbBrush->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        sbBrush->setProperty("showGroupSeparator", QVariant(false));
        sbBrush->setMinimum(1);
        sbBrush->setMaximum(10);
        sbBrush->setValue(5);

        horizontalLayout_2->addWidget(sbBrush);

        wMarkerColor = new QWidget(ImageSetupFrame);
        wMarkerColor->setObjectName(QString::fromUtf8("wMarkerColor"));
        wMarkerColor->setMinimumSize(QSize(35, 0));

        horizontalLayout_2->addWidget(wMarkerColor);


        verticalLayout->addLayout(horizontalLayout_2);

        cbMarkers = new QComboBox(ImageSetupFrame);
        cbMarkers->setObjectName(QString::fromUtf8("cbMarkers"));
        sizePolicy4.setHeightForWidth(cbMarkers->sizePolicy().hasHeightForWidth());
        cbMarkers->setSizePolicy(sizePolicy4);
        cbMarkers->setToolTipDuration(10000);
        cbMarkers->setInsertPolicy(QComboBox::InsertAlphabetically);

        verticalLayout->addWidget(cbMarkers);


        horizontalLayout_3->addLayout(verticalLayout);

        frame_3 = new QFrame(ImageSetupFrame);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::VLine);
        frame_3->setFrameShadow(QFrame::Raised);

        horizontalLayout_3->addWidget(frame_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_9 = new QLabel(ImageSetupFrame);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        sizePolicy6.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy6);
        label_9->setMaximumSize(QSize(16777215, 20));

        verticalLayout_2->addWidget(label_9);

        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setVerticalSpacing(2);
        label_5 = new QLabel(ImageSetupFrame);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_5);

        cbMarkersVisibility = new QComboBox(ImageSetupFrame);
        cbMarkersVisibility->addItem(QString());
        cbMarkersVisibility->addItem(QString());
        cbMarkersVisibility->addItem(QString());
        cbMarkersVisibility->setObjectName(QString::fromUtf8("cbMarkersVisibility"));
        cbMarkersVisibility->setToolTipDuration(10000);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, cbMarkersVisibility);

        label_8 = new QLabel(ImageSetupFrame);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_8);

        cbActivationVisibility = new QComboBox(ImageSetupFrame);
        cbActivationVisibility->addItem(QString());
        cbActivationVisibility->addItem(QString());
        cbActivationVisibility->addItem(QString());
        cbActivationVisibility->setObjectName(QString::fromUtf8("cbActivationVisibility"));
        cbActivationVisibility->setToolTipDuration(10000);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, cbActivationVisibility);

        cbGtVisibility = new QComboBox(ImageSetupFrame);
        cbGtVisibility->addItem(QString());
        cbGtVisibility->addItem(QString());
        cbGtVisibility->addItem(QString());
        cbGtVisibility->setObjectName(QString::fromUtf8("cbGtVisibility"));
        cbGtVisibility->setEnabled(false);
        cbGtVisibility->setToolTipDuration(10000);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, cbGtVisibility);

        label_10 = new QLabel(ImageSetupFrame);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_10);


        verticalLayout_2->addLayout(formLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_2);

        frame_4 = new QFrame(ImageSetupFrame);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setFrameShape(QFrame::VLine);
        frame_4->setFrameShadow(QFrame::Raised);

        horizontalLayout_3->addWidget(frame_4);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        label_7 = new QLabel(ImageSetupFrame);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_9->addWidget(label_7);

        lblSize = new QLabel(ImageSetupFrame);
        lblSize->setObjectName(QString::fromUtf8("lblSize"));

        verticalLayout_9->addWidget(lblSize);

        lblMinMax = new QLabel(ImageSetupFrame);
        lblMinMax->setObjectName(QString::fromUtf8("lblMinMax"));

        verticalLayout_9->addWidget(lblMinMax);

        lblBPP = new QLabel(ImageSetupFrame);
        lblBPP->setObjectName(QString::fromUtf8("lblBPP"));

        verticalLayout_9->addWidget(lblBPP);


        horizontalLayout_3->addLayout(verticalLayout_9);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_6);


        verticalLayout_5->addWidget(ImageSetupFrame);


        horizontalLayout->addWidget(centralFrame);

        RightFrame = new QFrame(centralWidget);
        RightFrame->setObjectName(QString::fromUtf8("RightFrame"));
        sizePolicy.setHeightForWidth(RightFrame->sizePolicy().hasHeightForWidth());
        RightFrame->setSizePolicy(sizePolicy);
        RightFrame->setMinimumSize(QSize(0, 0));
        RightFrame->setFrameShape(QFrame::Box);
        verticalLayout_4 = new QVBoxLayout(RightFrame);
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(RightFrame);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy7(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy7);

        verticalLayout_4->addWidget(label);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setSizeConstraint(QLayout::SetDefaultConstraint);
        pbOpenTrainFolder = new QPushButton(RightFrame);
        pbOpenTrainFolder->setObjectName(QString::fromUtf8("pbOpenTrainFolder"));
        sizePolicy2.setHeightForWidth(pbOpenTrainFolder->sizePolicy().hasHeightForWidth());
        pbOpenTrainFolder->setSizePolicy(sizePolicy2);
        pbOpenTrainFolder->setToolTipDuration(10000);
        pbOpenTrainFolder->setIcon(icon);

        horizontalLayout_5->addWidget(pbOpenTrainFolder);

        frame_5 = new QFrame(RightFrame);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setFrameShape(QFrame::VLine);
        frame_5->setFrameShadow(QFrame::Raised);

        horizontalLayout_5->addWidget(frame_5);

        pbOpenMarkers = new QPushButton(RightFrame);
        pbOpenMarkers->setObjectName(QString::fromUtf8("pbOpenMarkers"));
        pbOpenMarkers->setToolTipDuration(10000);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/draw.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbOpenMarkers->setIcon(icon8);

        horizontalLayout_5->addWidget(pbOpenMarkers);

        pbSaveMarkers = new QPushButton(RightFrame);
        pbSaveMarkers->setObjectName(QString::fromUtf8("pbSaveMarkers"));
        pbSaveMarkers->setToolTipDuration(10000);
        pbSaveMarkers->setIcon(icon1);

        horizontalLayout_5->addWidget(pbSaveMarkers);

        pbRemoveDrawnMarkers = new QPushButton(RightFrame);
        pbRemoveDrawnMarkers->setObjectName(QString::fromUtf8("pbRemoveDrawnMarkers"));
        sizePolicy2.setHeightForWidth(pbRemoveDrawnMarkers->sizePolicy().hasHeightForWidth());
        pbRemoveDrawnMarkers->setSizePolicy(sizePolicy2);
        pbRemoveDrawnMarkers->setMinimumSize(QSize(16, 16));
        pbRemoveDrawnMarkers->setToolTipDuration(10000);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbRemoveDrawnMarkers->setIcon(icon9);
        pbRemoveDrawnMarkers->setIconSize(QSize(18, 18));

        horizontalLayout_5->addWidget(pbRemoveDrawnMarkers);

        frame_9 = new QFrame(RightFrame);
        frame_9->setObjectName(QString::fromUtf8("frame_9"));
        frame_9->setFrameShape(QFrame::VLine);
        frame_9->setFrameShadow(QFrame::Raised);

        horizontalLayout_5->addWidget(frame_9);

        pbLoadGroundTruth = new QPushButton(RightFrame);
        pbLoadGroundTruth->setObjectName(QString::fromUtf8("pbLoadGroundTruth"));
        pbLoadGroundTruth->setToolTipDuration(10000);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/icons/gold.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbLoadGroundTruth->setIcon(icon10);
        pbLoadGroundTruth->setIconSize(QSize(18, 18));

        horizontalLayout_5->addWidget(pbLoadGroundTruth);

        pbRemoveGroundTruth = new QPushButton(RightFrame);
        pbRemoveGroundTruth->setObjectName(QString::fromUtf8("pbRemoveGroundTruth"));
        pbRemoveGroundTruth->setToolTipDuration(10000);
        pbRemoveGroundTruth->setIcon(icon9);
        pbRemoveGroundTruth->setIconSize(QSize(18, 18));

        horizontalLayout_5->addWidget(pbRemoveGroundTruth);

        horizontalSpacer_5 = new QSpacerItem(150, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout_4->addLayout(horizontalLayout_5);

        frame_7 = new QFrame(RightFrame);
        frame_7->setObjectName(QString::fromUtf8("frame_7"));
        frame_7->setFrameShape(QFrame::HLine);
        frame_7->setFrameShadow(QFrame::Raised);

        verticalLayout_4->addWidget(frame_7);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        lwTrain = new QListWidget(RightFrame);
        lwTrain->setObjectName(QString::fromUtf8("lwTrain"));
        sizePolicy3.setHeightForWidth(lwTrain->sizePolicy().hasHeightForWidth());
        lwTrain->setSizePolicy(sizePolicy3);

        horizontalLayout_12->addWidget(lwTrain);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(0);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        lblNumMarkers = new QLabel(RightFrame);
        lblNumMarkers->setObjectName(QString::fromUtf8("lblNumMarkers"));
        lblNumMarkers->setMargin(0);

        verticalLayout_10->addWidget(lblNumMarkers);

        lwListOfMarkers = new QListWidget(RightFrame);
        lwListOfMarkers->setObjectName(QString::fromUtf8("lwListOfMarkers"));
        QSizePolicy sizePolicy8(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(lwListOfMarkers->sizePolicy().hasHeightForWidth());
        lwListOfMarkers->setSizePolicy(sizePolicy8);
        lwListOfMarkers->setMaximumSize(QSize(120, 16777215));

        verticalLayout_10->addWidget(lwListOfMarkers);


        horizontalLayout_12->addLayout(verticalLayout_10);


        verticalLayout_4->addLayout(horizontalLayout_12);

        label_2 = new QLabel(RightFrame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy7.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy7);

        verticalLayout_4->addWidget(label_2);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        cbLayer = new QComboBox(RightFrame);
        cbLayer->setObjectName(QString::fromUtf8("cbLayer"));
        sizePolicy4.setHeightForWidth(cbLayer->sizePolicy().hasHeightForWidth());
        cbLayer->setSizePolicy(sizePolicy4);
        cbLayer->setToolTipDuration(10000);

        horizontalLayout_10->addWidget(cbLayer);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer);

        pbInvertKernels = new QPushButton(RightFrame);
        pbInvertKernels->setObjectName(QString::fromUtf8("pbInvertKernels"));
        sizePolicy2.setHeightForWidth(pbInvertKernels->sizePolicy().hasHeightForWidth());
        pbInvertKernels->setSizePolicy(sizePolicy2);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/turn.svg"), QSize(), QIcon::Normal, QIcon::Off);
        pbInvertKernels->setIcon(icon11);

        horizontalLayout_10->addWidget(pbInvertKernels);

        pbProjectKernels = new QPushButton(RightFrame);
        pbProjectKernels->setObjectName(QString::fromUtf8("pbProjectKernels"));
        pbProjectKernels->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbProjectKernels->sizePolicy().hasHeightForWidth());
        pbProjectKernels->setSizePolicy(sizePolicy2);
        pbProjectKernels->setToolTipDuration(10000);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/icons/scatter.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbProjectKernels->setIcon(icon12);

        horizontalLayout_10->addWidget(pbProjectKernels);

        pbSelectKernelsManual = new QPushButton(RightFrame);
        pbSelectKernelsManual->setObjectName(QString::fromUtf8("pbSelectKernelsManual"));
        pbSelectKernelsManual->setEnabled(false);
        sizePolicy2.setHeightForWidth(pbSelectKernelsManual->sizePolicy().hasHeightForWidth());
        pbSelectKernelsManual->setSizePolicy(sizePolicy2);
        pbSelectKernelsManual->setToolTipDuration(10000);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/icons/select.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbSelectKernelsManual->setIcon(icon13);

        horizontalLayout_10->addWidget(pbSelectKernelsManual);


        verticalLayout_4->addLayout(horizontalLayout_10);

        lwActivation = new QListWidget(RightFrame);
        lwActivation->setObjectName(QString::fromUtf8("lwActivation"));
        sizePolicy3.setHeightForWidth(lwActivation->sizePolicy().hasHeightForWidth());
        lwActivation->setSizePolicy(sizePolicy3);
        lwActivation->setMouseTracking(true);
        lwActivation->setToolTipDuration(10000);
        lwActivation->setProperty("showDropIndicator", QVariant(false));
        lwActivation->setDefaultDropAction(Qt::IgnoreAction);

        verticalLayout_4->addWidget(lwActivation);


        horizontalLayout->addWidget(RightFrame);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1654, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionLoad_Project->setText(QCoreApplication::translate("MainWindow", "Load Project", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "FLIM Architecture", nullptr));
#if QT_CONFIG(tooltip)
        pbLoadArch->setToolTip(QCoreApplication::translate("MainWindow", "Open FLIM Architecture", nullptr));
#endif // QT_CONFIG(tooltip)
        pbLoadArch->setText(QString());
#if QT_CONFIG(tooltip)
        pbSaveArch->setToolTip(QCoreApplication::translate("MainWindow", "Save FLIM Architecture", nullptr));
#endif // QT_CONFIG(tooltip)
        pbSaveArch->setText(QString());
        label_6->setText(QCoreApplication::translate("MainWindow", "FLIM Setup", nullptr));
        lblModelDir->setText(QCoreApplication::translate("MainWindow", "Model Directory", nullptr));
#if QT_CONFIG(tooltip)
        lblGPU->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p>Leave -1 to run in CPU</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        lblGPU->setText(QCoreApplication::translate("MainWindow", "GPU Device Index", nullptr));
#if QT_CONFIG(tooltip)
        sbDevice->setToolTip(QCoreApplication::translate("MainWindow", "Select GPU device", nullptr));
#endif // QT_CONFIG(tooltip)
        lblTraining->setText(QCoreApplication::translate("MainWindow", "Training", nullptr));
#if QT_CONFIG(tooltip)
        cbSelectTrainingLayers->setToolTip(QCoreApplication::translate("MainWindow", "Select Layers for Training and Feature Extraction", nullptr));
#endif // QT_CONFIG(tooltip)
        lblRunType->setText(QCoreApplication::translate("MainWindow", "Run Type", nullptr));
        cbRunMode->setItemText(0, QCoreApplication::translate("MainWindow", "Train and Extract Features", nullptr));
        cbRunMode->setItemText(1, QCoreApplication::translate("MainWindow", "Extract Features", nullptr));
        cbRunMode->setItemText(2, QCoreApplication::translate("MainWindow", "Interactive Filter Selection", nullptr));

#if QT_CONFIG(tooltip)
        cbRunMode->setToolTip(QCoreApplication::translate("MainWindow", "Select between train and extract features", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        leModelDir->setToolTip(QCoreApplication::translate("MainWindow", "Define directory to save the model", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pbOpenModelDir->setToolTip(QCoreApplication::translate("MainWindow", "Select directory to store FLIM model", nullptr));
#endif // QT_CONFIG(tooltip)
        pbOpenModelDir->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
#if QT_CONFIG(tooltip)
        pbRun->setToolTip(QCoreApplication::translate("MainWindow", "Train FLIM network", nullptr));
#endif // QT_CONFIG(tooltip)
        pbRun->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        lblIntensityAtPoint->setText(QString());
        lblActivationAtPoint->setText(QString());
#if QT_CONFIG(tooltip)
        pbZoomIn->setToolTip(QCoreApplication::translate("MainWindow", "Zoom in", nullptr));
#endif // QT_CONFIG(tooltip)
        pbZoomIn->setText(QString());
#if QT_CONFIG(tooltip)
        pbZoomOut->setToolTip(QCoreApplication::translate("MainWindow", "Zoom out", nullptr));
#endif // QT_CONFIG(tooltip)
        pbZoomOut->setText(QString());
#if QT_CONFIG(tooltip)
        pbFitWindow->setToolTip(QCoreApplication::translate("MainWindow", "Fit image in canvas", nullptr));
#endif // QT_CONFIG(tooltip)
        pbFitWindow->setText(QString());
#if QT_CONFIG(tooltip)
        pbOriginalSize->setToolTip(QCoreApplication::translate("MainWindow", "Return image to original size", nullptr));
#endif // QT_CONFIG(tooltip)
        pbOriginalSize->setText(QString());
        lblContrast->setText(QCoreApplication::translate("MainWindow", "Contrast", nullptr));
        lblBrightness->setText(QCoreApplication::translate("MainWindow", "Brightness", nullptr));
        cb3DPlane->setItemText(0, QCoreApplication::translate("MainWindow", "Axial", nullptr));
        cb3DPlane->setItemText(1, QCoreApplication::translate("MainWindow", "Coronal", nullptr));
        cb3DPlane->setItemText(2, QCoreApplication::translate("MainWindow", "Sagittal", nullptr));

#if QT_CONFIG(tooltip)
        cb3DPlane->setToolTip(QCoreApplication::translate("MainWindow", "Select image plane", nullptr));
#endif // QT_CONFIG(tooltip)
        lblSlice->setText(QCoreApplication::translate("MainWindow", "Slice", nullptr));
#if QT_CONFIG(tooltip)
        sbSliceNum->setToolTip(QCoreApplication::translate("MainWindow", "Slice number", nullptr));
#endif // QT_CONFIG(tooltip)
        lblBand->setText(QCoreApplication::translate("MainWindow", "Band", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Markers", nullptr));
#if QT_CONFIG(tooltip)
        pbAddMarker->setToolTip(QCoreApplication::translate("MainWindow", "Add marker", nullptr));
#endif // QT_CONFIG(tooltip)
        pbAddMarker->setText(QString());
#if QT_CONFIG(tooltip)
        pbRemoveMarker->setToolTip(QCoreApplication::translate("MainWindow", "Remove marker", nullptr));
#endif // QT_CONFIG(tooltip)
        pbRemoveMarker->setText(QString());
#if QT_CONFIG(tooltip)
        wBrush->setToolTip(QCoreApplication::translate("MainWindow", "Brush radius reference", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        sbBrush->setToolTip(QCoreApplication::translate("MainWindow", "Brush radius", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        cbMarkers->setToolTip(QCoreApplication::translate("MainWindow", "Select Marker", nullptr));
#endif // QT_CONFIG(tooltip)
        label_9->setText(QCoreApplication::translate("MainWindow", "Visibility", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Markers", nullptr));
        cbMarkersVisibility->setItemText(0, QCoreApplication::translate("MainWindow", "Fill", nullptr));
        cbMarkersVisibility->setItemText(1, QCoreApplication::translate("MainWindow", "Border", nullptr));
        cbMarkersVisibility->setItemText(2, QCoreApplication::translate("MainWindow", "Off", nullptr));

#if QT_CONFIG(tooltip)
        cbMarkersVisibility->setToolTip(QCoreApplication::translate("MainWindow", "Marker visibility", nullptr));
#endif // QT_CONFIG(tooltip)
        label_8->setText(QCoreApplication::translate("MainWindow", "Activation", nullptr));
        cbActivationVisibility->setItemText(0, QCoreApplication::translate("MainWindow", "HeatMap", nullptr));
        cbActivationVisibility->setItemText(1, QCoreApplication::translate("MainWindow", "Saliency", nullptr));
        cbActivationVisibility->setItemText(2, QCoreApplication::translate("MainWindow", "Off", nullptr));

#if QT_CONFIG(tooltip)
        cbActivationVisibility->setToolTip(QCoreApplication::translate("MainWindow", "Activation visibility", nullptr));
#endif // QT_CONFIG(tooltip)
        cbGtVisibility->setItemText(0, QCoreApplication::translate("MainWindow", "Fill", nullptr));
        cbGtVisibility->setItemText(1, QCoreApplication::translate("MainWindow", "Border", nullptr));
        cbGtVisibility->setItemText(2, QCoreApplication::translate("MainWindow", "Off", nullptr));

#if QT_CONFIG(tooltip)
        cbGtVisibility->setToolTip(QCoreApplication::translate("MainWindow", "Label visibility", nullptr));
#endif // QT_CONFIG(tooltip)
        label_10->setText(QCoreApplication::translate("MainWindow", "GT Label", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Image Info.", nullptr));
        lblSize->setText(QCoreApplication::translate("MainWindow", "Size:", nullptr));
        lblMinMax->setText(QCoreApplication::translate("MainWindow", "Min./Max.", nullptr));
        lblBPP->setText(QCoreApplication::translate("MainWindow", "BPP", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Training Set", nullptr));
#if QT_CONFIG(tooltip)
        pbOpenTrainFolder->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p>Load dir with training images.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pbOpenTrainFolder->setText(QString());
#if QT_CONFIG(tooltip)
        pbOpenMarkers->setToolTip(QCoreApplication::translate("MainWindow", "Load Markers", nullptr));
#endif // QT_CONFIG(tooltip)
        pbOpenMarkers->setText(QString());
#if QT_CONFIG(tooltip)
        pbSaveMarkers->setToolTip(QCoreApplication::translate("MainWindow", "<html><head/><body><p>Save markers.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
        pbSaveMarkers->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        pbSaveMarkers->setText(QString());
#if QT_CONFIG(tooltip)
        pbRemoveDrawnMarkers->setToolTip(QCoreApplication::translate("MainWindow", "Remove all drawn markers", nullptr));
#endif // QT_CONFIG(tooltip)
        pbRemoveDrawnMarkers->setText(QString());
#if QT_CONFIG(tooltip)
        pbLoadGroundTruth->setToolTip(QCoreApplication::translate("MainWindow", "Load gold standard label", nullptr));
#endif // QT_CONFIG(tooltip)
        pbLoadGroundTruth->setText(QString());
#if QT_CONFIG(tooltip)
        pbRemoveGroundTruth->setToolTip(QCoreApplication::translate("MainWindow", "Remove labels", nullptr));
#endif // QT_CONFIG(tooltip)
        pbRemoveGroundTruth->setText(QString());
        lblNumMarkers->setText(QCoreApplication::translate("MainWindow", "# markers:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Activation Viewer", nullptr));
#if QT_CONFIG(tooltip)
        cbLayer->setToolTip(QCoreApplication::translate("MainWindow", "Select activation layer", nullptr));
#endif // QT_CONFIG(tooltip)
        pbInvertKernels->setText(QString());
#if QT_CONFIG(tooltip)
        pbProjectKernels->setToolTip(QCoreApplication::translate("MainWindow", "Open Kernel Projection Module", nullptr));
#endif // QT_CONFIG(tooltip)
        pbProjectKernels->setText(QString());
#if QT_CONFIG(tooltip)
        pbSelectKernelsManual->setToolTip(QCoreApplication::translate("MainWindow", "Select Kernels", nullptr));
#endif // QT_CONFIG(tooltip)
        pbSelectKernelsManual->setText(QString());
#if QT_CONFIG(tooltip)
        lwActivation->setToolTip(QCoreApplication::translate("MainWindow", "Load label directory", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
