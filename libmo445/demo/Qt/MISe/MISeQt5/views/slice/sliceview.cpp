#include "sliceview.h"
#include "ui_sliceview.h"

double SliceView::f1;
double SliceView::g1;
double SliceView::f2;
double SliceView::g2;

SliceView::SliceView(QWidget *parent):
    QWidget(parent),
    ui(new Ui::SliceView)
{
    ui->setupUi(this);


    gradientVisibility = false;


    setIcons();

    sliceImage = nullptr;

    horizontalSlice = nullptr;
    verticalSlice = nullptr;

    view = View::instance();

    connect(ui->gvSlice, SIGNAL(increaseBrush()), this, SLOT(increaseBrush()));
    connect(ui->gvSlice, SIGNAL(decreaseBrush()), this, SLOT(decreaseBrush()));
}

SliceView::~SliceView()
{
    disconnect(ui->gvSlice, SIGNAL(increaseBrush()), nullptr, nullptr);
    disconnect(ui->gvSlice, SIGNAL(decreaseBrush()), nullptr, nullptr);
    delete[] orientationIndicators;
    if (sliceImage)
        delete sliceImage;
    delete ui;
}

void SliceView::initializeSlice(int sliceType, SliceView *slice2, SliceView *slice3)
{
    this->sliceType = sliceType;
    this->horizontalSlice = slice2;
    this->verticalSlice = slice3;


    // Creating pen for the lines to be drawn. The width is set to 0 so the lines wont scale with the image.
    QPen pen;
    pen.setColor(Qt::green);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(0);

    // Creating lines to cross the anatomical planes
    linesVisibility = false;
    verticalLine = new QGraphicsLineItem();
    verticalLine->setPen(pen);
    verticalLine->setVisible(linesVisibility);
    horizontalLine = new QGraphicsLineItem();
    horizontalLine->setPen(pen);
    horizontalLine->setVisible(linesVisibility);

    // Creating the pixmap itens that will display the slices
    imagePixmapItem = new QGraphicsPixmapItem();

    // Creating the text itens that will display the orientation on the screen
    orientationIndicators = new QGraphicsTextItem*[4];
    for (int i = 0; i < 4; i++){
        QColor q;
        QFont f;
        q.setRed(255);
        q.setGreen(215);
        q.setBlue(0);
        f.setPointSize(16);
        orientationIndicators[i] = new QGraphicsTextItem();
        orientationIndicators[i]->setDefaultTextColor(q);
        orientationIndicators[i]->setFont(f);
        orientationIndicators[i]->setVisible(false);
        orientationIndicators[i]->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
    }

    // Setting orientation letters to guide the specialist
    switch (sliceType) {
        case AXIAL:
            orientationIndicators[0]->setPlainText("R");
            orientationIndicators[1]->setPlainText("L");
            orientationIndicators[2]->setPlainText("A");
            orientationIndicators[3]->setPlainText("P");
            break;
        case CORONAL:
            orientationIndicators[0]->setPlainText("R");
            orientationIndicators[1]->setPlainText("L");
            orientationIndicators[2]->setPlainText("S");
            orientationIndicators[3]->setPlainText("I");
            break;
        case SAGITTAL:
            orientationIndicators[0]->setPlainText("A");
            orientationIndicators[1]->setPlainText("P");
            orientationIndicators[2]->setPlainText("S");
            orientationIndicators[3]->setPlainText("I");
            break;
    }

    // Instantiating Scenes
    QGraphicsScene *scene;

    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    scene->addItem(imagePixmapItem);
    scene->addItem(horizontalLine);
    scene->addItem(verticalLine);
    scene->addItem(orientationIndicators[0]);
    scene->addItem(orientationIndicators[1]);
    scene->addItem(orientationIndicators[2]);
    scene->addItem(orientationIndicators[3]);
    ui->gvSlice->setScene(scene);

    ui->gvSlice->horizontalScrollBar()->setCursor(Qt::ArrowCursor);
    ui->gvSlice->verticalScrollBar()->setCursor(Qt::ArrowCursor);

    //QGraphicsScene *scene = ui->gvSlice->scene();

/*    connect(scene, &QGraphicsScene::changed, [scene]() {
        QRectF rect = scene->itemsBoundingRect();
        scene->setSceneRect(rect);
    })*/;
}

