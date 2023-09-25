#include "renderingview.h"
#include "ui_renderingview.h"

RenderingView::RenderingView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderingView)
{
    ui->setupUi(this);

    setIcons();

    projection = nullptr;

    view = View::instance();

    projectionPixmap = new QGraphicsPixmapItem();

    // Instantiating Scene
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    scene->addItem(projectionPixmap);
    ui->gvRendering->setScene(scene);
}

RenderingView::~RenderingView()
{
    delete projection;
    delete ui;
}

void RenderingView::updateBrightnessAndConstrast(double f1, double f2, double g1, double g2)
{
    this->f1 = f1;
    this->f2 = f2;
    this->g1 = g1;
    this->g2 = g2;
}

void RenderingView::slotZoomIn()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Zooming in", 0);

    scaleFactor = 1.25;
    abs_scaleFactor *= scaleFactor;
//    ui->gvSlice->scale(scaleFactor,scaleFactor);
    ui->gvRendering->scale(scaleFactor,scaleFactor);

//    placeOrientationIndicators();
//    updateLines();

    iftWriteOnLog("Zooming in took", elapsedTime.elapsed());
}

void RenderingView::slotZoomOut()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Zooming out", 0);

    scaleFactor = .8;
    abs_scaleFactor *= scaleFactor;
//    ui->gvSlice->scale(scaleFactor,scaleFactor);
    ui->gvRendering->scale(scaleFactor,scaleFactor);

//    placeOrientationIndicators();
//    updateLines();

    iftWriteOnLog("Zooming out took", elapsedTime.elapsed());
}

void RenderingView::slotNormalSize()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing to normal size", 0);

    scaleFactor = 1.0/abs_scaleFactor;
    //ui->gvSlice->scale(scaleFactor,scaleFactor);
    ui->gvRendering->scale(scaleFactor,scaleFactor);
    abs_scaleFactor = 1.0;

    iftWriteOnLog("Changing to normal size took", elapsedTime.elapsed());
}

void RenderingView::slotUpdateRenderingAngle(float tilt, float spin)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Updating Rendering Angle", 0);

    char text[50];
    double t,s;
    view->incrementRenderingAngle(double(tilt),double(spin));
    view->getTiltSpinAngle(&t,&s);
    sprintf(text,"tilt: %.2f, spin: %.2f",t,s);
    ui->lblRendering->setText(text);
    updateRendition();

    iftWriteOnLog("Updating Rendering Angle took", elapsedTime.elapsed());
}

void RenderingView::slotUpdateRenderingMode(char mode)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Updating Rendering mode", 0);

    view->setRenderingMode(mode);
    updateRendition();

    iftWriteOnLog("Updating Rendering mode took", elapsedTime.elapsed());
}

void RenderingView::slotRenderingClicked(int x, int y)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Rendering clicked", 0);

    if (view->isLabelEmpty())
        return;
//    char mode = view->annotation->getAnnotationMode();
//    if (mode == HALT_ANNOTATION){
    iftVoxel u = {x,y,0,0},v;
    v = view->rendition->getVoxelFromIndexBuffer(u);
    if ((v.x != -1) || (v.y != -1) || (v.z != -1)){
        view->setCurrentAxialSliceNum(v.z);
        view->setCurrentCoronalSliceNum(v.y);
        view->setCurrentSagittalSliceNum(-v.x+1+view->getXsize());
        /*
         * The value -v.x+1+xsize for the sagittal slice is a trick to get the
         * correct slice. That is, since the sagittal is shown from the right
         * to the left and it is stored in memory from the left to the right, the
         * correct way to display it is: xsize-1-v.x. Therefore, when we click
         * in the rendition, the slice does not need to be corrected and in order
         * to make the current sagittal slice equals v.x, we need to send
         * -v.x+1+xsize as parameter. So:
         *
         * slice = xsize-1-v.x
         * slice = xsize-1-(-v.x)+1+xsize
         * slice = v.x
         */
         emit updatedGraphicsViews();
    }
