/********************************************************************************
** Form generated from reading UI file 'postprocessing.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POSTPROCESSING_H
#define UI_POSTPROCESSING_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PostProcessing
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QListView *lvProcessing;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QToolButton *btDelete;
    QToolButton *btAdd;
    QFrame *line;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QFrame *line_2;
    QVBoxLayout *gbProcessingOptions;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *PostProcessing)
    {
        if (PostProcessing->objectName().isEmpty())
            PostProcessing->setObjectName(QString::fromUtf8("PostProcessing"));
        PostProcessing->resize(451, 236);
        verticalLayout = new QVBoxLayout(PostProcessing);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_2 = new QLabel(PostProcessing);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        lvProcessing = new QListView(PostProcessing);
        lvProcessing->setObjectName(QString::fromUtf8("lvProcessing"));
        lvProcessing->setEditTriggers(QAbstractItemView::NoEditTriggers);
        lvProcessing->setDragEnabled(true);
        lvProcessing->setDragDropOverwriteMode(false);
        lvProcessing->setDragDropMode(QAbstractItemView::InternalMove);
        lvProcessing->setDefaultDropAction(Qt::IgnoreAction);
        lvProcessing->setAlternatingRowColors(true);
        lvProcessing->setSelectionBehavior(QAbstractItemView::SelectItems);

        verticalLayout_2->addWidget(lvProcessing);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btDelete = new QToolButton(PostProcessing);
        btDelete->setObjectName(QString::fromUtf8("btDelete"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/icons/remove.svg"), QSize(), QIcon::Normal, QIcon::Off);
        btDelete->setIcon(icon);

        horizontalLayout_2->addWidget(btDelete);

        btAdd = new QToolButton(PostProcessing);
        btAdd->setObjectName(QString::fromUtf8("btAdd"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/icons/add.svg"), QSize(), QIcon::Normal, QIcon::Off);
        btAdd->setIcon(icon1);

        horizontalLayout_2->addWidget(btAdd);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout_2);

        line = new QFrame(PostProcessing);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label = new QLabel(PostProcessing);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setLayoutDirection(Qt::LeftToRight);
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        verticalLayout_3->addWidget(label);

        line_2 = new QFrame(PostProcessing);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line_2);

        gbProcessingOptions = new QVBoxLayout();
        gbProcessingOptions->setObjectName(QString::fromUtf8("gbProcessingOptions"));
        gbProcessingOptions->setSizeConstraint(QLayout::SetMaximumSize);

        verticalLayout_3->addLayout(gbProcessingOptions);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_3);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(PostProcessing);

        QMetaObject::connectSlotsByName(PostProcessing);
    } // setupUi

    void retranslateUi(QWidget *PostProcessing)
    {
        PostProcessing->setWindowTitle(QCoreApplication::translate("PostProcessing", "Post-processing pipeline", nullptr));
        label_2->setText(QCoreApplication::translate("PostProcessing", "Active methods", nullptr));
        btDelete->setText(QString());
        btAdd->setText(QCoreApplication::translate("PostProcessing", "...", nullptr));
        label->setText(QCoreApplication::translate("PostProcessing", "Select a method to show its options", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PostProcessing: public Ui_PostProcessing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POSTPROCESSING_H