void SliceView::initalizeGraphicalView()
{
//    ui->gvSlice->scene()->removeItem(imagePixmapItem);
//    delete imagePixmapItem;
//    imagePixmapItem = new QGraphicsPixmapItem();
//    ui->gvSlice->scene()->addItem(imagePixmapItem);

    ui->sbSlice->setMaximum(getMaxCurrentSlice());
    // TODO: if the following value isnt different from the previous one,
    // it wont trigger the sliceChange slot.
    ui->sbSlice->setValue(getCurrentSliceNumber());
    ui->sbSlice->setMinimum(0);
    linesVisibility = true;
    placeOrientationIndicators();
    updateCursor();
}

void SliceView::updateBrightnessAndConstrast(double f1, double f2, double g1, double g2)
{
    SliceView::f1 = f1;
    SliceView::f2 = f2;
    SliceView::g1 = g1;
    SliceView::g2 = g2;
}

void SliceView::slotUpdateSliceMousePosition(int x, int y)
{
    if (view->isImageEmpty())
        return;

    QString s;
    iftVoxel u,v;
    const iftImage *img = nullptr;

    u.x = x;
    u.y = y;
    u.z = 1;
    v = view->orientation->mapPixelToVolume(u, getCurrentSliceNumber(), sliceType);

    img = view->getImage();
    const iftImage *norm = view->getNormalizedImage();
    if (iftValidVoxel(img,v)) {
        if (iftIs3DImage(img)) {
            s = QString::asprintf("(%d, %d, %d) = ", v.x, v.y, v.z);
        } else {
            s = QString::asprintf("(%d, %d) = ", v.x, v.y);
        }

        if (iftIsColorImage(img)) {
            int p = iftGetVoxelIndex(img, v);
            iftColor yCbCr = {{norm->val[p], norm->Cb[p], norm->Cr[p]}, 1};
            iftColor rgb   = iftYCbCrtoRGB(yCbCr, 255);
            s += QString::asprintf("R:%d G:%d B:%d", rgb.val[0], rgb.val[1], rgb.val[2]);
        } else {
            int val = view->iftGetImageIntensityAtVoxel(v);
            s += QString::asprintf("%d", val);
        }

        ui->lbCoordinatesValue->setText(s);

        Segmentation *segMethod = view->currentSegmentationMethod();
        if (segMethod) {
            segMethod->mouseMoveGraphics(x,y, sliceType);
        }
    }
}

void SliceView::slotReleaseView(int x, int y)
{
    char mode = view->annotation->getAnnotationMode();
    if (mode == ERASING_ANNOTATION){
        iftVoxel u,w;
        u.x = x;
        u.y = y;
        u.z = 1;
        w = view->orientation->mapPixelToVolume(u, getCurrentSliceNumber(), sliceType);
        view->annotation->selectPointForDeletion(w, view->getLabel(), view->getImageForest());
        updateAllGraphicalViews();
    } else {
        view->annotation->setLastPoint(nullptr);
        updateAllGraphicalViews();
        //TODO updateRendition();
    }

}

