#include "methodchooser.h"
#include "postprocessing.h"
#include "ui_postprocessing.h"
#include <QDebug>
#include <iftImage.h>
#include <postprocessing/method/morphclosing.h>
#include <postprocessing/method/morphopening.h>

class MyStringListModel : public QStringListModel {
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        Qt::ItemFlags flags = QStringListModel::flags(index);
        // TODO if (index.isValid())
        // allow index moving
            flags.setFlag(Qt::ItemIsDropEnabled, false);
        return flags;
    }
};

PostProcessing *PostProcessing::_instance = nullptr;

PostProcessing *PostProcessing::instance()
{
    if (_instance == nullptr)
        _instance = new PostProcessing();
    return _instance;
}

PostProcessing::PostProcessing() :
    QWidget(),
    ui(new Ui::PostProcessing)
{
    ui->setupUi(this);

    connect(ui->btAdd, SIGNAL(clicked()), this, SLOT(addProcessingMethod()));
    connect(ui->btDelete, SIGNAL(clicked()), this, SLOT(deleteProcessingMethod()));
    connect(ui->lvProcessing, SIGNAL(clicked(QModelIndex)), this, SLOT(changeActiveMethod(QModelIndex)));
    connect(ui->lvProcessing, SIGNAL(activated(QModelIndex)), this, SLOT(changeActiveMethod(QModelIndex)));
    //TODO connect(ui->lvProcessing, SIGNAL(indexesMoved(QModelIndexList)), this, SLOT(moveIndexes(QModelIndexList)));

    list_model = new MyStringListModel;
    ui->lvProcessing->setModel(list_model);
}

PostProcessing::~PostProcessing()
{
    delete list_model;
    delete ui;
}

iftImage *PostProcessing::applyPipelineOnImage(iftImage *img)
{
    iftImage *aux = iftCopyImage(img);
    for (Method *method: methods_ptrs) {
        iftImage *tmp = aux;
        aux = method->process(aux);
        iftDestroyImage(&tmp);
        qDebug() << method->toString();
    }
    return aux;
}

Method * morphOpeningInst() {return new MorphOpening;}
Method * morphClosingInst() {return new MorphClosing;}

void PostProcessing::addProcessingMethod()
{
    QMap<QString, Method* (*)()> methods_constructors;
    methods_constructors.insert("Morphological Opening", morphOpeningInst);
    methods_constructors.insert("Morphological Closing", morphClosingInst);
    QStringList methods;
    for (auto name : methods_constructors.keys())
        methods << name;

    MethodChooser dialog(this, methods);
    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        QString selected_method = dialog.activeMethod();
        Method *method = methods_constructors.take(selected_method)();
        ui->lvProcessing->model()->insertRow(list_model->rowCount());
        QModelIndex index = list_model->index(list_model->rowCount() - 1, 0);
        ui->lvProcessing->model()->setData(index, method->toString(), Qt::EditRole);
        //ui->lvProcessing->model()->setData(index, QVariant::fromValue(method), Qt::UserRole);
        methods_ptrs.insert(index.row(), method);
    }

}

void PostProcessing::deleteProcessingMethod()
{
    int row = ui->lvProcessing->currentIndex().row();
    Method *method = methods_ptrs.at(row);
    methods_ptrs.remove(row);
    ui->lvProcessing->model()->removeRow(row);
    method->hide();
    delete method;
}

void PostProcessing::changeActiveMethod(QModelIndex index)
{
    //QVariant variant = ui->lvProcessing->model()->data(index, Qt::UserRole);
    //Method * method = variant.value<Method*>();
    Method *method = methods_ptrs.at(index.row());

    // Hide the last module shown
    if (ui->gbProcessingOptions->layout()->count() > 0) {
        ui->gbProcessingOptions->layout()->takeAt(ui->gbProcessingOptions->layout()->count()-1)->widget()->hide();
    }
    ui->gbProcessingOptions->layout()->update();

    // Show the selected module
    method->show();
    ui->gbProcessingOptions->layout()->addWidget(method);
}

void PostProcessing::moveIndexes(QModelIndexList indexes)
{
    qDebug() << indexes;
}
