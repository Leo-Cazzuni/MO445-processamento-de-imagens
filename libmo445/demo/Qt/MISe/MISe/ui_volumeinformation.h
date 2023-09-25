/********************************************************************************
** Form generated from reading UI file 'volumeinformation.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VOLUMEINFORMATION_H
#define UI_VOLUMEINFORMATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_VolumeInformation
{
public:
    QLabel *lblLabelPath;
    QLabel *lblLabelDomain;
    QLabel *lblLabelVoxel;
    QLabel *lblImagePath;
    QLabel *lblImageDomain;
    QLabel *lblVoxelSize;
    QLabel *lblLabelDepth;
    QLabel *lblDepth;
    QLabel *lblMaxAndMinValues;

    void setupUi(QDialog *VolumeInformation)
    {
        if (VolumeInformation->objectName().isEmpty())
            VolumeInformation->setObjectName(QString::fromUtf8("VolumeInformation"));
        VolumeInformation->resize(565, 238);
        lblLabelPath = new QLabel(VolumeInformation);
        lblLabelPath->setObjectName(QString::fromUtf8("lblLabelPath"));
        lblLabelPath->setGeometry(QRect(40, 30, 91, 17));
        lblLabelDomain = new QLabel(VolumeInformation);
        lblLabelDomain->setObjectName(QString::fromUtf8("lblLabelDomain"));
        lblLabelDomain->setGeometry(QRect(40, 112, 111, 17));
        lblLabelVoxel = new QLabel(VolumeInformation);
        lblLabelVoxel->setObjectName(QString::fromUtf8("lblLabelVoxel"));
        lblLabelVoxel->setGeometry(QRect(40, 70, 101, 17));
        lblImagePath = new QLabel(VolumeInformation);
        lblImagePath->setObjectName(QString::fromUtf8("lblImagePath"));
        lblImagePath->setGeometry(QRect(110, 31, 431, 16));
        lblImageDomain = new QLabel(VolumeInformation);
        lblImageDomain->setObjectName(QString::fromUtf8("lblImageDomain"));
        lblImageDomain->setGeometry(QRect(150, 112, 391, 17));
        lblVoxelSize = new QLabel(VolumeInformation);
        lblVoxelSize->setObjectName(QString::fromUtf8("lblVoxelSize"));
        lblVoxelSize->setGeometry(QRect(118, 71, 421, 17));
        lblLabelDepth = new QLabel(VolumeInformation);
        lblLabelDepth->setObjectName(QString::fromUtf8("lblLabelDepth"));
        lblLabelDepth->setGeometry(QRect(204, 150, 341, 17));
        lblDepth = new QLabel(VolumeInformation);
        lblDepth->setObjectName(QString::fromUtf8("lblDepth"));
        lblDepth->setGeometry(QRect(40, 150, 171, 17));
        lblMaxAndMinValues = new QLabel(VolumeInformation);
        lblMaxAndMinValues->setObjectName(QString::fromUtf8("lblMaxAndMinValues"));
        lblMaxAndMinValues->setGeometry(QRect(40, 190, 511, 17));

        retranslateUi(VolumeInformation);

        QMetaObject::connectSlotsByName(VolumeInformation);
    } // setupUi

    void retranslateUi(QDialog *VolumeInformation)
    {
        VolumeInformation->setWindowTitle(QCoreApplication::translate("VolumeInformation", "Volume Information", nullptr));
        lblLabelPath->setText(QCoreApplication::translate("VolumeInformation", "Filename:", nullptr));
        lblLabelDomain->setText(QCoreApplication::translate("VolumeInformation", "Image Domain:", nullptr));
        lblLabelVoxel->setText(QCoreApplication::translate("VolumeInformation", "Voxel Size:", nullptr));
        lblImagePath->setText(QString());
        lblImageDomain->setText(QString());
        lblVoxelSize->setText(QString());
        lblLabelDepth->setText(QString());
        lblDepth->setText(QCoreApplication::translate("VolumeInformation", "Radiometric resolution:", nullptr));
        lblMaxAndMinValues->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class VolumeInformation: public Ui_VolumeInformation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VOLUMEINFORMATION_H