void SliceView::slotGraphicalViewClicked(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers modifiers)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Axial graphics view clicked", 0);

    if ((x >= sliceImage->width()) || (x < 0) || (y >= sliceImage->height()) || (y < 0))
        return;

    /*
     * When the axial graphical view is clicked, the coronal
     * and sagittal slices must be updated.
     *
     * X-axis => Sagittal
     * Y-axis => Coronal
     */
    Segmentation *segMethod = view->currentSegmentationMethod();
    if (segMethod) {
        if (segMethod->mouseClickGraphics(x,y,bts,modifiers, sliceType))
            return;
    }

    char mode = view->annotation->getAnnotationMode();
    if (bts == Qt::MiddleButton) {

        iftVoxel u,w;
        u.x = x;
        u.y = y;
        u.z = 1;
        w = view->orientation->mapPixelToVolume(u, getCurrentSliceNumber(), sliceType);
        view->annotation->selectPointForDeletion(w, view->getLabel(), view->getImageForest());
        updateAllGraphicalViews();
    } else if (mode == HALT_ANNOTATION){

        if ((x >= sliceImage->width()) || (x < 0) || (y >= sliceImage->height()) || (y < 0))
            return;

        int h_slice = 0;
        int v_slice = 0;

        switch (sliceType) {
            case AXIAL:
                h_slice = y;
                v_slice = sliceImage->width() - x;
                break;
            case CORONAL:
                h_slice = sliceImage->height() - y;
                v_slice = sliceImage->width() - x;
                break;
            case SAGITTAL:
                h_slice = sliceImage->height() - y;
                v_slice = x;
                break;
        }

        horizontalSlice->ui->sbSlice->setValue(h_slice);
        verticalSlice->ui->sbSlice->setValue(v_slice);
    } else if (mode == BOX_ANNOTATION){
        iftVoxel u,w;
        u.x = x;
        u.y = y;
        u.z = 1;
        w = view->orientation->mapPixelToVolume(u, getCurrentSliceNumber(), sliceType);
        MarkerSettings *ms = MarkerSettings::instance();
        view->annotation->paintBox(w,ms->getMarkerLabel());
        updateAllGraphicalViews();
    } else if (mode == FREE_FORM_ANNOTATION){
        bool erasing = mode == ERASING_ANNOTATION;
        QPoint point(x, y);
        int marker_label = bts == Qt::RightButton? 0: view->annotation->getMarkerInfoArray().getActive().label();
        view->annotation->paintMarkersOnQImage(view->orientation, sliceImage, point, marker_label, sliceType, getCurrentSliceNumber(), erasing);
        imagePixmapItem->setPixmap(QPixmap::fromImage(*sliceImage));
    } else { // ERASING_ANNOTATION
//        iftVoxel u,w;
//        u.x = x;
//        u.y = y;
//        u.z = 1;
//        w = view->orientation->mapPixelToVolume(u, getCurrentSliceNumber(), sliceType);
//        view->annotation->selectPointForDeletion(w, view->getLabel(), view->getImageForest());
//        updateAllGraphicalViews();
    }

    iftWriteOnLog("Axial graphics view clicked took", elapsedTime.elapsed());
}

void SliceView::slotChangeSliceImage()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing ```axial``` slice", 0);

    setCurrentSliceNumber(ui->sbSlice->value());
    showCurrentSlice();
    //updateRendition();
    updateAllSlicesLines();

    iftWriteOnLog("Changing ```axial``` slice took", elapsedTime.elapsed());
}

void SliceView::slotSaveGraphicalView()
{
    if (this->sliceImage == nullptr)
        return;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"),
                                                    QString(),
                                                    tr("Images (*.png)"));
    if (!fileName.isEmpty())
    {
      this->sliceImage->save(fileName);
    }
}

void SliceView::increaseBrush()
{
    MarkerSettings *ms = MarkerSettings::instance();
    ms->increaseBrush();
}

void SliceView::decreaseBrush()
{
    MarkerSettings *ms = MarkerSettings::instance();
    ms->decreaseBrush();
}

void SliceView::createActions()
{
    connect(ui->gvSlice, SIGNAL(positionChanged(int,int)), this, SLOT(slotUpdateSliceMousePosition(int,int)));
    connect(ui->gvSlice, SIGNAL(clicked(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), this, SLOT(slotGraphicalViewClicked(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)));

    connect(ui->sbSlice, SIGNAL(valueChanged(int)), this, SLOT(slotChangeSliceImage()));

    connect(ui->pbSaveSlice, SIGNAL(clicked(bool)), this, SLOT(slotSaveGraphicalView()));

    connect(ui->gvSlice, SIGNAL(signalRelease(int, int)), this, SLOT(slotReleaseView(int, int)));
}

void SliceView::destroyActions()
{
    disconnect(ui->gvSlice, SIGNAL(clicked(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), nullptr,nullptr);
    disconnect(ui->gvSlice, SIGNAL(positionChanged(int, int)), nullptr,nullptr);
    disconnect(ui->gvSlice, SIGNAL(signalRelease(int, int)), nullptr,nullptr);

    disconnect(ui->sbSlice, SIGNAL(valueChanged(int)), nullptr, nullptr);

    disconnect(ui->pbSaveSlice, SIGNAL(clicked(bool)), nullptr, nullptr);
}

void SliceView::set2DVisualizationMode()
{
    ui->sbSlice->hide();
}

void SliceView::set3DVisualizationMode()
{
    ui->sbSlice->show();
}

void SliceView::startAnnotation(int)
{
    updateCursor();
}

iftImage *SliceView::getCurrentRemovalMarkersSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentRemovalMarkersAxialSlice();
        case CORONAL:
            return view->getCurrentRemovalMarkersCoronalSlice();
        case SAGITTAL:
            return view->getCurrentRemovalMarkersSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentSliceImage()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentAxialSlice();
        case CORONAL:
            return view->getCurrentCoronalSlice();
        case SAGITTAL:
            return view->getCurrentSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentNormalizedSliceImage()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentNormalizedAxialSlice();
        case CORONAL:
            return view->getCurrentNormalizedCoronalSlice();
        case SAGITTAL:
            return view->getCurrentNormalizedSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentMarkerSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentMarkerAxialSlice();
        case CORONAL:
            return view->getCurrentMarkerCoronalSlice();
        case SAGITTAL:
            return view->getCurrentMarkerSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentLabelSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentLabelAxialSlice();
        case CORONAL:
            return view->getCurrentLabelCoronalSlice();
        case SAGITTAL:
            return view->getCurrentLabelSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentBorderSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentBorderAxialSlice();
        case CORONAL:
            return view->getCurrentBorderCoronalSlice();
        case SAGITTAL:
            return view->getCurrentBorderSagittalSlice();
    }
    return nullptr;
}

