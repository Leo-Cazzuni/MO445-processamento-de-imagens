/********************************************************************************
** Form generated from reading UI file 'livewire.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIVEWIRE_H
#define UI_LIVEWIRE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LiveWire
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QPushButton *pbStart;
    QPushButton *pbReset;
    QPushButton *pbUndo;
    QPushButton *pbDone;
    QGroupBox *gbOrientation;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *rbNoOrientaion;
    QRadioButton *rbClockwise;
    QRadioButton *rbCounterClockwise;
    QSpacerItem *horizontalSpacer;
    QCheckBox *cbImageAsSaliency;

    void setupUi(QWidget *LiveWire)
    {
        if (LiveWire->objectName().isEmpty())
            LiveWire->setObjectName(QString::fromUtf8("LiveWire"));
        LiveWire->resize(451, 139);
        verticalLayout = new QVBoxLayout(LiveWire);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 6);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pbStart = new QPushButton(LiveWire);
        pbStart->setObjectName(QString::fromUtf8("pbStart"));

        horizontalLayout->addWidget(pbStart);

        pbReset = new QPushButton(LiveWire);
        pbReset->setObjectName(QString::fromUtf8("pbReset"));
        pbReset->setEnabled(false);

        horizontalLayout->addWidget(pbReset);

        pbUndo = new QPushButton(LiveWire);
        pbUndo->setObjectName(QString::fromUtf8("pbUndo"));
        pbUndo->setEnabled(false);

        horizontalLayout->addWidget(pbUndo);

        pbDone = new QPushButton(LiveWire);
        pbDone->setObjectName(QString::fromUtf8("pbDone"));
        pbDone->setEnabled(false);

        horizontalLayout->addWidget(pbDone);


        verticalLayout_4->addLayout(horizontalLayout);


        verticalLayout->addLayout(verticalLayout_4);

        gbOrientation = new QGroupBox(LiveWire);
        gbOrientation->setObjectName(QString::fromUtf8("gbOrientation"));
        gbOrientation->setEnabled(true);
        verticalLayout_2 = new QVBoxLayout(gbOrientation);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        rbNoOrientaion = new QRadioButton(gbOrientation);
        rbNoOrientaion->setObjectName(QString::fromUtf8("rbNoOrientaion"));
        rbNoOrientaion->setChecked(true);

        horizontalLayout_3->addWidget(rbNoOrientaion);

        rbClockwise = new QRadioButton(gbOrientation);
        rbClockwise->setObjectName(QString::fromUtf8("rbClockwise"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/turn.svg"), QSize(), QIcon::Normal, QIcon::Off);
        rbClockwise->setIcon(icon);
        rbClockwise->setChecked(false);

        horizontalLayout_3->addWidget(rbClockwise);

        rbCounterClockwise = new QRadioButton(gbOrientation);
        rbCounterClockwise->setObjectName(QString::fromUtf8("rbCounterClockwise"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/icons/turn_left.png"), QSize(), QIcon::Normal, QIcon::Off);
        rbCounterClockwise->setIcon(icon1);

        horizontalLayout_3->addWidget(rbCounterClockwise);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout->addWidget(gbOrientation);

        cbImageAsSaliency = new QCheckBox(LiveWire);
        cbImageAsSaliency->setObjectName(QString::fromUtf8("cbImageAsSaliency"));

        verticalLayout->addWidget(cbImageAsSaliency);


        retranslateUi(LiveWire);

        QMetaObject::connectSlotsByName(LiveWire);
    } // setupUi

    void retranslateUi(QWidget *LiveWire)
    {
        LiveWire->setWindowTitle(QCoreApplication::translate("LiveWire", "Form", nullptr));
        pbStart->setText(QCoreApplication::translate("LiveWire", "Start", nullptr));
        pbReset->setText(QCoreApplication::translate("LiveWire", "Reset", nullptr));
        pbUndo->setText(QCoreApplication::translate("LiveWire", "Undo", nullptr));
        pbDone->setText(QCoreApplication::translate("LiveWire", "Done", nullptr));
        gbOrientation->setTitle(QCoreApplication::translate("LiveWire", "Orientation", nullptr));
        rbNoOrientaion->setText(QCoreApplication::translate("LiveWire", "No orientation", nullptr));
        rbClockwise->setText(QCoreApplication::translate("LiveWire", "Clockwise", nullptr));
        rbCounterClockwise->setText(QCoreApplication::translate("LiveWire", "Counter clockwise", nullptr));
        cbImageAsSaliency->setText(QCoreApplication::translate("LiveWire", "Load the image itself as the orientation saliency map", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LiveWire: public Ui_LiveWire {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIVEWIRE_H
