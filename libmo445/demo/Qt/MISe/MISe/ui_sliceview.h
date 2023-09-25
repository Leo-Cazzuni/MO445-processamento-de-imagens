/********************************************************************************
** Form generated from reading UI file 'sliceview.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLICEVIEW_H
#define UI_SLICEVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "views/qmygraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_SliceView
{
public:
    QVBoxLayout *verticalLayout;
    QMyGraphicsView *gvSlice;
    QHBoxLayout *layoutBottom;
    QPushButton *pbSaveSlice;
    QSpinBox *sbSlice;
    QLabel *lbCoordinatesValue;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QWidget *SliceView)
    {
        if (SliceView->objectName().isEmpty())
            SliceView->setObjectName(QString::fromUtf8("SliceView"));
        SliceView->resize(491, 378);
        verticalLayout = new QVBoxLayout(SliceView);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gvSlice = new QMyGraphicsView(SliceView);
        gvSlice->setObjectName(QString::fromUtf8("gvSlice"));

        verticalLayout->addWidget(gvSlice);

        layoutBottom = new QHBoxLayout();
        layoutBottom->setObjectName(QString::fromUtf8("layoutBottom"));
        pbSaveSlice = new QPushButton(SliceView);
        pbSaveSlice->setObjectName(QString::fromUtf8("pbSaveSlice"));

        layoutBottom->addWidget(pbSaveSlice);

        sbSlice = new QSpinBox(SliceView);
        sbSlice->setObjectName(QString::fromUtf8("sbSlice"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sbSlice->sizePolicy().hasHeightForWidth());
        sbSlice->setSizePolicy(sizePolicy);
        sbSlice->setMinimumSize(QSize(110, 0));
        sbSlice->setAccelerated(true);
        sbSlice->setMinimum(-1);
        sbSlice->setValue(-1);

        layoutBottom->addWidget(sbSlice);

        lbCoordinatesValue = new QLabel(SliceView);
        lbCoordinatesValue->setObjectName(QString::fromUtf8("lbCoordinatesValue"));
        lbCoordinatesValue->setMinimumSize(QSize(150, 0));
        QFont font;
        font.setFamilies({QString::fromUtf8("Ubuntu Mono")});
        lbCoordinatesValue->setFont(font);

        layoutBottom->addWidget(lbCoordinatesValue);

        horizontalSpacer_6 = new QSpacerItem(910, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layoutBottom->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(layoutBottom);


        retranslateUi(SliceView);

        QMetaObject::connectSlotsByName(SliceView);
    } // setupUi

    void retranslateUi(QWidget *SliceView)
    {
        SliceView->setWindowTitle(QCoreApplication::translate("SliceView", "Form", nullptr));
        pbSaveSlice->setText(QString());
        lbCoordinatesValue->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SliceView: public Ui_SliceView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLICEVIEW_H