iftImage *SliceView::getCurrentGradientSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentGradientAxialSlice();
        case CORONAL:
            return view->getCurrentGradientCoronalSlice();
        case SAGITTAL:
            return view->getCurrentGradientSagittalSlice();
    }
    return nullptr;
}

int SliceView::getMaxCurrentSlice()
{
    switch (sliceType) {
        case AXIAL:
            return view->getMaxAxialSlice();
        case CORONAL:
            return view->getMaxCoronalSlice();
        case SAGITTAL:
            return view->getMaxSagittalSlice();
    }
    return -1;
}

int SliceView::getCurrentSliceNumber()
{
    switch (sliceType) {
        case AXIAL:
            return view->getCurrentAxialSliceNum();
        case CORONAL:
            return view->getCurrentCoronalSliceNum();
        case SAGITTAL:
            return view->getCurrentSagittalSliceNum();
    }
    return -1;
}

void SliceView::setCurrentSliceNumber(int index)
{
    switch (sliceType) {
        case AXIAL:
            view->setCurrentAxialSliceNum(index);
            break;
        case CORONAL:
            view->setCurrentCoronalSliceNum(index);
            break;
        case SAGITTAL:
            view->setCurrentSagittalSliceNum(index);
            break;
    }
}

void SliceView::showCurrentSlice()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Showing axial slice", 0);

//    if (gradientVisibility && view->getGradient() != nullptr) {
//        iftImage *grad = getCurrentGradientSlice();


//        iftImage *corrected_grad = view->orientation->applyOrientationOnSlice(grad, sliceType);
//        iftDestroyImage(&grad);

//        if (sliceImage != nullptr){
//            delete sliceImage;
//            sliceImage = nullptr;
//        }

//        // TODO simplify
//        iftColorTable *tm = view->annotation->generateColorTable();
//        iftIntArray *markers_visibility = view->annotation->generateMarkersVisibility();
//        const QVector<MarkerInformation> markerInfoArray = view->annotation->getMarkerInfoArray();

//        iftImage *removal_markers = getCurrentRemovalMarkersSlice();
//        iftImage *corrected_removal = view->orientation->applyOrientationOnSlice(removal_markers, sliceType);
//        iftDestroyImage(&removal_markers);

//        iftImage *m = getCurrentMarkerSlice();
//        iftImage *corrected_markers = view->orientation->applyOrientationOnSlice(m, sliceType);
//        iftDestroyImage(&m);

//        sliceImage = iftImageToQImage(corrected_grad,nullptr,nullptr,nullptr,nullptr,corrected_markers, corrected_removal, tm,markers_visibility);
//        //sliceImage = iftImageToQImage(corrected_grad);

//        imagePixmapItem->setPixmap(QPixmap::fromImage(*sliceImage));

//        iftDestroyImage(&corrected_grad);