//    }

    iftWriteOnLog("Rendering clicked took", elapsedTime.elapsed());
}

void RenderingView::slotReleaseView()
{
    view->annotation->setLastPoint(nullptr);
    emit updatedGraphicsViews();
    //updateRendition();
}

void RenderingView::slotDrawWireframe()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Toogling wireframe visibility", 0);

    view->rendition->toogleWireFrameVisibility();
    updateRendition();

    iftWriteOnLog("Toogling wireframe visibility took", elapsedTime.elapsed());
}

void RenderingView::slotDrawPlanes()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Toogling planes visibility", 0);

    view->rendition->toogleProjectionMode();
    updateRendition();

    iftWriteOnLog("Toogling planes visibility took", elapsedTime.elapsed());
}

void RenderingView::slotSaveProjection()
{
    const iftImage *projection = view->getProjection();
    if (projection != nullptr){
        QString fileName = QFileDialog::getSaveFileName(this,
                tr("Save Projection"), "",
                tr("PNG (*.png);;PPM (*ppm);;PGM (*.pgm)"));
        if (fileName.isEmpty())
                return;

        iftWriteImageByExt(projection,fileName.toUtf8().constData());
    }
}

void RenderingView::createActions()
{
    connect(ui->gvRendering, SIGNAL(angleChanged(float,float)), this, SLOT(slotUpdateRenderingAngle(float,float)));
    connect(ui->gvRendering, SIGNAL(modeChanged(char)), this, SLOT(slotUpdateRenderingMode(char)));
    connect(ui->gvRendering, SIGNAL(renderClick(int,int)), this, SLOT(slotRenderingClicked(int,int)));

    connect(ui->pbWireframe, SIGNAL(clicked(bool)), this, SLOT(slotDrawWireframe()));
    connect(ui->pbPlanes, SIGNAL(clicked(bool)), this, SLOT(slotDrawPlanes()));
    connect(ui->pbSaveProjection, SIGNAL(clicked(bool)), this, SLOT(slotSaveProjection()));
}

void RenderingView::destroyActions()
{
    disconnect(ui->gvRendering, SIGNAL(angleChanged(float,float)), nullptr, nullptr);
    disconnect(ui->gvRendering, SIGNAL(modeChanged(char)), nullptr, nullptr);
    disconnect(ui->gvRendering, SIGNAL(renderClick(int,int)), nullptr, nullptr);

    disconnect(ui->pbWireframe, SIGNAL(clicked(bool)), nullptr, nullptr);
    disconnect(ui->pbPlanes, SIGNAL(clicked(bool)), nullptr, nullptr);
    disconnect(ui->pbSaveProjection, SIGNAL(clicked(bool)), nullptr, nullptr);
}

void RenderingView::updateRendition()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Rendering", 0);


    view->performRendering();
    showProjection();


    iftWriteOnLog("Rendering took", elapsedTime.elapsed());
}

void RenderingView::showProjection()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Computing Projection", 0);

    const iftImage *aux = view->getProjection();
    // TODO create class Zoomable
    iftImage *proj = iftLinearStretch(const_cast<iftImage*>(aux),f1,f2,g1,g2);


    if (projection != nullptr)
        delete projection;
    projection = iftColoredImageToColoredQImage(proj);
    projectionPixmap->setPixmap(QPixmap::fromImage(*projection));

    iftDestroyImage(&proj);
    iftWriteOnLog("Projection took", elapsedTime.elapsed());
}

void RenderingView::setIcons()
{
    ui->pbPlanes->setIcon(QIcon(":/Images/icons/planes.svg"));
    ui->pbSaveProjection->setIcon(QIcon(":/Images/icons/save.svg"));
    ui->pbWireframe->setIcon(QIcon(":/Images/icons/wireframe.svg"));
}
