#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include "method.h"

#include <QStandardItemModel>
#include <QStringListModel>
#include <QWidget>
#include <iftImage.h>

namespace Ui {
class PostProcessing;
}

class PostProcessing : public QWidget
{
    Q_OBJECT

public:
    static PostProcessing *instance();
    ~PostProcessing();

    iftImage *applyPipelineOnImage(iftImage *img);
private slots:
    void addProcessingMethod();
    void deleteProcessingMethod();
    void changeActiveMethod(QModelIndex index);
    void moveIndexes(QModelIndexList indexes);
private:
    PostProcessing();
    static PostProcessing *_instance;

    Ui::PostProcessing *ui;
    // TODO subclass QAbstractListModel
    QStringListModel *list_model;

    QVector<Method*> methods_ptrs;
};

#endif // POSTPROCESSING_H