//        iftDestroyImage(&corrected_markers);
//        iftDestroyImage(&corrected_removal);
//        iftDestroyColorTable(&tm);
//        iftDestroyIntArray(&markers_visibility);
//    } else {

        iftImage *s = nullptr;

        if (gradientVisibility && view->getGradient() != nullptr) {
            s = getCurrentGradientSlice();
        } else {
            s = getCurrentNormalizedSliceImage();
        }

        if (sliceImage != nullptr){
            delete sliceImage;
            sliceImage = nullptr;
        }

        iftImage *ls = iftLinearStretch(s,f1,f2,g1,g2);

        iftImage *border = nullptr, *corrected_border = nullptr;
        iftImage *l = nullptr, *corrected_label = nullptr;
        iftImage *m = nullptr, *corrected_markers = nullptr;
        iftImage *corrected = nullptr;
        iftColorTable *t = nullptr;
        iftColorTable *tm = nullptr;
        iftIntArray *markers_visibility = nullptr;
        bool *obj_visibility = nullptr;

        corrected = view->orientation->applyOrientationOnSlice(ls, sliceType);
        iftDestroyImage(&ls);
        m = getCurrentMarkerSlice();
        corrected_markers = view->orientation->applyOrientationOnSlice(m, sliceType);
        iftDestroyImage(&m);
        tm = view->annotation->generateColorTable();
        markers_visibility = view->annotation->generateMarkersVisibility();
        const QVector<MarkerInformation> markerInfoArray = view->annotation->getMarkerInfoArray();

        iftImage *removal_markers = getCurrentRemovalMarkersSlice();
        iftImage *corrected_removal = view->orientation->applyOrientationOnSlice(removal_markers, sliceType);
        iftDestroyImage(&removal_markers);

        if (!view->isLabelEmpty()){
            l = getCurrentLabelSlice();
            corrected_label = view->orientation->applyOrientationOnSlice(l, sliceType);
            iftDestroyImage(&l);
            t = view->getColorTable();
            obj_visibility = view->rendition->getObjectsVisibility();
            border = getCurrentBorderSlice();
            corrected_border = view->orientation->applyOrientationOnSlice(border, sliceType);
            iftDestroyImage(&border);
        }
        if (sliceImage)
            delete sliceImage;
        sliceImage = iftImageToQImage(corrected,corrected_label,corrected_border,t,obj_visibility,corrected_markers, corrected_removal, tm,markers_visibility);

        Segmentation *segMethod = view->currentSegmentationMethod();
        if (segMethod) {
            segMethod->renderGraphics(sliceImage, imagePixmapItem, sliceType);
        }

        iftDestroyImage(&corrected_border);
        iftDestroyImage(&corrected_removal);
        iftDestroyImage(&corrected);
        iftDestroyImage(&corrected_markers);
        iftDestroyImage(&corrected_label);
        iftDestroyColorTable(&t);
        iftDestroyColorTable(&tm);
        iftDestroyIntArray(&markers_visibility);
        iftDestroyImage(&s);

        if (obj_visibility != NULL){
            free(obj_visibility);
        }

        imagePixmapItem->setPixmap(QPixmap::fromImage(*sliceImage));

        iftWriteOnLog("Showing axial slice took", elapsedTime.elapsed());
    //}
}

void SliceView::updateLines()
{
    if (!sliceImage)
        return;

    QPoint p1, p2;
    verticalLine->setVisible(linesVisibility);
    horizontalLine->setVisible(linesVisibility);
    /*
     * Axial
     */
    // vertical line
    p1.setX(verticalSlice->getCurrentSliceNumber());
    p1.setY(0);
    p2.setX(verticalSlice->getCurrentSliceNumber());
    p2.setY(sliceImage->height());
    verticalLine->setLine(p1.x(),p1.y(),p2.x(),p2.y());
    // horizontal line
    // TODO remove workaround
    if (horizontalSlice->sliceType == AXIAL) {
        p1.setX(0);
        p1.setY(horizontalSlice->getMaxCurrentSlice() - horizontalSlice->getCurrentSliceNumber());
        p2.setX(sliceImage->width());
        p2.setY(horizontalSlice->getMaxCurrentSlice() - horizontalSlice->getCurrentSliceNumber());
        horizontalLine->setLine(p1.x(),p1.y(),p2.x(),p2.y());
    } else {
        p1.setX(0);
        p1.setY(horizontalSlice->getCurrentSliceNumber());
        p2.setX(sliceImage->width());
        p2.setY(horizontalSlice->getCurrentSliceNumber());
        horizontalLine->setLine(p1.x(),p1.y(),p2.x(),p2.y());
    }

}

void SliceView::updateAllSlicesLines()
{
    if (horizontalSlice && verticalSlice) {
        this->updateLines();
        horizontalSlice->updateLines();
        verticalSlice->updateLines();
    }
}

