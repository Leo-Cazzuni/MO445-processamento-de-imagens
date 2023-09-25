#include "executework.h"
#include "../mainwindow.h"

ExecuteWork::ExecuteWork(Segmentation *method)
{
    this->method = method;
}

void ExecuteWork::execute()
{
    if (!method->view->isLabelEmpty()){
        method->parent->destroyObjectTableActions();
    }

    QList<iftImage*>label = method->generateLabel();

    method->updateLabel(label);

    emit done();
}
