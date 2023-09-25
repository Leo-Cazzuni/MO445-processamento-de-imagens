#include "manualsegmentation.h"
#include "ui_manualsegmentation.h"

ManualSegmentation::ManualSegmentation(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::ManualSegmentation)
{
    ui->setupUi(this);

    annotationVisible = true;

    _name = "Manual";

    connect(ui->pbDone, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));
}

ManualSegmentation::~ManualSegmentation()
{
    delete ui;
}

void ManualSegmentation::reset()
{
    QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Warning"),
                                                                 "This action is irreversible. "
                                                                 "Do you want to proceed?",
                                                                 QMessageBox::Yes | QMessageBox::No);

    if (bt_clicked == QMessageBox::Yes) {
        view->annotation->resetMarkers();
        view->destroyLabel();
        finishSegmentation();
    }
}

iftImage *ManualSegmentation::generateLabel()
{
    const iftImage *current_label = view->getLabel();

    const iftImage *markers = view->annotation->getMarkers();
    const iftImage *removal_markers = view->annotation->getRemovalMarkers();
    iftImage *label = NULL;
    if (current_label) {
        label = iftCopyImage(current_label);
    } else {
        label = iftCreateImageFromImage(markers);
    }


    #pragma omp parallel for
    for (int i = 0; i < label->n; i++) {
        if (removal_markers->val[i]) {
            markers->val[i] = UNMARKED;
            removal_markers->val[i] = 0;
        } else if (markers->val[i] >= 0) {
            label->val[i] = markers->val[i];
        }

        //TODO
        markers->val[i] = UNMARKED;
    }

    labelInfo = &view->annotation->getMarkerInfoArray();


    return label;
}