void SliceView::placeOrientationIndicators() {
    int halfLetterWidth;
    int halfLetterHeight;
    int Scorrection = -50;
    int Lcorrection = -20;

    QFont font = orientationIndicators[0]->font();
    QFontMetrics fm(font);

    halfLetterHeight = fm.height()/2;

    for (int i = 0; i < 4; i ++)
        orientationIndicators[i]->setVisible(true);

    halfLetterWidth = int(orientationIndicators[0]->textWidth())/2;
    orientationIndicators[0]->setPos(Scorrection,(sliceImage->height()/2)-halfLetterWidth);
    halfLetterWidth = int(orientationIndicators[1]->textWidth())/2;
    orientationIndicators[1]->setPos(sliceImage->width()-Lcorrection,(sliceImage->height()/2)-halfLetterWidth);
    orientationIndicators[2]->setPos((sliceImage->width()/2)-halfLetterHeight,Scorrection);
    orientationIndicators[3]->setPos((sliceImage->width()/2)-halfLetterHeight,(sliceImage->height())-Lcorrection);
}

void SliceView::updateAllGraphicalViews()
{
    updateGraphicalView();
    horizontalSlice->updateGraphicalView();
    verticalSlice->updateGraphicalView();
}

void SliceView::updateGraphicalView()
{
    showCurrentSlice();
    updateLines();

    // The following code forces the view to be resized to its contents
    QGraphicsScene *scene = ui->gvSlice->scene();
    QRectF rect = scene->itemsBoundingRect();
    scene->setSceneRect(rect);
}

void SliceView::setIcons()
{
    ui->pbSaveSlice->setIcon(QIcon(":/Images/icons/save.svg"));
}

void SliceView::updateCursor()
{
    char mode = view->annotation->getAnnotationMode();
    if (mode == FREE_FORM_ANNOTATION) {
        float size = (view->annotation->getBrushRadius()+0.5) * abs_scaleFactor * 2;
        // if the size is bigger than 220, the window manager fails
        size = iftMin(iftMax(size, 4), 220);

        int height_slice = this->size().height();
        if (size > height_slice) {
            ui->gvSlice->setCursor(Qt::CrossCursor);
            return;
        }


        QPixmap pixmap(QSize(size, size));
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setPen(QPen(Qt::white));
        QRect r(QPoint(), pixmap.size());
        r.adjust(1, 1, -1, -1);
        painter.drawEllipse(r);
        painter.setPen(QPen(Qt::gray));
        QRect r2(QPoint(2,2), QSize(size-4, size-4));
        //r2.adjust(1, 1, -1, -1);
        painter.drawEllipse(r2);
        painter.end();

        QCursor cursor(pixmap);
        ui->gvSlice->setCursor(cursor);
    } else if (mode == ERASING_ANNOTATION){
        ui->gvSlice->setCursor(Qt::PointingHandCursor);
    } else {
        ui->gvSlice->setCursor(Qt::CrossCursor);
    }
}

QMyGraphicsView *SliceView::gvSlice()
{
    return ui->gvSlice;
}

void SliceView::slotZoomIn()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Zooming in", 0);

    scaleFactor = 1.25;
    abs_scaleFactor *= scaleFactor;
    ui->gvSlice->scale(scaleFactor,scaleFactor);
    //ui->gvRendering->scale(scaleFactor,scaleFactor);

    placeOrientationIndicators();
    updateLines();

    updateCursor();

    iftWriteOnLog("Zooming in took", elapsedTime.elapsed());
}

void SliceView::slotZoomOut()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Zooming out", 0);

    scaleFactor = .8;
    abs_scaleFactor *= scaleFactor;
    ui->gvSlice->scale(scaleFactor,scaleFactor);
    //ui->gvRendering->scale(scaleFactor,scaleFactor);

    placeOrientationIndicators();
    updateLines();

    updateCursor();

    iftWriteOnLog("Zooming out took", elapsedTime.elapsed());
}

void SliceView::slotNormalSize()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing to normal size", 0);

    scaleFactor = 1.0/abs_scaleFactor;
    ui->gvSlice->scale(scaleFactor,scaleFactor);
    //ui->gvRendering->scale(scaleFactor,scaleFactor);
    abs_scaleFactor = 1.0;

    placeOrientationIndicators();
    updateLines();

    updateCursor();

    iftWriteOnLog("Changing to normal size took", elapsedTime.elapsed());
}

void SliceView::slotToogleLinesVisibility()
{
    linesVisibility = !linesVisibility;
    updateGraphicalView();
}

void SliceView::slotSetGradientVisibility(int state)
{
    this->gradientVisibility = state == Qt::Checked;
    if (gradientVisibility && view->getGradient() == nullptr) {
        emit requestGradientCalculation();
    }
    updateGraphicalView();
}
