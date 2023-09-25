#include "executework.h"
#include "segmentation.h"

#include <QProgressDialog>
#include <QThread>

#include "../mainwindow.h"

Segmentation::Segmentation(MainWindow *parent, View *view) :
    QWidget((QMainWindow*) parent)
{
    this->parent = parent;
    this->view = view;
    this->_name = "Unnamed";
    annotationVisible = false;
    labelInfo = nullptr;
    hide();
}

Segmentation::Segmentation(const Segmentation &seg) : QWidget(seg.parentWidget())
{
    view = seg.view;
    parent = seg.parent;
    _name = seg._name;
    annotationVisible = seg.annotationVisible;

    //TODO labelColorTable = iftCopyColorTable(seg.labelColorTable);
    labelInfo = seg.labelInfo;
}

Segmentation::~Segmentation()
{

}

QString Segmentation::name()
{
    return _name;
}

void Segmentation::notifyGradientUpdate(ArcWeightFunction *)
{

}

void Segmentation::renderGraphics(QImage *, QGraphicsPixmapItem *, int)
{

}

bool Segmentation::mouseClickGraphics(int, int, Qt::MouseButtons, Qt::KeyboardModifiers, int)
{
    return false;
}

bool Segmentation::mouseMoveGraphics(int, int, int)
{
    return false;
}

void Segmentation::execute()
{
    QProgressDialog progress_dialog("Task in progress...", "Cancel", 0, PROGRESS_STEPS, this);
    progress_dialog.setCancelButton(0);
    progress_dialog.setCursor(Qt::WaitCursor);
    progress_dialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progress_dialog.setWindowModality(Qt::WindowModal);

    QThread* thread = new QThread(this);
    ExecuteWork* worker = new ExecuteWork(this);
    worker->moveToThread(thread);

    connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), &progress_dialog, SLOT(reset()));
    connect(thread, SIGNAL(finished()), this, SLOT(finishSegmentation()));
    connect(thread, SIGNAL(started()), worker, SLOT(execute()));

    connect(worker, SIGNAL(done()), thread, SLOT(quit()));

    connect(this, SIGNAL(progress(int)), &progress_dialog, SLOT(setValue(int)));
    connect(this, SIGNAL(textProgress(QString)), &progress_dialog, SLOT(setLabelText(QString)));

    thread->start();
    progress_dialog.setValue(0);
    progress_dialog.exec();
}

void Segmentation::notifyImageUpdate()
{

}


void Segmentation::finishSegmentation() {
    //TODO emit segmentationFinished();
    parent->updateAllGraphicalViews();
    bool useMarkerInfo = labelInfo != nullptr;
    parent->loadListOfObjects(useMarkerInfo);
}

void Segmentation::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    parent->setAnnotationVisibility(annotationVisible);
}

void Segmentation::updateLabel(iftImage *label)
{
    parent->setSystemMode("OPEN_LABEL");

    updateProgress(0.65, "Post-processing image.");
    iftImage *aux = PostProcessing::instance()->applyPipelineOnImage(label);
    iftDestroyImage(&label);
    label = aux;

    updateProgress(0.7, "Updating label.");

    if (label) {
        iftColorTable *ctb = nullptr;
        if (labelInfo)
            ctb = labelInfo->generateColorTable();
        else {
            int max = iftMaximumValue(label);
            ctb = iftCreateColorTable(max+1);
            // TODO remove from here
            for (int i = 0; i < ctb->ncolors; i++) {
                ctb->color[i] = MarkerInformation::defaultColor(i);
            }
            //
        }
        // TODO replace ctb with bool "use_marker_info"
        view->setLabel(label, ctb);
        iftDestroyColorTable(&ctb);
    } else
        view->destroyLabel();
    updateProgress(0.8, "Rendering.");

    view->setRenditionLabel();
    updateProgress(0.9, "Finishing up.");

    iftDestroyImage(&label);
}

void Segmentation::updateProgress(float p, QString msg)
{
    emit progress(p*PROGRESS_STEPS);
    if (msg != "") {
        QString text = "Task in progress... " + msg;
        emit textProgress(text);
    }
}
