#include "mainwindow.h"
#include "ui_mainwindow.h"

/* Optimization bottlenecks:
 * 1) The kernel inversion method applies the whole layer again for all images, it would be
 * better to convolve only the inverted kernels and overwrite the corresponding activation maps
 * of all images
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    markers = new Markers();
    flim = new FLIM();
    orientation = new Orientation();
    this->last_known_dir = QDir::currentPath();
    this->DICEimportance = nullptr;

    // Creating Tmp dir and Disabling components
    cleanWorkspace();
    ui->leModelDir->setText(this->model_path);

    // Create connections
    createWidgetsConnections();

    // Initialize Marker box
    // - background
    hashMarkerName2MarkerColor.insert("Background",QColor(255,255,255));
    QPixmap pix(15,15);
    pix.fill(hashMarkerName2MarkerColor["Background"]);
    ui->cbMarkers->addItem(pix,QString("Background"));
    // - marker 1
    hashMarkerName2MarkerColor.insert("Marker 1",QColor(255,0,0));
    pix.fill(hashMarkerName2MarkerColor["Marker 1"]);
    ui->cbMarkers->addItem(pix,QString("Marker 1"));
    ui->cbMarkers->setCurrentIndex(1);

    // Initialize label colors
    iftColorTable *ctb_label = iftCategory21ColorTable(MAX_NUMBER_LABEL_COLORS); // Remember: this colortable is in YCbCr, not RGB
    for (int n = 0; n < MAX_NUMBER_LABEL_COLORS; n++){
        QColor c;
        c.setRgb(ctb_label->color[n].val[0],ctb_label->color[n].val[1],ctb_label->color[n].val[2]);
        this->hashLabelId2LabelColor.insert(n,c);
    }
    iftDestroyColorTable(&ctb_label);

    // Initialize Graphics View
    QGraphicsScene *scene = new QGraphicsScene();
    this->current_display_image = new QGraphicsPixmapItem();
    scene->addItem(this->current_display_image);
    ui->gvDisplay->setScene(scene);

    // Initialize Layers selection comboboxes
    QStandardItemModel *model = new QStandardItemModel(2,1);
    QStandardItem *item = new QStandardItem("");
    model->setItem(0,0,item);
    item = new QStandardItem("All");
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    model->setItem(1,0,item);
    ui->cbSelectTrainingLayers->setModel(model);
}

MainWindow::~MainWindow()
{
    iftDestroyImage(&this->current_ift_img);
    iftDestroyImage(&this->current_ift_img_norm);
    iftDestroyImage(&this->current_markers);
    iftDestroyImage(&this->current_activation);
    iftDestroyImage(&this->current_activation_interp);
    if (this->current_qt_img != nullptr)
        delete current_qt_img;
    if (this->current_item != nullptr)
        delete current_item;
    if (this->current_display_image != nullptr)
        delete current_display_image;
    if (this->markers != nullptr)
        delete this->markers;
    if (this->flim != nullptr)
        delete this->flim;
    if (this->orientation != nullptr)
        delete this->orientation;
    if (this->kernel_proj!= nullptr)
        delete this->kernel_proj;
    delete ui;
}

void MainWindow::updateGraphicsView()
{
    ui->gvDisplay->blockSignals(true);

    iftImage *img = nullptr, *markers = nullptr, *gt = nullptr, *gt_border = nullptr;
    int slice_num = ui->sbSliceNum->text().toInt();

    // checking if it is possible to update graphics view
    if (this->current_item == nullptr)
        return;
    if (this->current_ift_img_norm == nullptr)
        return;

    // cleaning used images
    if (this->current_qt_img != nullptr){
        delete this->current_qt_img;
        this->current_qt_img = nullptr;
    }

    if (iftIs3DImage(this->current_ift_img_norm)){
        iftImage *oriented_img = nullptr;
        if (ui->cb3DPlane->currentIndex() == 0) { // AXIAL
            img = iftGetXYSlice(this->current_ift_img_norm,slice_num);
            oriented_img = this->orientation->applyOrientationOnSlice(img,0);
        }else if (ui->cb3DPlane->currentIndex() == 1) { // CORONAL
            img = iftGetZXSlice(this->current_ift_img_norm,slice_num);
            oriented_img = this->orientation->applyOrientationOnSlice(img,1);
        } else if (ui->cb3DPlane->currentIndex() == 2) { // SAGITTAL
            img = iftGetYZSlice(this->current_ift_img_norm,slice_num);
            oriented_img = this->orientation->applyOrientationOnSlice(img,2);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Failed to select 3D plane.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.show();
            return;
        }
        // correcting image orientation
        iftDestroyImage(&img);
        img = oriented_img;
        oriented_img = nullptr;
    } else {
        img = iftCopyImage(this->current_ift_img_norm);
    }

    /* Adjusting window and level */
    double b,c;
    double bf,cf;
    b = (100-ui->hsBrightness->value());
    c = (100-ui->hsContrast->value());
    bf = b*255/100.0; // 255 -> normalization value
    cf = c*255/100.0; // 255 -> normalization value
    this->f1 = (2*bf - cf)/2;
    this->f2 = cf + f1;
    this->g1 = 0;
    this->g2 = 255;
    iftImage *img_stretch = iftLinearStretch(img,f1,f2,g1,g2);
    iftDestroyImage(&img);
    img = img_stretch;
    img_stretch = nullptr;
    
    /* Processing Markers */
    // -- For new image, check if there are saved ones and load it. Otherwise create new markers.

    if (this->current_markers != nullptr)
        iftDestroyImage(&this->current_markers);
    if (iftFileExists(this->current_marker_filename.toUtf8().data())){
        this->current_markers = iftReadImageByExt(this->current_marker_filename.toUtf8().data());
    } else {
        this->current_markers = iftCreateImageFromImage(this->current_ift_img_norm);
    }
    if (ui->cbMarkersVisibility->currentText() != "Off"){
        if (iftIs3DImage(this->current_markers)){
            iftImage *oriented_markers = nullptr;
            if (ui->cb3DPlane->currentIndex() == 0) { // AXIAL
                markers = iftGetXYSlice(this->current_markers,slice_num);
                oriented_markers = this->orientation->applyOrientationOnSlice(markers,0);
            }else if (ui->cb3DPlane->currentIndex() == 1) { // CORONAL
                markers = iftGetZXSlice(this->current_markers,slice_num);
                oriented_markers = this->orientation->applyOrientationOnSlice(markers,1);
            } else if (ui->cb3DPlane->currentIndex() == 2) { // SAGITTAL
                markers = iftGetYZSlice(this->current_markers,slice_num);
                oriented_markers = this->orientation->applyOrientationOnSlice(markers,2);
            }
            iftDestroyImage(&markers);
            if (ui->cbMarkersVisibility->currentText() == "Border"){
                markers = iftBorderImage(oriented_markers,false);
                iftDestroyImage(&oriented_markers);
            }else{
                markers = oriented_markers;
                oriented_markers = nullptr;
            }
        }else{
            markers = iftCopyImage(this->current_markers);
        }
    }

    /* Create QHash with existing markers */
    //insertMissingMarkersInHashTable();

    /* Processing GTs Labels*/
    if ((this->current_gt) && (ui->cbGtVisibility->currentText() != "Off"))
    {
        if (iftIs3DImage(this->current_gt)){
            iftImage *oriented_gt = nullptr;
            if (ui->cb3DPlane->currentIndex() == 0) { // AXIAL
                gt = iftGetXYSlice(this->current_gt,slice_num);
                oriented_gt = this->orientation->applyOrientationOnSlice(gt,0);
            }else if (ui->cb3DPlane->currentIndex() == 1) { // CORONAL
                gt = iftGetZXSlice(this->current_gt,slice_num);
                oriented_gt = this->orientation->applyOrientationOnSlice(gt,1);
            } else if (ui->cb3DPlane->currentIndex() == 2) { // SAGITTAL
                gt = iftGetYZSlice(this->current_gt,slice_num);
                oriented_gt = this->orientation->applyOrientationOnSlice(gt,2);
            }
            iftDestroyImage(&gt);
            gt_border = iftBorderImage(oriented_gt,false);
            if (ui->cbGtVisibility->currentText() == "Border"){
                gt = iftCopyImage(gt_border);
            }else{
                gt = oriented_gt;
                oriented_gt = nullptr;
            }
            iftDestroyImage(&oriented_gt);
        } else {
            gt = iftCopyImage(this->current_gt);
        }
    }


    /* Processing Activations */
    if ((ui->cbActivationVisibility->currentText() != "Off") && (this->current_activation_interp)){
        iftImage *activ = nullptr;
        iftImage *aux = nullptr;
        iftImage *oriented_aux = nullptr;
        if (ui->cbActivationVisibility->currentText() == "HeatMap"){
            iftColorTable *ctb = nullptr;
            if(iftIs3DImage(this->current_activation_interp)){
                if (ui->cb3DPlane->currentIndex() == 0) { // AXIAL
                    aux = iftGetXYSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,0);
                }else if (ui->cb3DPlane->currentIndex() == 1) { // CORONAL
                    aux = iftGetZXSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,1);
                } else if (ui->cb3DPlane->currentIndex() == 2) { // SAGITTAL
                    aux = iftGetYZSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,2);
                }
                iftDestroyImage(&aux);
                ctb      = iftBlueToRedColorTable(256);
                activ    = iftColorGrayImageByActivation(img,oriented_aux,ctb);
                iftDestroyImage(&oriented_aux);
            } else { //2D
                ctb      = iftBlueToRedColorTable(256);
                activ    = iftColorGrayImageByActivation(img,this->current_activation_interp,ctb);
            }
            iftDestroyColorTable(&ctb);
        } else if (ui->cbActivationVisibility->currentText() == "Saliency"){
            if(iftIs3DImage(this->current_activation_interp)){
                if (ui->cb3DPlane->currentIndex() == 0) { // AXIAL
                    aux = iftGetXYSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,0);
                }else if (ui->cb3DPlane->currentIndex() == 1) { // CORONAL
                    aux = iftGetZXSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,1);
                } else if (ui->cb3DPlane->currentIndex() == 2) { // SAGITTAL
                    aux = iftGetYZSlice(this->current_activation_interp,slice_num);
                    oriented_aux = this->orientation->applyOrientationOnSlice(aux,2);
                }
                iftDestroyImage(&aux);
                activ = oriented_aux;
                oriented_aux = nullptr;
            } else { //2D
                activ    = iftCopyImage(this->current_activation_interp);
            }
        }
        iftDestroyImage(&img);
        img = activ;
        activ = nullptr;
    }

    this->current_qt_img = iftImageToQImage(img,nullptr,this->hashMarkerName2MarkerColor,nullptr,nullptr,this->hashLabelId2LabelColor);
    QImage *qimg = iftImageToQImage(img,markers,this->hashMarkerName2MarkerColor,gt,gt_border,this->hashLabelId2LabelColor);
    this->current_display_image->setPixmap(QPixmap::fromImage(*qimg));
    delete qimg;
    qimg = nullptr;
    iftDestroyImage(&img);
    iftDestroyImage(&markers);
    iftDestroyImage(&gt);
    iftDestroyImage(&gt_border);

    ui->gvDisplay->blockSignals(false);
}

void MainWindow::updateListOfMarkers()
{
    // Reading all markers
    int n_markers = 0;
    ui->lwListOfMarkers->clear();
    QDir dir(this->markers_path+"/");
    QFileInfoList images = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    int position = 0;
    this->hashListOfMarkersItem2ScribbleCenterAndId.clear();
    foreach(QFileInfo filename, images){

        iftImage *markers = iftReadImageByExt(filename.filePath().toUtf8().data());
        iftLabeledSet *S = this->hashScribble2LabeledSet[filename.fileName()];
        while (S != nullptr){
            QListWidgetItem *item = new QListWidgetItem();
            QPixmap pix(15,15);
            // Adding same element in combobox
            if (markers->val[S->elem] == 1)
                pix.fill(this->hashMarkerName2MarkerColor[QString("Background")]);
            else
                pix.fill(this->hashMarkerName2MarkerColor[QString("Marker %1").arg(markers->val[S->elem]-1)]);
            item->setIcon(QIcon(pix));
            item->setText(filename.fileName());
            ui->lwListOfMarkers->addItem(item);
            long *elem_lbl = iftAllocLongIntArray(2);
            elem_lbl[0] = S->elem;
            elem_lbl[1] = markers->val[S->elem];
            this->hashListOfMarkersItem2ScribbleCenterAndId.insert(position,elem_lbl);
            S = S->next;
            n_markers++;            
            position++;
        }
        iftDestroyImage(&markers);
    }
    ui->lblNumMarkers->setText(QString("# markers: %1").arg(n_markers));
}

void MainWindow::addMarkerOnList()
{
    // Reading markers of current image
    iftAdjRel *A = iftSpheric(sqrt(3.));
    iftImage *comp = iftLabelComp(this->current_markers,A);
    iftDestroyAdjRel(&A);

    QFileInfo f(this->current_marker_filename);
    iftDestroyLabeledSet(&this->hashScribble2LabeledSet[f.fileName()]);
    this->hashScribble2LabeledSet[f.fileName()] = iftFirstElemOfComponents(comp);

    iftDestroyImage(&comp);
    updateListOfMarkers();
}

void MainWindow::cleanListOfMarkers()
{
    ui->lwListOfMarkers->clear();
    this->hashScribble2LabeledSet.clear();
    this->hashListOfMarkersItem2ScribbleCenterAndId.clear();
    ui->lblNumMarkers->setText(QString("# markers: "));
}

void MainWindow::createListOfMakers()
{
    this->hashScribble2LabeledSet.clear();

    // Reading all markers
    QDir dir(this->markers_path+"/");
    QFileInfoList images = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    foreach(QFileInfo filename, images){
        iftImage *markers = iftReadImageByExt(filename.filePath().toUtf8().data());
        iftAdjRel *A = iftSpheric(sqrt(3.));
        iftImage *comp = iftLabelComp(markers,A);
        iftDestroyAdjRel(&A);
        iftDestroyImage(&markers);

        this->hashScribble2LabeledSet[filename.fileName()] = iftFirstElemOfComponents(comp);
        iftDestroyImage(&comp);
    }
    updateListOfMarkers();
}

void MainWindow::on_lwListOfMarkers_itemDoubleClicked(QListWidgetItem *item)
{
    // Change current image to image with the clicked scrible
    QList<QListWidgetItem *>image_items = ui->lwTrain->findItems(item->text(),Qt::MatchExactly);
    if (image_items.count() > 1){
        qDebug() << "Two or more training images found with the same name.";
        return;
    } else if (image_items.count() == 0){
        qDebug() << "Image not found.";
        return;
    }
    on_lwTrain_itemDoubleClicked(image_items[0]);

    // Change Z slice if necessary
    if (iftIs3DImage(this->current_ift_img)){
        long center = this->hashListOfMarkersItem2ScribbleCenterAndId[ui->lwListOfMarkers->row(item)][0];
        iftVoxel u = iftGetVoxelCoord(this->current_ift_img,center);
        if (ui->cb3DPlane->currentIndex() == 0){ // Axial
            ui->sbSliceNum->setValue(u.z);
        } else if (ui->cb3DPlane->currentIndex() == 1){ // Coronal
            ui->sbSliceNum->setValue(u.y);
        } else if (ui->cb3DPlane->currentIndex() == 2){ // Sagittal
            ui->sbSliceNum->setValue(u.x);
        }
    }
}

void MainWindow::updateCursor()
{
    float size = (markers->current_ratio + 0.5) * this->abs_scale_factor * 2;

    QPixmap pixmap(QSize(size, size));
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::white));
    QRect r(QPoint(), pixmap.size());
    r.adjust(1, 1, -1, -1);
    painter.drawEllipse(r);
    painter.setPen(QPen(Qt::gray));
    QRect r2(QPoint(2,2), QSize(size-4, size-4));
    painter.drawEllipse(r2);
    painter.end();

    QCursor cursor(pixmap);
    ui->gvDisplay->setCursor(cursor);
}

void MainWindow::on_pbOpenTrainFolder_clicked()
{
    qDebug();

    /* Loading training data */

    /* Selecting training dir */
    QString training_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 this->last_known_dir,
                                                 QFileDialog::ShowDirsOnly);

    if (training_path.isEmpty()){
        return;
    }

    this->last_known_dir = training_path+"/../";

    /* Cleaning interface */
    // clear qGraphicsView when it is showing a training image and the training directory changes.
    // clear markers and gts
    cleanImageData();

    /* Creating Progress Dialog */
    QProgressDialog progress;
    progress.setLabelText("Loading training data");
    progress.setAutoClose(true);
    progress.setRange(0,100);
    progress.show();
    double step, cont;

    /* Creating symlinks with original data */
    QDir dir(training_path);
    QFileInfoList imageList = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    step = (100./4)/imageList.count();
    cont = 0;
    foreach(QFileInfo filename, imageList) {

        QString new_name = this->images_path + "/" + filename.fileName();
        QFile f(filename.absoluteFilePath().toUtf8().data());

        if (!iftIsValidFormat(new_name.toUtf8().data())){
            imageList.removeOne(filename);
            continue;
        }

        if (iftFileExists(new_name.toUtf8().data()))
            iftRemoveFile(new_name.toUtf8().data());

        f.link(new_name);

        cont += step;
        progress.setValue(int(cont));
        QApplication::processEvents(); // update GUI to redraw progress bar, not allowing user events
    }

    /* Creating thumbnails */
    this->thumbnail_path = this->tmp_dir + "/train_thumbnails/";
    createThumbnail(this->images_path);
    cont = cont + 25;
    progress.setValue(int(cont));
    QApplication::processEvents(); // update GUI to redraw progress bar, not allowing user events

    /* Display thumbnails with image names */
    step = (100 - cont)/imageList.count();
    foreach(QFileInfo filename, imageList) {

        QListWidgetItem *item = new QListWidgetItem(filename.fileName());

        QString thumbnail = this->thumbnail_path+filename.baseName()+".png";
        QPixmap pixmap(thumbnail);

        item->setCheckState(Qt::Unchecked);
        item->setData(Qt::DecorationRole,QVariant(pixmap));

        ui->lwTrain->addItem(item);

        cont += step;
        progress.setValue(int(cont));
        QApplication::processEvents(); // update GUI to redraw progress bar, not allowing user events
    }

    /* Checking if there is already a trained model loaded*/
    if (isModelTrained())
    {
        enableActivationVisualization();
        QSet<int> layersToComputeDICE;
        for (int l = 1; l <= ui->cbLayer->count(); l++)
            layersToComputeDICE.insert(l);
        computeDICEonActivation(layersToComputeDICE);
        updateActivations();
    } else {
        disableActivationVisualization();
    }
}

void MainWindow::on_lwTrain_itemDoubleClicked(QListWidgetItem *item)
{
    this->current_item = item;

    QString path = this->tmp_dir + "/train/" + item->text();
    // Reading traditional IFT image

    if (this->current_ift_img != nullptr){
        iftDestroyImage(&this->current_ift_img);
    }
    if (this->current_ift_img_norm != nullptr){
        iftDestroyImage(&this->current_ift_img_norm);
    }
    if (this->current_ift_mimg != nullptr){
        iftDestroyMImage(&this->current_ift_mimg);
    }
    if (this->current_activation != nullptr){
        iftDestroyImage(&this->current_activation);
    }
    if (this->current_activation_interp != nullptr){
        iftDestroyImage(&this->current_activation_interp);
    }
    if (this->current_gt != nullptr){
        iftDestroyImage(&this->current_gt);
    }

    this->setCursor(QCursor(Qt::WaitCursor));

    if (path.contains(".mimg")){
        ui->sbBandNum->blockSignals(true);
        this->current_ift_mimg = iftReadMImage(path.toUtf8().data());
        ui->sbBandNum->setRange(0,int(this->current_ift_mimg->m-1));
        ui->sbBandNum->setValue(int(this->current_ift_mimg->m)/2);
        ui->sbBandNum->blockSignals(false);
        this->current_ift_img = iftMImageToImage(this->current_ift_mimg,4095,int(this->current_ift_mimg->m)/2);
        this->current_ift_img_norm = iftMImageToImage(this->current_ift_mimg,255,int(this->current_ift_mimg->m)/2);
        if (iftIs3DImage(this->current_ift_img)){
            enable3DMImageVisualization();
            this->current_marker_filename = this->markers_path + "/" + QFileInfo(item->text()).baseName() + ".nii.gz";
        } else {
            enable2DMImageVisualization();
            this->current_marker_filename = this->markers_path + "/" + QFileInfo(item->text()).baseName() + ".png";
        }
    } else {
        this->current_ift_img = iftReadImageByExt(path.toUtf8().data());
        this->current_ift_img_norm = iftNormalize(this->current_ift_img,0,255);
        if (iftIs3DImage(this->current_ift_img)){
            enable3DImageVisualization();
            this->current_marker_filename = this->markers_path + "/" + QFileInfo(item->text()).baseName() + ".nii.gz";
        }else{
            enable2DImageVisualization();
            this->current_marker_filename = this->markers_path + "/" + QFileInfo(item->text()).baseName() + ".png";
        }
    }

    if (this->gt_path != ""){
        if (iftIs3DImage(this->current_ift_img)){
            QString gt_file_path = this->gt_path+"/"+QFileInfo(item->text()).baseName()+".nii.gz";
            if (!iftFileExists(gt_file_path.toUtf8().data())){
                QMessageBox msg;
                msg.setText("Unable to find image: "+gt_file_path);
                msg.show();
            } else {
                this->current_gt = iftReadImageByExt(gt_file_path.toUtf8().data());
            }
        }else{
            if (!iftFileExists(QString(this->gt_path+"/"+QFileInfo(item->text()).baseName()+".nii.gz").toUtf8().data())){
                QMessageBox msg;
                msg.setText("Unable to find image: "+QString(this->gt_path+"/"+QFileInfo(item->text()).baseName()+".png"));
                msg.show();
            } else {
                this->current_gt = iftReadImageByExt(QString(this->gt_path+"/"+QFileInfo(item->text()).baseName()+".png").toUtf8().data());
            }
        }
        if (current_gt != nullptr){
            int label_max = iftMaximumValue(this->current_gt);
            if (label_max == 0){
                iftDestroyImage(&this->current_gt);
            }
        }
        ui->cbGtVisibility->setEnabled(true);
    } else {
        ui->cbGtVisibility->setEnabled(false);
    }

    /* Centralizing image in Grpahics View */
    ui->gvDisplay->fitInView(0,0,ui->gvDisplay->width(),ui->gvDisplay->height());

    /* Setting Slice Number in UI */
    ui->sbSliceNum->blockSignals(true);
    ui->cb3DPlane->blockSignals(true);
    if (iftIs3DImage(this->current_ift_img)){
        this->current_axial_slice = this->current_ift_img->zsize/2;
        this->current_coronal_slice = this->current_ift_img->ysize/2;
        this->current_sagittal_slice = this->current_ift_img->xsize/2;
        //ui->cb3DPlane->setCurrentIndex(0);
        if (ui->cb3DPlane->currentIndex() == 0){
            ui->sbSliceNum->setRange(0,this->current_ift_img->zsize-1);
            ui->sbSliceNum->setValue(this->current_ift_img->zsize/2);
        } else if (ui->cb3DPlane->currentIndex() == 1){
            ui->sbSliceNum->setRange(0,this->current_ift_img->ysize-1);
            ui->sbSliceNum->setValue(this->current_ift_img->ysize/2);
        } else if (ui->cb3DPlane->currentIndex() == 2){
            ui->sbSliceNum->setRange(0,this->current_ift_img->xsize-1);
            ui->sbSliceNum->setValue(this->current_ift_img->xsize/2);
        }
    }
    ui->sbSliceNum->blockSignals(false);
    ui->cb3DPlane->blockSignals(false);

    /* Filling image information */
    ui->lblSize->setText(QString("Size: (%1, %2, %3)").arg(this->current_ift_img->xsize).arg(this->current_ift_img->ysize).arg(this->current_ift_img->zsize));
    ui->lblMinMax->setText(QString("Min./Max.: %1, %2").arg(iftMinimumValue(this->current_ift_img)).arg(iftMaximumValue(this->current_ift_img)));
    ui->lblBPP->setText(QString("BPP: %1").arg(iftNormalizationValue(iftMaximumValue(this->current_ift_img))));

    // Saving checked activations
    QBitArray *activation_checked = nullptr;
    if (ui->lwActivation->count() > 0){
        activation_checked = new QBitArray(ui->lwActivation->count(),false);
        for (int index = 0; index < ui->lwActivation->count(); index++){
            QListWidgetItem *item = ui->lwActivation->item(index);
            if (item->data(Qt::CheckStateRole) == Qt::Checked){
                activation_checked->setBit(index,true);
            }
        }
    }

    // Updating Activations
    updateActivations(activation_checked);

    // Updating graphics view
    updateGraphicsView();
    updateCursor();

    /* Scale new image back to its previous size */
    ui->gvDisplay->scale(this->abs_scale_factor,this->abs_scale_factor);

    this->setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::insertMissingMarkersInHashTable()
{
    QDir dir(this->markers_path);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
    foreach(QFileInfo markername, list) {
        iftImage *markers = iftReadImageByExt(markername.absoluteFilePath().toUtf8().data());
        int n_markers = iftMaximumValue(markers);
        if (n_markers > this->hashMarkerName2MarkerColor.count()){
            iftColorTable *ctb = iftCreateRandomColorTable(n_markers);
            for (int n = this->hashMarkerName2MarkerColor.count(); n < n_markers; n++)
            {
                // Adding element in hash
                QColor c;
                c.setRgb(ctb->color[n-1].val[0],ctb->color[n-1].val[1],ctb->color[n-1].val[2]);
                this->hashMarkerName2MarkerColor.insert(QString("Marker %1").arg(n),c);
                QPixmap pix(15,15);
                // Adding same element in combobox
                pix.fill(hashMarkerName2MarkerColor[QString("Marker %1").arg(n)]);
                ui->cbMarkers->addItem(pix,QString("Marker %1").arg(n));
            }
            iftDestroyColorTable(&ctb);
        }
        iftDestroyImage(&markers);
    }
}

void MainWindow::on_sbSliceNum_valueChanged(int value)
{
    if (value < 0)
        value = 0;
    if (value > ui->sbSliceNum->maximum())
        value = ui->sbSliceNum->maximum();

    if (this->current_ift_img_norm == nullptr)
        return;
    if (this->current_markers == nullptr)
        return;

    if (this->current_qt_img != nullptr){
        delete this->current_qt_img;
        this->current_qt_img = nullptr;
    }

    if (ui->cb3DPlane->currentIndex() == 0) //axial
        this->current_axial_slice = value;
    if (ui->cb3DPlane->currentIndex() == 1) //coronal
        this->current_coronal_slice = value;
    if (ui->cb3DPlane->currentIndex() == 2) //sagittal
        this->current_sagittal_slice = value;

    updateGraphicsView();
}

void MainWindow::on_cb3DPlane_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (this->current_ift_img == nullptr)
        return;

    ui->sbSliceNum->blockSignals(true);
    if (index == 0){ //axial
        ui->sbSliceNum->setRange(0,this->current_ift_img->zsize-1);
        ui->sbSliceNum->setValue(this->current_axial_slice);
    } else if (index == 1){ //coronal
        ui->sbSliceNum->setRange(0,this->current_ift_img->ysize-1);
        ui->sbSliceNum->setValue(this->current_coronal_slice);
    } else if (index == 2){ //sagittal
        ui->sbSliceNum->setRange(0,this->current_ift_img->xsize-1);
        ui->sbSliceNum->setValue(this->current_sagittal_slice);
    }
    ui->sbSliceNum->blockSignals(false);

    updateGraphicsView();
}

void MainWindow::on_sbBandNum_valueChanged(int value)
{
    if (value < 0)
        value = 0;
    if (value > ui->sbBandNum->maximum())
        value = ui->sbBandNum->maximum();

    if (this->current_item == nullptr){
        QMessageBox msgbox;
        msgbox.setText("No image has been selected yet.");
        msgbox.exec();
        return;
    }

    iftDestroyImage(&this->current_ift_img);
    iftDestroyImage(&this->current_ift_img_norm);

    this->current_ift_img = iftMImageToImage(this->current_ift_mimg,4095,value);
    this->current_ift_img_norm = iftMImageToImage(this->current_ift_mimg,255,value);

    updateGraphicsView();
}

void MainWindow::on_gvDisplay_printMarker(int x, int y)
{
    if ((this->current_ift_img == nullptr) || (this->current_markers == nullptr))
        return ;

    iftVoxel u,w;
    u.x = x;
    u.y = y;
    u.z = 0;
    w.x = x;
    w.y = y;
    w.z = 0;
    if (iftIs3DImage(this->current_ift_img)){
        this->orientation->plane = ui->cb3DPlane->currentIndex();
        this->orientation->slice = ui->sbSliceNum->value();
        w = this->orientation->mapPixelToVolume(u);
        if (iftValidVoxel(this->current_ift_img,w)){
            QColor c = hashMarkerName2MarkerColor[ui->cbMarkers->currentText()];
            int marker_label = ui->cbMarkers->currentText().replace("Marker ","").toInt()+1;
            if (ui->cbMarkers->currentText() == "Background")
                marker_label = 1;
            QImage qimg;
            qimg = this->current_display_image->pixmap().toImage();
            markers->draw(this->current_markers,&qimg,u,marker_label,c,this->orientation);
            if (ui->cbMarkersVisibility->currentText() != "Off")
                this->current_display_image->setPixmap(QPixmap::fromImage(qimg));
        }
    } else { // image is 2D
        if (iftValidVoxel(this->current_ift_img,u)){
            QColor c = hashMarkerName2MarkerColor[ui->cbMarkers->currentText()];
            int marker_label = ui->cbMarkers->currentText().replace("Marker ","").toInt()+1;
            if (ui->cbMarkers->currentText() == "Background")
                marker_label = 1;
            QImage qimg;
            qimg = this->current_display_image->pixmap().toImage();
            markers->draw(this->current_markers,&qimg,u,marker_label,c);
            if (ui->cbMarkersVisibility->currentText() != "Off")
                this->current_display_image->setPixmap(QPixmap::fromImage(qimg));
        }
    }
}

void MainWindow::on_gvDisplay_eraseMarker(int x, int y)
{

    if ((this->current_ift_img == nullptr) || (this->current_markers == nullptr))
        return ;

    QImage marked_img;
    iftVoxel u,w;
    u.x = x;
    u.y = y;
    u.z = 0;
    w.x = x;
    w.y = y;
    w.z = 0;
    if (iftIs3DImage(this->current_ift_img)){
        this->orientation->plane = char(ui->cb3DPlane->currentIndex());
        this->orientation->slice = ui->sbSliceNum->value();
        w = this->orientation->mapPixelToVolume(u);
        if (iftValidVoxel(this->current_ift_img,w)){
            marked_img = this->current_display_image->pixmap().toImage();
            markers->erase(this->current_markers,&marked_img,*this->current_qt_img,u,this->orientation);
            this->current_display_image->setPixmap(QPixmap::fromImage(marked_img));
        }
    } else { //image is 2D
        if (iftValidVoxel(this->current_ift_img,u)){
            marked_img = this->current_display_image->pixmap().toImage();
            markers->erase(this->current_markers,&marked_img,*this->current_qt_img,u);
            this->current_display_image->setPixmap(QPixmap::fromImage(marked_img));
        }
    }
}

void MainWindow::on_gvDisplay_released()
{
    if (this->current_markers == nullptr)
        return ;

    // Save markers in hard disk
    int max = iftMaximumValue(this->current_markers);
    if (max == 0){
        if (iftFileExists(this->current_marker_filename.toUtf8().data()))
            iftRemoveFile(this->current_marker_filename.toUtf8().data());
    } else if (max > 0)// || (iftFileExists(this->current_marker_filename.toUtf8().data())))
    {
        if (iftIs3DImage(this->current_markers))
            iftWriteImageByExt(this->current_markers,this->current_marker_filename.toUtf8().data());
        else {
            if (iftIsColorImage(this->current_markers)){
                free(this->current_markers->Cb);
                free(this->current_markers->Cr);
                this->current_markers->Cb = nullptr;
                this->current_markers->Cr = nullptr;
            }
            iftWriteImageByExt(this->current_markers,this->current_marker_filename.toUtf8().data());
        }
    }
    updateGraphicsView();
    addMarkerOnList();
}

void MainWindow::on_sbBrush_valueChanged(int value)
{
    markers->setBrush(value);
    double fl_ratio = 0.08 + ((markers->min_ratio - markers->current_ratio)/(markers->min_ratio - markers->max_ratio));

    QString ss = "background-color: qradialgradient(spread:pad, "
                 "cx:0.5, cy:0.5, "
                 "radius:%1, "
                 "fx:0.5, fy:0.5, "
                 "stop:0.9 rgba(0, 0, 0, 255), "
                 "stop:1.0 rgba(255, 255, 255, 255));";

    ui->wBrush->setStyleSheet(ss.arg(fl_ratio));
    updateCursor();
}

void MainWindow::on_gvDisplay_forwardSlice(int ratio)
{
    if (!iftIs3DImage(this->current_ift_img))
        return;
    int slice = 0;
    if (ui->cb3DPlane->currentIndex() == 0){ // axial
        slice = this->current_axial_slice + ratio;
    } else if (ui->cb3DPlane->currentIndex() == 1){ // coronal
        slice = this->current_coronal_slice + ratio;
    } else if (ui->cb3DPlane->currentIndex() == 2){ // sagittal
        slice = this->current_sagittal_slice + ratio;
    }
    ui->sbSliceNum->setValue(slice);
}

void MainWindow::on_gvDisplay_backwardSlice(int ratio)
{
    if (!iftIs3DImage(this->current_ift_img))
        return;
    int slice = 0;
    if (ui->cb3DPlane->currentIndex() == 0){ // axial
        slice = this->current_axial_slice + ratio;
    } else if (ui->cb3DPlane->currentIndex() == 1){ // coronal
        slice = this->current_coronal_slice + ratio;
    } else if (ui->cb3DPlane->currentIndex() == 2){ // sagittal
        slice = this->current_sagittal_slice + ratio;
    }
    ui->sbSliceNum->setValue(slice);
}

void MainWindow::on_gvDisplay_showIntensity(int x,int y)
{
    if (this->current_ift_img == nullptr)
        return ;

    iftVoxel u,w;
    u.x = x;
    u.y = y;
    u.z = 0;
    w.x = x;
    w.y = y;
    w.z = 0;
    if (iftIs3DImage(this->current_ift_img)){
        this->orientation->plane = char(ui->cb3DPlane->currentIndex());
        this->orientation->slice = ui->sbSliceNum->value();
        w = this->orientation->mapPixelToVolume(u);
        if (iftValidVoxel(this->current_ift_img,w)){
            int p = iftGetVoxelIndex(this->current_ift_img,w);
            ui->lblIntensityAtPoint->setText(QString("At (%1, %2, %3) --- Intensity: %4").arg(w.x,3,10,QLatin1Char('0')).arg(w.y,3,10,QLatin1Char('0')).arg(w.z,3,10,QLatin1Char('0')).arg(this->current_ift_img->val[p],4,10,QLatin1Char('0')));
            if (this->current_activation_interp != nullptr){
                ui->lblActivationAtPoint->setText(QString("Activation: %1").arg(this->current_activation_interp->val[p],4,10,QLatin1Char('0')));
            }
        }
    } else { //image is 2D
        if (iftValidVoxel(this->current_ift_img,u)){
            int p = iftGetVoxelIndex(this->current_ift_img,u);
            ui->lblIntensityAtPoint->setText(QString("At (%1, %2) --- Intensity: %3").arg(w.x,3,10,QLatin1Char('0')).arg(w.y,3,10,QLatin1Char('0')).arg(this->current_ift_img->val[p],4,10,QLatin1Char('0')));
            if (this->current_activation_interp != nullptr){
                ui->lblActivationAtPoint->setText(QString("Activation: %1").arg(this->current_activation_interp->val[p],4,10,QLatin1Char('0')));
            }
        }
    }
}

void MainWindow::on_pbAddMarker_clicked()
{
    QColor c = QColorDialog::getColor(Qt::yellow,this);
    if (c.isValid()){
        // finding marker index
        int marker_index = 1;
        while(true){
            bool found = false;
            for (int i = 1; i < ui->cbMarkers->count(); i++){
                if (QString("Marker %1").arg(marker_index) == ui->cbMarkers->itemText(i)){
                    found = true;
                    break;
                }
            }
            if (found)
                marker_index++;
            else
                break;
        }
        hashMarkerName2MarkerColor.insert(QString("Marker %1").arg(marker_index),c);
        QPixmap pix(15,15);
        pix.fill(hashMarkerName2MarkerColor[QString("Marker %1").arg(marker_index)]);
        ui->cbMarkers->addItem(pix,QString("Marker %1").arg(marker_index));
        ui->cbMarkers->model()->sort(0); // sort alphabetically
        ui->cbMarkers->setCurrentIndex(ui->cbMarkers->findText(QString("Marker %1").arg(marker_index)));
    }
}

bool MainWindow::thereAreScribblesOfMarker(QString marker_name)
{
    QHash<int, long*>::iterator i;
    for (i = this->hashListOfMarkersItem2ScribbleCenterAndId.begin();
         i != this->hashListOfMarkersItem2ScribbleCenterAndId.end();
         i++){
        if (i.value()[1] == 1){ //background
            return true;
        }
        if (QString("Marker %1").arg(i.value()[1]-1) == marker_name){
            return true;
        }
    }
    return false;
}

void MainWindow::on_pbRemoveMarker_clicked()
{
    if (ui->cbMarkers->currentText() == "Background")
        return;
    if (thereAreScribblesOfMarker(ui->cbMarkers->currentText())){
        QMessageBox msg;
        msg.setText("Scribbles found with the marker to be removed. Removal canceled.");
        msg.exec();
        return;
    }
    hashMarkerName2MarkerColor.remove(ui->cbMarkers->currentText());
    ui->cbMarkers->removeItem(ui->cbMarkers->currentIndex());
}

void MainWindow::on_pbLoadArch_clicked()
{
    QFileDialog dialog(this);
    QStringList filenames, filters;

    dialog.setViewMode(QFileDialog::List);

    filters << "FLIM Architecture (*json)";
    dialog.setNameFilters(filters);

    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (filenames.isEmpty()){
        return;
    }

    this->arch_path = filenames.at(0);
    QFileInfo fi(this->arch_path);

    // Create a file object and open it for reading.
    QFile jsonFile(filenames.at(0));
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    this->last_known_dir = fi.absoluteFilePath();

    // Read the entire file
    QByteArray data = jsonFile.readAll();
    // Create Json Document
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data));
    // Clear the text field
    ui->teFLIM_Arch->clear();
    // And set the contents of the Json object in the verification text field.
    ui->teFLIM_Arch->setPlainText(jsonDocument.toJson(QJsonDocument::Indented));

    // Load layers into combobox
    on_teFLIM_Arch_focusChanged();
}

void MainWindow::on_pbSaveArch_clicked()
{
    QMessageBox msgBox;
    QString save_arch_path = QFileDialog::getSaveFileName(this,tr("Saving Architecture"),QDir::currentPath(),tr("JSON file (*.json)"));

    if (save_arch_path == "")
        return;

    QFile file(save_arch_path);
    if (file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream << ui->teFLIM_Arch->toPlainText();
        file.flush();
        file.close();
    } else {
        msgBox.critical(this,tr("Saving Architecture"), tr("Failed to save architecture. File could not be opened."),QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    this->arch_path = save_arch_path;
    msgBox.setText("Architecture saved.");
    msgBox.exec();
}

void MainWindow::on_teFLIM_Arch_focusChanged()
{
    QString s = ui->teFLIM_Arch->toPlainText();

    QJsonDocument jsonDocument(QJsonDocument::fromJson(ui->teFLIM_Arch->toPlainText().toUtf8()));
    QJsonObject arch = jsonDocument.object();

    if (!arch.contains("nlayers"))
        return;

    // Saving marked layers
    int *is_layer_selected = new int(ui->cbSelectTrainingLayers->count()-2);
    for (int l = 0; l < ui->cbSelectTrainingLayers->count()-2; ++l) {
        if (ui->cbSelectTrainingLayers->itemData(l+2,Qt::CheckStateRole) == Qt::Checked){
            is_layer_selected[l] = Qt::Checked;
        } else {
            is_layer_selected[l] = Qt::Unchecked;
        }
    }

    // Clean list of layers that have been removed from the architecture
    ui->cbSelectTrainingLayers->clear();
    QStandardItem *item = new QStandardItem("");
    QStandardItemModel *m = static_cast<QStandardItemModel*>(ui->cbSelectTrainingLayers->model());
    m->setItem(0,0,item);
    item = new QStandardItem("All");
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setData(Qt::Unchecked, Qt::CheckStateRole);
    m->setItem(1,0,item);

    // List layers in the combobox
    int nlayers = arch.value("nlayers").toInt();
    QString list_of_layers;
    for (int i = 0; i < nlayers; i++){
        if ((arch.contains(QString("layer%1").arg(i+1))) && (ui->cbSelectTrainingLayers->findText(QString("Layer %1").arg(i+1)) == -1)){

            // if it does not have, insert it
            QStandardItem *item = new QStandardItem();
            item->setText(QString("Layer %1").arg(i+1));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(is_layer_selected[i], Qt::CheckStateRole);
            if (is_layer_selected[i] == Qt::Checked)
                list_of_layers = list_of_layers + QString("Layer %1").arg(i+1) + "; ";
            //QStandardItemModel *m = static_cast<QStandardItemModel*>(ui->cbSelectTrainingLayers->model());
            m->insertRow(i+2,item); // first two rows are blank and "All".
        }
    }
    m->item(0)->setText(list_of_layers);
    delete is_layer_selected;
}

bool MainWindow::isModelTrained()
{
    // Checking if at least the activations and outputs of the first layer exists
    if (!((iftDirExists((model_path+QString("/activation1")).toUtf8().data())) && (iftDirExists((model_path+QString("/layer1")).toUtf8().data()))))
    {
        qDebug() << "Model not trained. Activations not found.";
        return false;
    }
    // Checking if there are any images loaded into FLIMbuilder
    if (ui->lwTrain->count() == 0){
        return false;
    }

    // Checking if all images loaded in FLIMbuilder have its respective activation image
    QDir dir(model_path+QString("/activation1"));
    QFileInfoList list = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    if (list.count() != ui->lwTrain->count()){
        qDebug() << "Model not trained. Some training images do not have an activation map.";
        return false;
    }

    // Checking if the number of images as input of next layer is the same as the images loaded in FLIMbuilder    
    /*dir = QDir(model_path+QString("/layer1"));
    list = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    if (list.count() != ui->lwTrain->count()){
        qDebug() << "Model not trained. Number of training images varies.";
        return false;
    }*/

    // Checking if user wants to resume training or start over
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Message", "Trained model found. Resume training?\nAnswering NO will erase all files in current model folder.",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        QDir dir(this->model_path);
        dir.removeRecursively();
        iftMakeDir(this->model_path.toUtf8().data());
        return false;
    }

    return true;
}

void MainWindow::on_pbOpenModelDir_clicked()
{
    QString model_path = QFileDialog::getExistingDirectory(this, tr("Model Directory"),
                                                 this->last_known_dir,
                                                 QFileDialog::ShowDirsOnly);

    if (model_path.isEmpty()){
        return;
    }

    this->last_known_dir = model_path;

    ui->leModelDir->setText(model_path);
    this->model_path = model_path + "/";

    // In case the user chooses to make kernel selection without prior training. This can be removed if a project-based interface is implemented.
    this->flim->param_dir = this->model_path;

    // Checking if there is already a trained model
    // Here, we can check if at least the activations of the first convolutional layer are available. If that is the case,
    // then we can enable activations. The loading of activations from the remaining layer are done by the enableActivationVisualization()
    if (isModelTrained())
    {
        enableActivationVisualization();
        QSet<int> layersToComputeDICE;
        for (int l = 1; l <= ui->cbLayer->count(); l++)
            layersToComputeDICE.insert(l);
        computeDICEonActivation(layersToComputeDICE);
        updateActivations();
    } else {
        disableActivationVisualization();
    }
}

void MainWindow::on_cbSelectTrainingLayers_itemClicked(int index)
{
    if (index != 1) // if not the ALL button
        return;

    ui->cbSelectTrainingLayers->setItemText(0,"");
    for (int i = 2; i < ui->cbSelectTrainingLayers->count(); i++){
        if (ui->cbSelectTrainingLayers->itemData(index, Qt::CheckStateRole) == Qt::Checked){
            ui->cbSelectTrainingLayers->setItemData(i, Qt::Checked, Qt::CheckStateRole);
            ui->cbSelectTrainingLayers->setItemText(0,ui->cbSelectTrainingLayers->itemText(0)+ui->cbSelectTrainingLayers->itemText(i)+"; ");
        } else {
            ui->cbSelectTrainingLayers->setItemData(i, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

/*
bool MainWindow::transformMarkerImages2SeedFiles()
{
    // declaring variables
    bool existItemChecked = false;
    QMessageBox msgBox;

    // Transforming marker images into seed files
    // Clean seeds tmp dir
    if (iftDirExists(this->seeds_path.toUtf8().data())){
        QDir dir(this->seeds_path);
        dir.removeRecursively();
        iftMakeDir(this->seeds_path.toUtf8().data());
    }
    // Checking if user selected training images
    existItemChecked = false;
    for (int i = 0; i < ui->lwTrain->count(); i++)
    {
        QListWidgetItem *item = ui->lwTrain->item(i);
        if (item->checkState() == Qt::Checked){
            QString filename;
            QString seeds_name;
            char *aux;
            filename = this->markers_path + "/" + item->text();
            aux = iftBasename(item->text().toUtf8().data());
            seeds_name = this->seeds_path + "/" + aux + "-seeds.txt"; //this->images_path = this->seeds_path and -seeds.txt
            free(aux);
            if (!iftFileExists(filename.toUtf8().data()))
                continue;
            iftImage *label_img  = iftReadImageByExt(filename.toUtf8().data());
            if (iftMaximumValue(label_img) == 0){
                iftDestroyImage(&label_img);
                continue;
            }
            iftLabeledSet *seeds = nullptr;
            for (int p = 0; p < label_img->n; p++){
                if (label_img->val[p] > 0){
                    int label = label_img->val[p]-1;
                    iftInsertLabeledSet(&seeds,p,label);
                }
            }
            iftWriteSeeds(seeds,label_img,seeds_name.toUtf8().data());
            iftDestroyLabeledSet(&seeds);
            iftDestroyImage(&label_img);
            existItemChecked = true;
        }
    }
    if (!existItemChecked){ // If no image was selected, we consider all images with markers as training images
        foreach(QFileInfo filename, list) {
            QString seeds_name;
            char *aux;
            aux = iftBasename(filename.fileName().toUtf8().data());
            seeds_name = this->seeds_path + "/" + aux + "-seeds.txt"; //this->images_path = this->seeds_path and -seeds.txt
            free(aux);
            iftImage *label_img  = iftReadImageByExt((filename.absolutePath()+"/"+filename.fileName()).toUtf8().data());
            if (iftMaximumValue(label_img) == 0){
                iftDestroyImage(&label_img);
                continue;
            }
            iftLabeledSet *seeds = nullptr;
            for (int p = 0; p < label_img->n; p++){
                if (label_img->val[p] > 0){
                    int label = label_img->val[p]-1;
                    iftInsertLabeledSet(&seeds,p,label);
                }
            }
            iftWriteSeeds(seeds,label_img,seeds_name.toUtf8().data());
            iftDestroyLabeledSet(&seeds);
            iftDestroyImage(&label_img);
        }
    }

    // Checking if the markers were saved in the seeds directory
    QDir seeds_dir(this->seeds_path);
    list = seeds_dir.entryInfoList(QStringList() << "Markers" << "*.txt",QDir::Files);
    if (list.count() == 0 && ui->cbRunMode->currentIndex() != 1){ // feature extraction only
        msgBox.setText("No seeds detected. Error when converting marker images into seed files.");
        msgBox.exec();
        return false;
    }

    return true;
}
*/

void MainWindow::generateSeedsForLayer(int layer_index)
{
    // declaring variables
    bool existItemChecked = false;
    QMessageBox msgBox;

    // Transforming marker images into seed files
    // Clean seeds tmp dir
    this->seeds_path = this->model_path + "/markers" + QString::number(layer_index);
    if (iftDirExists(this->seeds_path.toUtf8().data())){
        QDir dir(this->seeds_path);
        dir.removeRecursively();
        iftMakeDir(this->seeds_path.toUtf8().data());
    } else {
        iftMakeDir(this->seeds_path.toUtf8().data());
    }
    // Checking if user selected training images
    existItemChecked = false;
    for (int i = 0; i < ui->lwTrain->count(); i++)
    {
        QListWidgetItem *item = ui->lwTrain->item(i);
        if (item->checkState() == Qt::Checked){
            QString filename;
            QString seeds_name;
            char *aux;
            filename = this->markers_path + "/" + item->text();
            aux = iftBasename(item->text().toUtf8().data());
            seeds_name = this->seeds_path + "/" + aux + "-seeds.txt"; //this->images_path = this->seeds_path and -seeds.txt
            free(aux);
            if (!iftFileExists(filename.toUtf8().data()))
                continue;
            iftImage *label_img  = iftReadImageByExt(filename.toUtf8().data());
            if (iftMaximumValue(label_img) == 0){
                iftDestroyImage(&label_img);
                continue;
            }
            iftLabeledSet *seeds = nullptr;
            for (int p = 0; p < label_img->n; p++){
                if (label_img->val[p] > 0){
                    int label = label_img->val[p]-1;
                    iftInsertLabeledSet(&seeds,p,label);
                }
            }
            iftWriteSeeds(seeds,label_img,seeds_name.toUtf8().data());
            iftDestroyLabeledSet(&seeds);
            iftDestroyImage(&label_img);
            existItemChecked = true;
        }
    }
    if (!existItemChecked){ // If no image was selected, we consider all images with markers as training images
        QDir markers_dir(this->markers_path);
        QFileInfoList list = markers_dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
        foreach(QFileInfo filename, list) {
            QString seeds_name;
            char *aux;
            aux = iftBasename(filename.fileName().toUtf8().data());
            seeds_name = this->seeds_path + "/" + aux + "-seeds.txt"; //this->images_path = this->seeds_path and -seeds.txt
            free(aux);
            iftImage *label_img  = iftReadImageByExt((filename.absolutePath()+"/"+filename.fileName()).toUtf8().data());
            if (iftMaximumValue(label_img) == 0){
                iftDestroyImage(&label_img);
                continue;
            }
            iftLabeledSet *seeds = nullptr;
            for (int p = 0; p < label_img->n; p++){
                if (label_img->val[p] > 0){
                    int label = label_img->val[p]-1;
                    iftInsertLabeledSet(&seeds,p,label);
                }
            }
            iftWriteSeeds(seeds,label_img,seeds_name.toUtf8().data());
            iftDestroyLabeledSet(&seeds);
            iftDestroyImage(&label_img);
        }
    }
}

QSet<QString> getListOfNewMarkers(QString seeds_path, QString images_path, QString model_path, int layer_index){
    QDir seeds_dir(seeds_path); // seeds_path = this->model_path + "/markersN", where N is the layer_index;
    QFileInfoList list = seeds_dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
    QSet<QString> listOfNewMarkers;
    foreach(QFileInfo filename, list) {
        // check if its corresponding activation is present in the layer input
        QString base = filename.baseName().replace("-seeds","").toUtf8().data();
        QString activ_path;
        if (layer_index == 1) { // input layer
             activ_path = images_path;
        } else {
             activ_path = model_path+"/layer"+QString::number(layer_index-1);
        }
        // checking if file exists in layer folder
        QDir input_dir(activ_path);
        QFileInfoList list_activ = input_dir.entryInfoList(QStringList() << "Activations" << "*",QDir::Files);
        bool found = false;
        foreach(QFileInfo filename_activ, list_activ){
            if (filename_activ.fileName().contains(base, Qt::CaseInsensitive)){
                found = true;
                break;
            }
        }
        if (!found){
            listOfNewMarkers.insert(base);
        }
    }
    return listOfNewMarkers;
}

bool areFilesMissingForFLIM(QSet<QString> listOfNewMarkers, QString activ_path)
{
    QSet<QString>::iterator i;
    bool missing_file = false;
    for (i = listOfNewMarkers.begin(); i != listOfNewMarkers.end(); ++i){
        // checking if file exists in layer folder
        QDir input_dir(activ_path);
        QFileInfoList list_activ = input_dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
        bool found = false;
        foreach(QFileInfo filename_activ, list_activ){
            if (filename_activ.fileName().contains((*i), Qt::CaseInsensitive)){
                found = true;
                break;
            }
        }
        if (!found){
            missing_file=true;
            break;
        }
    }
    return missing_file;
}

QSet<QString> findMissingFilesForFLIM(QSet<QString> listOfNewMarkers, QString activ_path)
{
    QSet<QString>::iterator i;
    QSet<QString> listOfNewImages;
    for (i = listOfNewMarkers.begin(); i != listOfNewMarkers.end(); ++i){
        // checking if file exists in layer folder
        qDebug() << *i;
        QDir input_dir(activ_path);
        QFileInfoList list_activ = input_dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
        foreach(QFileInfo filename_activ, list_activ){
            if (filename_activ.fileName().contains((*i), Qt::CaseInsensitive)){
                listOfNewImages.insert(filename_activ.fileName());
                break;
            }
        }
    }
    return listOfNewImages;
}

void MainWindow::generateInputForLayer(int layer_index, QSet<QString> listOfNewMarkers)
{
    QString activ_path;
    QString out_path = this->model_path+"/layer"+QString::number(layer_index-1);;
    /* Explain the -2 thing */
    if (layer_index - 2 <= 0){
        activ_path = this->images_path;
    } else {
        activ_path = this->model_path+"/layer"+QString::number(layer_index-2);
    }

    QDir weight_dir(this->model_path);
    QFileInfoList weight_list = weight_dir.entryInfoList(QStringList() << "Weights" << QString("conv%1-*").arg(layer_index-1),QDir::Files);
    if (weight_list.empty()){ // previous layer has no weights
        return;
    }

    // Checking if all images with new markers have their activations in the input of previous layer

    // If there are missing activations in the previous layer, compute them recursivelly.
    if (areFilesMissingForFLIM(listOfNewMarkers,activ_path))
        generateInputForLayer(layer_index-1, listOfNewMarkers);

    QSet<QString> listOfNewImages = findMissingFilesForFLIM(listOfNewMarkers,activ_path);

    /* Computing activations for new markers */
    // creating tmp dir
    char *aux_dir = iftMakeTempDir(NULL,NULL,this->model_path.toUtf8().data());
    // copying missing images into tmp folder
    QString tmp_list_path = this->model_path+"/tmp_list.csv" ;
    QFile file(tmp_list_path);
    if(file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream stream(&file);
        QSet<QString>::iterator i;
        for (i = listOfNewImages.begin(); i != listOfNewImages.end(); ++i){
            if (!QFile::copy(activ_path+"/"+(*i),QString(aux_dir)+"/"+(*i)))
                qDebug() << "could not copy file " + activ_path+"/"+(*i);
            stream << (*i) << "\n";
        }
        file.close();
    }

    // extract feature from tmp folder
    // --- reading architecture
    iftFLIMArch *tmp_arch = iftReadFLIMArch((this->model_path+QString("arch_layer%1.json").arg(layer_index-1)).toUtf8().data());
    // --- changing pooling stride to 1
    tmp_arch->layer[0].pool_stride = 1;

    // Extracting Features
    iftFLIMExtractFeaturesFromLayer(aux_dir,tmp_list_path.toUtf8().data(),tmp_arch,this->model_path.toUtf8().data(),layer_index-1,out_path.toUtf8().data(),nullptr,ui->sbDevice->value());
    iftDestroyFLIMArch(&tmp_arch);
    file.remove();

    // Deleting tmp dir and files
    iftRemoveDir(aux_dir);
    iftFree(aux_dir);
}

bool MainWindow::runIFS()
{
    /* Declaring variables */
    QMessageBox msgBox;
    /* Running IFS */
    // Getting current layer
    int current_training_layer = -1;
    for (int l = 2; l < ui->cbSelectTrainingLayers->count(); l++){
        if (ui->cbSelectTrainingLayers->itemData(l, Qt::CheckStateRole) == Qt::Checked){
            if (current_training_layer != -1){
                msgBox.setText("Interactive filter select requires only one layer at a time.");
                msgBox.exec();
                return false;
            }
            current_training_layer = ui->cbSelectTrainingLayers->itemText(l).replace("Layer ","").toInt();
        }
    }
    if (current_training_layer == -1){
        msgBox.setText("No layer selected");
        msgBox.exec();
        return false;
    }

    if (this->kernel_proj != nullptr)
        delete this->kernel_proj;

    // Projecting kernels and interactive selection
    this->kernel_proj = new Projection(nullptr,this->images_path,this->seeds_path,this->flim->arch_json,current_training_layer,this->model_path);
    this->kernel_proj->setMarkersHash(this->hashMarkerName2MarkerColor);
    this->kernel_proj->exec();

    if (this->kernel_proj->didSomething == false){
        return false;
    }

    // Generating input for next layer
    bool done = TRUE;
    if (flim->modelExists(current_training_layer)){
        this->setCursor(QCursor(Qt::WaitCursor));
        done = this->flim->ExtractFeaturesFromLayerForTraining(current_training_layer);
        this->setCursor(QCursor(Qt::ArrowCursor));
    } else {
        msgBox.setText(QString("Model not found."));
        msgBox.exec();
        return false;
    }
    if (!done){
        msgBox.setText(QString("Failed to extract features of layer %1.").arg(current_training_layer));
        msgBox.exec();
        return false;
    }

    // Applying feature extraction after kernels are selected
    done = TRUE;
    if (flim->modelExists(current_training_layer)){
        this->setCursor(QCursor(Qt::WaitCursor));
        done = this->flim->ExtractFeaturesFromLayer(current_training_layer);
        this->setCursor(QCursor(Qt::ArrowCursor));
    }

    return true;
}

bool MainWindow::runFLIM()
{
    /* Declaring variables */
    QMessageBox msgBox;
    /* Train and feature extraction */
    this->setCursor(QCursor(Qt::WaitCursor));
    bool done = TRUE;
    for (int l = 2; l < ui->cbSelectTrainingLayers->count(); l++){
        if (ui->cbSelectTrainingLayers->itemData(l, Qt::CheckStateRole) == Qt::Checked){
            int layer_index = ui->cbSelectTrainingLayers->itemText(l).replace("Layer ","").toInt();

            /* Seeds */
            generateSeedsForLayer(layer_index);
            QSet<QString> listOfNewMarkers = getListOfNewMarkers(this->seeds_path,this->images_path,this->model_path,layer_index);
            if (!listOfNewMarkers.empty())
                generateInputForLayer(layer_index,listOfNewMarkers);

            /* Preparing FLIM */
            if (!prepareFLIM()){
                msgBox.setText("Error when parsing architecture. Please check if the architecture is correct.");
                msgBox.exec();
                return false;
            }

            /* FLIM */
            done = this->flim->TrainLayerFLIM(layer_index);
            if (!done){
                msgBox.setText(QString("Failed to train layer %1.").arg(layer_index));
                msgBox.exec();
                return false;
            }
            if (flim->modelExists(layer_index)){
                done = this->flim->ExtractFeaturesFromLayer(layer_index);
            }else{
                this->setCursor(QCursor(Qt::ArrowCursor));
                msgBox.setText(QString("Model not found."));
                msgBox.exec();
                return false;
            }
            if (!done){
                this->setCursor(QCursor(Qt::ArrowCursor));
                msgBox.setText(QString("Failed to extract features of layer %1.").arg(layer_index));
                msgBox.exec();
                return false;
            }
        }
    }

    this->setCursor(QCursor(Qt::ArrowCursor));

    return true;
}

bool MainWindow::runFeatExtract()
{
    /* Declaring variables */
    QMessageBox msgBox;
    /* Feature extraction */
    this->setCursor(QCursor(Qt::WaitCursor));
    bool done = true;
    for (int l = 2; l < ui->cbSelectTrainingLayers->count(); l++){
        if (ui->cbSelectTrainingLayers->itemData(l, Qt::CheckStateRole) == Qt::Checked){
            int layer_index = ui->cbSelectTrainingLayers->itemText(l).replace("Layer ","").toInt();
            if (flim->modelExists(layer_index)){
                done = this->flim->ExtractFeaturesFromLayer(layer_index);
            } else {
                this->setCursor(QCursor(Qt::ArrowCursor));
                msgBox.setText(QString("Model not found."));
                msgBox.exec();
                return false;
            }
            if (!done){
                this->setCursor(QCursor(Qt::ArrowCursor));
                msgBox.setText(QString("Failed to extract features of layer %1.").arg(layer_index));
                msgBox.exec();
                return false;
            }
            this->flim->ExtractFeaturesFromLayerForTraining(layer_index); // generate input for next layer
        }
    }

    this->setCursor(QCursor(Qt::ArrowCursor));
    return true;
}

bool MainWindow::canRun(QSet<int> *layersToComputeDICE)
{
    QMessageBox msgBox;
    /* Checking if training set has been loaded */
    if (ui->lwTrain->count() == 0){
        msgBox.setText("No training images found. Please first load the training images");
        msgBox.exec();
        return false;
    }

    /* Checking if at least one layer has been selected for training */
    bool existItemChecked = false;
    for (int l = 2; l < ui->cbSelectTrainingLayers->count(); l++){
        if (ui->cbSelectTrainingLayers->itemData(l, Qt::CheckStateRole) == Qt::Checked){
            existItemChecked = true;
            int layer_index = ui->cbSelectTrainingLayers->itemText(l).replace("Layer ","").toInt();
            layersToComputeDICE->insert(layer_index);
        }
    }
    if (!existItemChecked){
        msgBox.setText("No layers selected.");
        msgBox.exec();
        return false;
    }


    // Checking if there are any markers at all
    QDir dir(this->markers_path);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
    if (list.count() == 0 && ui->cbRunMode->currentIndex() == 0){
        msgBox.setText("No markers detected.");
        msgBox.exec();
        return false;
    }

    return true;
}

void MainWindow::on_pbRun_clicked()
{
    this->model_path = ui->leModelDir->text() + "/";
    if (!iftDirExists(ui->leModelDir->text().toUtf8().data())){
        iftMakeDir(this->model_path.toUtf8().data());
    }

    /* Declaring variables */
    QMessageBox msgBox;
    QSet<int> layersToComputeDICE;

    /* Cleaning data */
    iftDestroyImage(&this->current_activation);
    iftDestroyImage(&this->current_activation_interp);

    if (!canRun(&layersToComputeDICE)){
        return;
    }

    if (ui->cbRunMode->currentIndex() == 2){ // IFS

        if (!runIFS())
            return;

    } else if(ui->cbRunMode->currentIndex() == 0){ // Train and Feature Extraction

        if (!runFLIM())
            return;

    } else if(ui->cbRunMode->currentIndex() == 1){ // Feature Extraction only

        if (!runFeatExtract())
            return;
    }

    enableActivationVisualization();
    computeDICEonActivation(layersToComputeDICE);
    updateActivations();
    updateGraphicsView();

    msgBox.setText("Training and Feat. Extraction complete.");
    msgBox.exec();

    QJsonDocument doc(this->flim->arch_json);
    QByteArray barr = doc.toJson();
    ui->teFLIM_Arch->setPlainText(QString(barr));
}

void MainWindow::updateActivations(QBitArray *activation_checked)
{
    if ((ui->cbLayer->currentIndex() == -1) || (ui->cbLayer->currentText() == ""))
        return;
    if (this->current_item == nullptr)
        return;
    if (this->current_ift_img_norm == nullptr)
        return;

    if (this->current_activation != nullptr)
        iftDestroyImage(&this->current_activation);
    if (this->current_activation_interp)
        iftDestroyImage(&this->current_activation_interp);

    // -- Initialize list widget
    ui->lwActivation->clear();
    ui->lwActivation->setViewMode(QListWidget::IconMode);
    ui->lwActivation->setIconSize(QSize(50,50));
    ui->lwActivation->setResizeMode(QListWidget::Adjust);

    // -- Initialize variables
    QString filename(QFileInfo(this->current_item->text()).baseName());

    //int resize_width = 50, resize_height = 50;
    int l = ui->cbLayer->currentText().replace("layer","").toInt();
    QString file_path = this->model_path + QString("/activation%1/").arg(l) + QFileInfo(this->current_item->text()).baseName() + ".mimg";
    iftMImage *mimg = iftReadMImage(file_path.toUtf8().data());
    for (int i = 0; i < int(mimg->m); i++)
    {
        iftImage *band = iftMImageToImage(mimg,255,int(i));
        iftImage *band_resize = nullptr;
        if (iftIs3DImage(band)){
            band_resize = iftGetXYSlice(band,band->zsize/2);
        } else {
            band_resize = iftCopyImage(band);
        }
        iftDestroyImage(&band);

        char item_name[20];
        sprintf(item_name,"kernel %03d",i+1);
        QImage *qimg = iftImageToQImage(band_resize,nullptr,this->hashMarkerName2MarkerColor,nullptr,nullptr,this->hashLabelId2LabelColor);
        QListWidgetItem *item = new QListWidgetItem(QIcon(QPixmap::fromImage(*qimg)),item_name);        
        item->setCheckState(Qt::Unchecked);        
        if (this->hashDICEscorePerImagePerLayer[l][filename] != nullptr){
            item->setToolTip(QString("DICE score: "+QString::number(iftMatrixElem(hashDICEscorePerImagePerLayer[l][filename],0,i))));
        } else {
            item->setToolTip(QString("DICE score: not available."));
        }
        if (activation_checked)
            if (activation_checked->count() == int(mimg->m))
                if (activation_checked->at(i))
                    item->setCheckState(Qt::Checked);

        ui->lwActivation->addItem(item);

        delete qimg;
        iftDestroyImage(&band_resize);
    }
    iftDestroyMImage(&mimg);
}

void MainWindow::on_cbMarkers_currentTextChanged(QString s)
{
    QColor c = this->hashMarkerName2MarkerColor[s];
    QString ss = "background-color: rgb("+QString::number(c.red())+","+QString::number(c.green())+","+QString::number(c.blue())+");";
    ui->wMarkerColor->setStyleSheet(ss);
}

void MainWindow::on_cbLayer_currentIndexChanged(int)
{
    qDebug();

    QSet<int> layersToComputeDICE;
    computeDICEonActivation(layersToComputeDICE);
    updateActivations();
}

void MainWindow::on_lwActivation_itemDoubleClicked(QListWidgetItem *item)
#define ROUND(x) ((x < 0)?int(x-0.5):int(x+0.5))
{
    if (this->current_item == nullptr)
    {
        QMessageBox msgbox;
        msgbox.setText("No image selected. Please first select an image and then its activation.");
        msgbox.exec();
        return;
    }

    // Initialization
    const QString s("kernel ");
    int band = item->text().remove(QString("kernel ")).toInt()-1;
    QString activation_dir = this->model_path + QString("/activation%1/").arg(ui->cbLayer->currentText().replace("layer","").toInt())+"/";
    iftMImage *mimg = iftReadMImage((activation_dir+QFileInfo(this->current_item->text()).baseName()+".mimg").toUtf8().data());
    if (this->current_activation != nullptr)
        iftDestroyImage(&this->current_activation);
    this->current_activation = iftMImageToImage(mimg,255,band);
    iftDestroyMImage(&mimg);

    if (this->current_activation_interp != nullptr)
        iftDestroyImage(&this->current_activation_interp);
    // Interpolating activation to the same space as the input image, in case of pooling with stride > 1
    if ((this->current_ift_img->xsize == this->current_activation->xsize) &&
        (this->current_ift_img->ysize == this->current_activation->ysize) &&
        (this->current_ift_img->zsize == this->current_activation->zsize)) {
        this->current_activation_interp = iftCopyImage(this->current_activation);
    } else {
        if (iftIs3DImage(this->current_activation)){
            double s, sx, sy, sz;
            sx  = double(this->current_ift_img->xsize/this->current_activation->xsize);
            if (this->current_ift_img->xsize != ROUND(sx * this->current_activation->xsize)){
                for (s=sx-0.5; this->current_ift_img->xsize != ROUND(s * this->current_activation->xsize); s+=0.0001);
                sx  = s;
            }
            sy  = double(this->current_ift_img->ysize/this->current_activation->ysize);
            if (this->current_ift_img->ysize != ROUND(sy * this->current_activation->ysize)){
                for (s=sy-0.5; this->current_ift_img->ysize != ROUND(s * this->current_activation->ysize); s+=0.0001);
                sy  = s;
            }
            sz  = double(this->current_ift_img->zsize/this->current_activation->zsize);
            if (this->current_ift_img->zsize != ROUND(sz * this->current_activation->zsize)){
                for (s=sz-0.5; this->current_ift_img->zsize != ROUND(s * this->current_activation->zsize); s+=0.0001);
                sz  = s;
            }
            this->current_activation_interp = iftInterp(this->current_activation,float(sx),float(sy),float(sz));
        } else {
            double s, sx, sy;
            sx  = double(this->current_ift_img->xsize/this->current_activation->xsize);
            if (this->current_ift_img->xsize != ROUND(sx * this->current_activation->xsize)){
                for (s=sx-0.5; this->current_ift_img->xsize != ROUND(s * this->current_activation->xsize); s+=0.0001);
                sx  = s;
            }
            sy  = double(this->current_ift_img->ysize/this->current_activation->ysize);
            if (this->current_ift_img->ysize != ROUND(sy * this->current_activation->ysize)){
                for (s=sy-0.5; this->current_ift_img->ysize != ROUND(s * this->current_activation->ysize); s+=0.0001);
                sy  = s;
            }
            this->current_activation_interp = iftInterp2D(this->current_activation,float(sx),float(sy));
        }
    }

    updateGraphicsView();
}

void MainWindow::on_lwActivation_itemChanged(QListWidgetItem *item)
{
    if (this->kernel_proj == nullptr){
        qDebug() << "projection not found.";
        return;
    }

    int sample = item->text().replace("kernel ","").toInt();
    if (item->checkState() == Qt::Checked){
        kernel_proj->selectSampleFromMainWindow(sample-1);
    } else {
        kernel_proj->deselectSampleFromMainWindow(sample-1);
    }

    kernel_proj->paintNodes();
}

bool MainWindow::prepareFLIM()
{

    /* Transforming FLIM Architecture in TextEdit to JsonObject */
    QJsonDocument jsonDocument(QJsonDocument::fromJson(ui->teFLIM_Arch->toPlainText().toUtf8()));
    this->flim->arch_json = jsonDocument.object();

    /* Checking if architecture is correct */
    if (!this->flim->IsArchitectureValid()){
        // IsArchitectureValid() already shows the error message
        return false;
    }

    this->flim->orig_dir = this->images_path;
    this->flim->seeds_dir = this->seeds_path;
    this->flim->param_dir = this->model_path;
    this->flim->device = ui->sbDevice->value();
    this->flim->object_dir = "";

    return true;
}

void MainWindow::on_pbSelectKernelsManual_clicked()
{
    QMessageBox::StandardButton reply;
    if (!prepareFLIM())
        return;

    bool activation_selected = false;
    for (int i = 0; i < ui->lwActivation->count(); i++){
        QListWidgetItem *item = ui->lwActivation->item(i);
        if (item->checkState() == Qt::Checked){
            activation_selected = true;
            break;
        }
    }
    if (!activation_selected){
        QMessageBox msg;
        msg.setText("No activation selected.");
        msg.exec();
        return;
    }

    if (this->kernel_proj != nullptr){
        reply = QMessageBox::question(this, "Message", "Projection will be closed for manual kernel selection. Proceed?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            this->kernel_proj->close();
            delete this->kernel_proj;
            this->kernel_proj = nullptr;
        } else {
            return;
        }
    }

    int layer = ui->cbLayer->currentText().replace("layer","").toInt(); // getting current layer
    QString kernel_bank_path = this->model_path + QString("/conv%1-kernels.npy").arg(layer);
    QString manual_kernels_file = this->model_path + QString("/manual_kernels%1.json").arg(layer);
    QJsonObject obj;
    QJsonArray arr;

    // Selecting checked kernels
    for (int index = 0, i = 0; index < ui->lwActivation->count(); index++){
        QListWidgetItem *item = ui->lwActivation->item(index);
        if (item->data(Qt::CheckStateRole) == Qt::Checked){
            arr.insert(i++,index);
        }
    }

    if (arr.empty())
        return;

    obj.insert("selected_kernels",arr);

    QJsonDocument save_manual_kernels(obj);
    QFile save_arch(manual_kernels_file);
    if (!save_arch.open(QIODevice::WriteOnly)){
        qWarning("Could not open kernels");
        return;
    }
    save_arch.write(save_manual_kernels.toJson());
    save_arch.close();

    iftMatrix *selected_kernels = iftFLIMSelectKernelsManual(kernel_bank_path.toUtf8().data(),manual_kernels_file.toUtf8().data());
    iftWriteMatrix(selected_kernels,kernel_bank_path.toUtf8().data());
    iftDestroyMatrix(&selected_kernels);

    this->setCursor(QCursor(Qt::WaitCursor));

    // Updating input of next layer with selected kernels
    this->flim->ExtractFeaturesFromLayerForTraining(layer);

    // Updating activations with selected kernels and correct stride
    this->flim->ExtractFeaturesFromLayer(layer);

    QSet<int> layersToComputeDICE;
    layersToComputeDICE.insert(layer);
    computeDICEonActivation(layersToComputeDICE);
    updateActivations();
    this->setCursor(QCursor(Qt::ArrowCursor));


    reply = QMessageBox::question(this, "Message", "Do you wish to update the FLIM Architecture?.",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QJsonValueRef ref = this->flim->arch_json[QString("layer")+QVariant(layer).toString()].toObject()[QString("conv")];
        QJsonObject modifier = ref.toObject();
        modifier.insert("noutput_channels",arr.count());
        ref = modifier;
        QJsonDocument jsonDocument(flim->arch_json);
        ui->teFLIM_Arch->clear();
        ui->teFLIM_Arch->setPlainText(jsonDocument.toJson(QJsonDocument::Indented));
    }

    QMessageBox msg;
    msg.setText("Manual Kernel Selection complete.");
    msg.exec();
}

void MainWindow::on_pbProjectKernels_clicked()
{
    if (this->kernel_proj != nullptr)
        delete this->kernel_proj;

    this->kernel_proj = new Projection(nullptr,this->model_path,this->seeds_path,this->gt_path, ui->cbLayer->currentText().replace("layer","").toInt());
    this->kernel_proj->setMarkersHash(this->hashMarkerName2MarkerColor);
    this->kernel_proj->setDICEImportance(this->DICEimportance);
    ui->cbLayer->setEnabled(false);
    ui->pbProjectKernels->setEnabled(false);

    connect(this->kernel_proj, SIGNAL(projectionClosed()), this, SLOT(on_projectionClosed()), Qt::UniqueConnection);
    connect(this->kernel_proj, SIGNAL(kernelsSelected(iftSet*)), this, SLOT(on_kernelsSelected(iftSet*)), Qt::UniqueConnection);
    connect(this->kernel_proj, SIGNAL(kernelDoubleClicked(int)), this, SLOT(on_kernelDoubleClicked(int)), Qt::UniqueConnection);
    this->kernel_proj->show();
}

void MainWindow::on_pbInvertKernels_clicked()
{
    int layer = ui->cbLayer->currentText().replace("layer","").toInt(); // getting current layer
    QJsonArray arr;

    // Selecting checked kernels
    for (int index = 0, i = 0; index < ui->lwActivation->count(); index++){
        QListWidgetItem *item = ui->lwActivation->item(index);
        if (item->data(Qt::CheckStateRole) == Qt::Checked){
            arr.insert(i++,index);
        }
    }

    if (arr.empty()){
        QMessageBox msgbox;
        msgbox.setText("No kernels selected");
        msgbox.exec();
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Message", "Do you wish to invert selected kernels.",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    QString kernel_path = this->model_path+QString("conv%1-kernels.npy").arg(layer);

    if (!iftFileExists(kernel_path.toUtf8().data())){
        QMessageBox msgbox;
        msgbox.setText("Unable to find file "+kernel_path);
        msgbox.exec();
        return;
    }

    iftMatrix *kernels = iftReadMatrix(kernel_path.toUtf8().data());

    // cols -> kernels
    // rows -> dimension
    int feat = kernels->nrows;
    for (int i = 0; i < arr.count(); i++)
    {
        for (int f = 0; f < feat; f++){
            iftMatrixElem(kernels,arr.at(i).toInt(),f) = -iftMatrixElem(kernels,arr.at(i).toInt(),f);
        }
    }

    iftWriteMatrix(kernels,kernel_path.toUtf8().data());
    iftDestroyMatrix(&kernels);

    this->setCursor(QCursor(Qt::WaitCursor));
    // Updating input of next layer with selected kernels

    // --- setting stride to 1
    //int stride = flim->arch_json.value(QString("layer")+QVariant(layer).toString()).toObject().value(QString("pooling")).toObject().value(QString("stride")).toInt();

    // ----- all this is required to change a single value in a QJsonObject :(
    // ----- changing pooling stride to 1
    /*QJsonValueRef ref = this->flim->arch_json[QString("layer")+QVariant(layer).toString()].toObject()[QString("pooling")];
    QJsonObject mod_stride = ref.toObject();
    mod_stride.insert("stride",1);
    ref = mod_stride;*/

    // --- generating input for next layer
    this->flim->ExtractFeaturesFromLayerForTraining(layer);

    // ----- changing stride back
    /*
    mod_stride.insert("stride",stride);
    ref = mod_stride;*/

    // --- moving output to folder specific layer
    /*QDir dir(this->model_path+"layer"+QVariant(layer).toString()+"/");
    QFileInfoList images = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    foreach(QFileInfo filename, images) {
        // Check if file exists for the QFile::copy returns false if the file already exists
        if (QFile::exists(this->model_path+"layer"+QVariant(layer).toString()+"/"+filename.fileName()))
        {
            QFile::remove(this->model_path+"layer"+QVariant(layer).toString()+"/"+filename.fileName());
        }
        QFile::copy(this->model_path+"activation"+QVariant(layer).toString()+"/"+filename.fileName(),
                    this->model_path+"layer"+QVariant(layer).toString()+"/"+filename.fileName());
    }*/

    // Updating activations with selected kernels and correct stride
    this->flim->ExtractFeaturesFromLayer(layer);

    updateActivations();
    this->setCursor(QCursor(Qt::ArrowCursor));

}

void MainWindow::on_projectionClosed()
{
    ui->cbLayer->setEnabled(true);
    ui->pbProjectKernels->setEnabled(true);
    delete this->kernel_proj;
    this->kernel_proj = nullptr;
}

void MainWindow::on_kernelsSelected(iftSet *kernel_indexes)
{
    if (ui->lwActivation->count() == 0){
        return;
    }

    ui->lwActivation->blockSignals(true);

    // cleaning kernels selection
    for (int i = 0; i < ui->lwActivation->count(); i++){
        QListWidgetItem *item = ui->lwActivation->item(i);
        item->setCheckState(Qt::Unchecked);
    }

    // selecting kernels from projection
    iftSet *aux = kernel_indexes;
    while (aux != nullptr)
    {
        int index = aux->elem;
        QListWidgetItem *item = ui->lwActivation->item(index-1);
        if (item->data(Qt::CheckStateRole) == Qt::Checked)
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);
        aux = aux->next;
    }

    ui->lwActivation->blockSignals(false);
}

void MainWindow::on_kernelDoubleClicked(int index)
{
    qDebug();
    on_lwActivation_itemDoubleClicked(ui->lwActivation->item(index-1));
}

void MainWindow::createThumbnail(QString images_path)
{
    qDebug();

    // Getting list of images
    int resize_width = 50, resize_height = 50;
    QDir dir(images_path);
    QFileInfoList images = dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    foreach(QFileInfo filename, images) {
        if (filename.completeSuffix() == "mimg"){
            iftMImage *mimg = iftReadMImage(filename.absoluteFilePath().toUtf8().data());
            if (iftIs3DMImage(mimg)){
                long n_band                       = mimg->m/2;
                iftImage *img_band                = iftMImageToImage(mimg,4095,int(n_band));
                iftImage *slice_band              = iftGetXYSlice(img_band, img_band->zsize/2);
                iftImage *slice_band_norm         = nullptr;
                iftImage *slice_band_norm_resize  = nullptr;
                iftDestroyImage(&img_band);

                if (iftMaximumValue(slice_band) != iftMinimumValue(slice_band)){
                    slice_band_norm = iftNormalize(slice_band,0,255);
                } else {
                    slice_band_norm = iftCopyImage(slice_band);
                }
                iftDestroyImage(&slice_band);

                slice_band_norm_resize = iftResizeImage(slice_band_norm,resize_width,resize_height,1);
                iftDestroyImage(&slice_band_norm);

                QString thumbnail = this->thumbnail_path + "/" + filename.baseName() + ".png";
                iftWriteImageByExt(slice_band_norm_resize,thumbnail.toUtf8().data());
                iftDestroyImage(&slice_band_norm_resize);

            } else { // Is 2D multi-band image
                long n_band                       = mimg->m/2;
                iftImage *img_band                = iftMImageToImage(mimg,255,int(n_band));
                iftImage *img_band_norm           = nullptr;
                iftImage *img_band_norm_resize    = nullptr;

                if (iftMaximumValue(img_band) != iftMinimumValue(img_band)){
                    img_band_norm = iftNormalize(img_band,0,255);
                } else {
                    img_band_norm = iftCopyImage(img_band);
                }
                iftDestroyImage(&img_band);

                img_band_norm_resize = iftResizeImage(img_band_norm,resize_width,resize_height,1);
                iftDestroyImage(&img_band_norm);

                QString thumbnail = this->thumbnail_path + "/" + filename.baseName() + ".png";
                iftWriteImageByExt(img_band_norm_resize,thumbnail.toUtf8().data());
                iftDestroyImage(&img_band_norm_resize);

            }
            iftDestroyMImage(&mimg);
        } else {
            iftImage *img = iftReadImageByExt(filename.absoluteFilePath().toUtf8().data());
            if (iftIs3DImage(img)){

                iftImage *slice              = iftGetXYSlice(img,img->zsize/2);
                iftImage *slice_norm         = nullptr;
                iftImage *slice_norm_resize  = nullptr;

                if (iftMaximumValue(slice) != iftMinimumValue(slice)){
                    slice_norm = iftNormalize(slice,0,255);
                } else {
                    slice_norm = iftCopyImage(slice);
                }
                iftDestroyImage(&slice);

                slice_norm_resize = iftResizeImage(slice_norm,resize_width,resize_height,1);
                iftDestroyImage(&slice_norm);

                QString thumbnail = this->thumbnail_path + "/" + filename.baseName() + ".png";
                iftWriteImageByExt(slice_norm_resize,thumbnail.toUtf8().data());
                iftDestroyImage(&slice_norm_resize);

            } else { // Is 2D Image

                iftImage *img_norm = nullptr;
                iftImage *img_norm_resize = nullptr;
                if (iftMaximumValue(img) != iftMinimumValue(img)){
                    img_norm = iftNormalize(img,0,255);
                } else {
                    img_norm = iftCopyImage(img);
                }

                img_norm_resize = iftResizeImage(img_norm,resize_width,resize_height,1);
                iftDestroyImage(&img_norm);

                QString thumbnail = this->thumbnail_path + "/" + filename.baseName() + ".png";
                iftWriteImageByExt(img_norm_resize,thumbnail.toUtf8().data());
                iftDestroyImage(&img_norm_resize);

            }
            iftDestroyImage(&img);
        }
    }
}

void MainWindow::on_cbMarkersVisibility_currentTextChanged(QString s)
{
    Q_UNUSED(s);
    updateGraphicsView();
}

void MainWindow::on_cbActivationVisibility_currentTextChanged(QString s)
{
    Q_UNUSED(s);
    updateGraphicsView();
}

void MainWindow::on_cbGtVisibility_currentTextChanged(QString s)
{
    Q_UNUSED(s);
    updateGraphicsView();
}

void MainWindow::on_pbZoomIn_clicked()
{
    double scale_factor = 1.25;
    this->abs_scale_factor *= scale_factor;
    ui->gvDisplay->scale(scale_factor,scale_factor);
    updateCursor();
}

void MainWindow::on_pbZoomOut_clicked()
{
    double scale_factor = 0.8;
    this->abs_scale_factor *= scale_factor;
    ui->gvDisplay->scale(scale_factor,scale_factor);
    updateCursor();
}

void MainWindow::on_pbFitWindow_clicked()
{
    qDebug();
    qreal width_ratio = double(ui->gvDisplay->geometry().width()) / (this->current_qt_img->width()*this->abs_scale_factor);
    qreal height_ratio = double(ui->gvDisplay->geometry().height()) / (this->current_qt_img->height()*this->abs_scale_factor);

    qreal scale_factor;
    if (width_ratio <= height_ratio)
        scale_factor = width_ratio;
    else
        scale_factor = height_ratio;

    this->abs_scale_factor *= scale_factor;

    ui->gvDisplay->scale(scale_factor, scale_factor);
    updateCursor();
}

void MainWindow::on_pbOriginalSize_clicked()
{
    qreal scaleFactor = 1.0/this->abs_scale_factor;
    ui->gvDisplay->scale(scaleFactor,scaleFactor);
    this->abs_scale_factor = 1.0;
    updateCursor();
}

void MainWindow::on_hsBrightness_valueChanged(int value)
{
    if (value < 0)
        return;
    if (this->current_ift_img_norm == nullptr)
        return;

    updateGraphicsView();
}

void MainWindow::on_hsContrast_valueChanged(int value)
{
    if (value < 0)
        return;
    if (this->current_ift_img_norm == nullptr)
        return;

    updateGraphicsView();
}

void MainWindow::on_pbSaveMarkers_clicked()
{
    QFileDialog save_dialog;
    QString save_path;
    QMessageBox msgBox;
    QDir dir(this->markers_path);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
    if (list.count() == 0){
        msgBox.setText("No markers detected.");
        msgBox.exec();
        return;
    } else {
        /* Selecting save dir */
        save_dialog.setLabelText(QFileDialog::Accept, "Save");
        save_dialog.setOption(QFileDialog::ShowDirsOnly,true);
        save_dialog.setFileMode(QFileDialog::Directory);
        save_dialog.setDirectory(this->last_known_dir);

        if (!save_dialog.exec()){
            return;
        }
        save_path = save_dialog.directory().canonicalPath();

        if (!iftDirExists(save_path.toUtf8().data())){
            iftMakeDir(save_path.toUtf8().data());
        }
    }
    if (!iftDirExists(save_path.toUtf8().data())){
        msgBox.setText("Directory not found.");
        msgBox.exec();
        return;
    }
    this->last_known_dir = save_path+"/../";
    foreach(QFileInfo filename, list) {
        QString seeds_name;
        char *aux;
        aux = iftBasename(filename.fileName().toUtf8().data());
        seeds_name = save_path + "/" + aux + "-seeds.txt"; //this->images_path = this->seeds_path and -seeds.txt
        free(aux);
        iftImage *marker_img  = iftReadImageByExt((filename.absolutePath()+"/"+filename.fileName()).toUtf8().data());
        if (iftMaximumValue(marker_img) == 0){
            iftDestroyImage(&marker_img);
            continue;
        }
        iftLabeledSet *seeds = nullptr;
        for (int p = 0; p < marker_img->n; p++){
            if (marker_img->val[p] > 0){
                int label = marker_img->val[p]-1;
                iftInsertLabeledSet(&seeds,p,label);
            }
        }
        iftWriteSeeds(seeds,marker_img,seeds_name.toUtf8().data());
        iftDestroyLabeledSet(&seeds);
        iftDestroyImage(&marker_img);
    }

    msgBox.setText("Seeds saved successfully.");
    msgBox.exec();
}

void MainWindow::on_pbOpenMarkers_clicked()
{
    QMessageBox msgBox;
    QDir img_dir(this->images_path);
    QFileInfoList img_list = img_dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    if (img_list.count() == 0){
        msgBox.setText("No training images found. Please load the training images first.");
        msgBox.exec();
        return;
    }
    QString open_path;
    open_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                this->last_known_dir,
                                                QFileDialog::ShowDirsOnly);
    if (open_path == "")
        return ;
    if (!iftDirExists(open_path.toUtf8().data())){
        msgBox.setText("Directory not found.");
        msgBox.exec();
        return;
    }
    this->last_known_dir = open_path+"/../";

    if (iftDirExists(this->seeds_path.toUtf8().data()))
        iftRemoveDir(this->seeds_path.toUtf8().data());
    iftMakeDir(this->seeds_path.toUtf8().data());

    QDir dir(open_path);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Markers" << "*.txt",QDir::Files);
    foreach(QFileInfo markername, list) {
        foreach(QFileInfo imgname, img_list){
            if (imgname.fileName() == ""){
                continue;
            }
            if (markername.baseName().contains(imgname.baseName())){ //the marker and image names match
                iftImage *M = nullptr;
                iftLabeledSet *S = nullptr;
                QString ext;
                if (imgname.completeSuffix() == "mimg"){
                    iftMImage *mimg = iftReadMImage(imgname.absoluteFilePath().toUtf8().data());
                    S = iftMReadSeeds(mimg,markername.absoluteFilePath().toUtf8().data());
                    iftImage *aux = iftMImageToImage(mimg,255,0);
                    M = iftCreateImageFromImage(aux);
                    if (iftIs3DMImage(mimg)){
                        ext = ".nii.gz";
                    } else {
                        ext = ".png";
                    }
                    iftDestroyImage(&aux);
                    iftDestroyMImage(&mimg);
                } else {
                    iftImage *img = iftReadImageByExt(imgname.absoluteFilePath().toUtf8().data());
                    S = iftReadSeeds(img,markername.absoluteFilePath().toUtf8().data());
                    M = iftCreateImageFromImage(img);
                    if (iftIs3DImage(img)){
                        ext = ".nii.gz";
                    } else {
                        ext = ".png";
                    }
                    iftDestroyImage(&img);
                }
                while (S != nullptr){
                    int label = 0;
                    int p = iftRemoveLabeledSet(&S,&label);
                    iftVoxel u = iftGetVoxelCoord(M,p);
                    if (iftValidVoxel(M,u)) // checking if voxel is valid
                        M->val[p] = label+1;
                }
                if (iftIsColorImage(M)){
                    free(M->Cb);
                    free(M->Cr);
                    M->Cb = nullptr;
                    M->Cr = nullptr;
                }
                iftWriteImageByExt(M,QString(this->markers_path+"/"+imgname.baseName()+ext).toUtf8().data());
                iftDestroyImage(&M);
            }
        }
    }
    insertMissingMarkersInHashTable();
    updateGraphicsView();
    createListOfMakers();
}

void MainWindow::on_pbRemoveDrawnMarkers_clicked()
{
    QDir markers_dir(this->markers_path);
    if (!markers_dir.exists())
        return;
    QFileInfoList list = markers_dir.entryInfoList(QStringList() << "Markers" << "*",QDir::Files);
    bool found_markers = FALSE;
    foreach(QFileInfo file, list){
        if (!iftIsValidFormat(file.fileName().toUtf8().data()))
            continue;
        iftImage *img = iftReadImageByExt(file.absoluteFilePath().toUtf8().data());
        int max = iftMaximumValue(img);
        iftDestroyImage(&img);
        if (max > 0){
            found_markers = TRUE;
            break;
        }
    }

    if (!found_markers){
        QMessageBox msgBox;
        msgBox.setText("No markers found.");
        msgBox.exec();
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Deleting all drawn markers", "Are you sure you want to delete all markers?\nThis operation is irreversible.",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        iftRemoveDir(this->markers_path.toUtf8().data());
        iftMakeDir(this->markers_path.toUtf8().data());
        updateGraphicsView();
    }

    cleanListOfMarkers();
}

void MainWindow::on_pbLoadGroundTruth_clicked()
{
    /* Selecting GT dir */
    QString gt_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 this->last_known_dir,
                                                 QFileDialog::ShowDirsOnly);

    if (gt_path.isEmpty()){
        return;
    }

    this->last_known_dir = gt_path+"/../";
    this->gt_path = gt_path+"/";
    char ext[10];

    if (this->current_item != nullptr){
        if (this->current_gt != nullptr)
            iftDestroyImage(&this->current_gt);
        if (this->current_gt_border != nullptr)
            iftDestroyImage(&this->current_gt_border);

        if (iftIs3DImage(this->current_ift_img)){
            strcpy(ext,".nii.gz");
        } else {
            strcpy(ext,".png");
        }
        QString gt_filename = this->gt_path+QFileInfo(this->current_item->text()).baseName()+QString(ext);
        if (!iftFileExists(gt_filename.toUtf8().data()))
        {
            QMessageBox msgbox;
            msgbox.setText("Unable to find file "+gt_filename);
            msgbox.exec();
            return;
        }
        this->current_gt = iftReadImageByExt(gt_filename.toUtf8().data());
        this->current_gt_border = iftBorderImage(this->current_gt,false);
        updateGraphicsView();
        ui->cbGtVisibility->setEnabled(true);
    }

    QSet<int> layersToComputeDICE;
    for (int l = 1; l <= ui->cbLayer->count(); l++)
        layersToComputeDICE.insert(l);
    computeDICEonActivation(layersToComputeDICE);
    updateActivations();
}

void MainWindow::on_pbRemoveGroundTruth_clicked()
{
    if (this->gt_path == ""){
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Removing labels from FLIMbuilder", "Are you sure you want proceed?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        this->gt_path = "";
        iftDestroyImage(&this->current_gt);
        iftDestroyImage(&this->current_gt_border);
        iftDestroyMatrix(&this->DICEimportance);
        this->hashDICEscorePerImage.clear();
        updateGraphicsView();
    }
}

void MainWindow::computeDICEonActivation(QSet<int> layersToBeComputed)
{
    if ((ui->cbLayer->currentIndex() == -1) || (ui->cbLayer->currentText() == "")) // model was not trained yet
        return;

    if (this->gt_path == ""){
        return;
    }

    QMessageBox msgbox;

    iftMImage *activ = nullptr;
    iftImage *gt     = nullptr;

    for (int l = 1; l <= ui->cbLayer->count(); l++) {

        int layer_index = ui->cbLayer->itemText(l-1).replace("layer","").toInt();
        QString activation_path = this->model_path+"activation"+QString::number(layer_index); // getting output activations of layer X as the input of layer X+1

        if (!iftDirExists(activation_path.toUtf8().data())){
           msgbox.setText("Activation directory not found. Make sure the FLIM was executed.");
           msgbox.exec();
           return;
        }

        if ((!layersToBeComputed.contains(l)) && (!this->hashDICEscorePerImagePerLayer[l].empty())){
            continue;
        }

        // Checking number of classes
        iftFileSet *fs_activ = iftLoadFileSetFromDir(activation_path.toUtf8().data(),1);
        activ = iftReadMImage(fs_activ->files[0]->path);
        int n_images   = int(fs_activ->n);
        int n_channels = activ->m;
        iftDestroyMImage(&activ);

        //Creating matrix of kernel importance per class
        if (this->hashDICEscorePerImagePerLayer[l].empty() == false){
            this->hashDICEscorePerImagePerLayer[l].clear();
        }

        if (this->DICEimportance != nullptr){
            iftDestroyMatrix(&this->DICEimportance);
        }
        this->DICEimportance = iftCreateMatrix(1,n_channels);

        /* Creating Progress Dialog */
        QProgressDialog progress;
        progress.setLabelText(QString("Computing DICE for layer %1").arg(l));
        progress.setAutoClose(true);
        progress.setRange(0,100);
        progress.show();
        double step, cont;


        step = (100/n_images);
        cont = 0.0;

        activ = iftReadMImage(fs_activ->files[0]->path);
        float *mean  = iftAllocFloatArray(activ->m);
        float *stdev = iftAllocFloatArray(activ->m);
        iftDestroyMImage(&activ);

    //#pragma omp parallel for shared(cont) private(mean,stdev,activ,gt)
        for (int i = 0; i < n_images; i++)
        {
            char s[100];
            // Creating hash of iftMatrices
            sprintf(s,"%s",iftFilename(fs_activ->files[i]->path,nullptr));
            sprintf(s,"%s",iftBasename(s));
            QString basename(iftBasename(iftFilename(fs_activ->files[i]->path,nullptr)));
            hashDICEscorePerImagePerLayer[l].insert(basename,iftCreateMatrix(1,n_channels));

            // Reading activation maps for image i
            activ = iftReadMImage(fs_activ->files[i]->path);

            // Reading ground-truth image
            char ext[10];
            sprintf(ext,"%s",iftFileExt(ui->lwTrain->item(0)->text().toUtf8().data()));
            if (iftFileExists(QString(this->gt_path+"/"+basename+ext).toUtf8().data())){
                gt = iftReadImageByExt(QString(this->gt_path+"/"+basename+ext).toUtf8().data());
            } else {
                // Searching ground-truth image
                QDirIterator it(this->gt_path, QDirIterator::Subdirectories); // load gt path so the files can be searched without the extension
                while (it.hasNext()){
                    QString filename = it.next();
                    QFileInfo file(filename);
                    if (file.isDir()) { // Check if it's a dir
                        continue;
                    }
                    // Checking if file has the same filename as the activ
                    if (file.fileName().contains(iftFilename(fs_activ->files[i]->path,".mimg"), Qt::CaseInsensitive)) {
                        // Reading gt for image i
                        gt    = iftReadImageByExt(file.absoluteFilePath().toUtf8().data());
                        break;
                    }
                }
            }
            // If the GT is not found
            if (gt == nullptr){
                qDebug() << iftFilename(fs_activ->files[i]->path,".mimg") << " not found.";
                iftDestroyMatrix(&hashDICEscorePerImagePerLayer[l][basename]);
                iftDestroyMImage(&activ);
                continue;
            }

            iftImage *gt_th = iftThreshold(gt,1,iftMaximumValue(gt),1);

            for (ulong p = 0; p < activ->n; p++){
                for (ulong b=0; b < activ->m; b++){
                    mean[b] += activ->val[p][b];
                }
            }
            for (ulong b=0; b < activ->m; b++){
                mean[b] /= activ->n;
            }
            for (ulong p = 0; p < activ->n; p++){
                for (ulong b=0; b < activ->m; b++){
                    stdev[b] += pow(activ->val[p][b]-mean[b],2);
                }
            }
            for (ulong b=0; b < activ->m; b++){
                stdev[b] = sqrtf(stdev[b]/activ->n);
            }

            for (int c = 0; c < n_channels; c++){
                iftImage *mask = iftCreateImage(activ->xsize,activ->ysize,activ->zsize);
                for (ulong p = 0; p < activ->n; p++){
                    if (activ->val[p][c] >= mean[c]+3*stdev[c]){
                        mask->val[p] = 1;
                    }
                }

                int sx = gt->xsize/activ->xsize;
                int sy = gt->ysize/activ->ysize;
                int sz = 1;
                if (iftIs3DMImage(activ)){
                    sz = gt->zsize/activ->zsize;
                }
                iftImage *interp_mask = iftCreateImage(gt->xsize,gt->ysize,gt->zsize);
                iftVoxel u;
                for (u.z = 0; u.z < gt->zsize; u.z++){
                    for (u.y = 0; u.y < gt->ysize; u.y++){
                        for (u.x = 0; u.x < gt->xsize; u.x++){
                            iftVoxel v;
                            v.x = u.x / sx;
                            v.y = u.y / sy;
                            v.z = u.z / sz;
                            int p = iftGetVoxelIndex(interp_mask,u);
                            if (iftValidVoxel(mask,v)){
                                int q = iftGetVoxelIndex(mask,v);
                                interp_mask->val[p] = mask->val[q];
                            }
                        }
                    }
                }
                iftDestroyImage(&mask);
                // Saving DICE
                float dice = iftDiceSimilarity(gt_th,interp_mask);
                iftMatrixElem(hashDICEscorePerImagePerLayer[l][basename],0,c) = dice;
                iftMatrixElem(this->DICEimportance,0,c) += dice;
                iftDestroyImage(&interp_mask);
            }

            for (ulong c=0; c < activ->m; c++){
                mean[c] = stdev[c] = 0.0;
            }

            iftDestroyImage(&gt);
            iftDestroyImage(&gt_th);
            iftDestroyMImage(&activ);

            cont += step;
            progress.setValue(int(cont));
            QApplication::processEvents();
        }


        // mean DICE per channel
        for (int c = 0; c < n_channels; c++){
            iftMatrixElem(this->DICEimportance,0,c) /= n_images;
        }

        iftFree(mean);
        iftFree(stdev);
        iftDestroyFileSet(&fs_activ);
    }
}

void MainWindow::createTmpDir()
{
    QDir dir("/tmp/tmpdir_FLIM/");
    dir.removeRecursively();

    this->tmp_dir = "/tmp/tmpdir_FLIM";
    iftMakeDir(this->tmp_dir.toUtf8().data());
    // setting all permissions for tmp dir
    QFile(this->tmp_dir).setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner|
                                        QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ExeUser|
                                        QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ExeGroup|
                                        QFileDevice::ReadOther|QFileDevice::WriteOther|QFileDevice::ExeOther);

    /* Creating tmp training dir if does not exists */
    this->images_path = this->tmp_dir + "/train/";
    iftMakeDir(this->images_path.toUtf8().data());

    /* Creating tmp markers dir if does not exists */
    this->markers_path = this->tmp_dir + "/markers/";
    iftMakeDir(this->markers_path.toUtf8().data());

    /* Creating tmp seeds dir if does not exists */
    this->seeds_path = this->tmp_dir + "/seeds/";
    iftMakeDir(this->seeds_path.toUtf8().data());

    /* Creating tmp thumbnail dir if does not exists */
    this->thumbnail_path = this->tmp_dir + "/train_thumbnails/";
    iftMakeDir(this->thumbnail_path.toUtf8().data());

    /* Creating tmp model dir if does not exists */
    this->model_path = QDir::currentPath() + "/model/";
    iftMakeDir(this->model_path .toUtf8().data());
}

void MainWindow::cleanWorkspace()
{
    // Cleaning tmp dir
    createTmpDir();

    // Reseting widgets
    ui->lwTrain->clear();
    ui->lwActivation->clear();
    ui->cbLayer->blockSignals(true);
    ui->cbLayer->clear();
    ui->gvDisplay->blockSignals(true);
    resetImageVisualization();

    // Deallocating memory
    this->current_item = nullptr;
    if (this->current_ift_img != nullptr){
        iftDestroyImage(&this->current_ift_img);
    }
    if (this->current_ift_img_norm != nullptr){
        iftDestroyImage(&this->current_ift_img_norm);
    }
    if (this->current_ift_mimg != nullptr){
        iftDestroyMImage(&this->current_ift_mimg);
    }
    if (this->current_markers != nullptr){
        iftDestroyImage(&this->current_markers);
    }
    if (this->current_activation != nullptr){
        iftDestroyImage(&this->current_activation);
    }
    if (this->current_qt_img != nullptr){
        QImage qimg = QImage(*this->current_qt_img);
        qimg.fill(Qt::white);
        this->current_display_image->setPixmap(QPixmap::fromImage(qimg));
        delete this->current_qt_img;
        this->current_qt_img = nullptr;
    }
}

void MainWindow::cleanImageData()
{
    QDir dir(this->images_path);
    dir.removeRecursively();
    iftMakeDir(this->images_path.toUtf8().data());

    dir = QDir(this->markers_path);
    dir.removeRecursively();
    iftMakeDir(this->markers_path.toUtf8().data());

    dir = QDir(this->seeds_path);
    dir.removeRecursively();
    iftMakeDir(this->seeds_path.toUtf8().data());

    this->gt_path = "";

    ui->lwTrain->clear();
    ui->lwActivation->clear();
    ui->cbLayer->blockSignals(true);
    ui->cbLayer->clear();
    ui->gvDisplay->blockSignals(true);
    resetImageVisualization();

    cleanListOfMarkers();

    // Deallocating memory
    this->current_item = nullptr;
    if (this->current_ift_img != nullptr){
        iftDestroyImage(&this->current_ift_img);
    }
    if (this->current_ift_img_norm != nullptr){
        iftDestroyImage(&this->current_ift_img_norm);
    }
    if (this->current_ift_mimg != nullptr){
        iftDestroyMImage(&this->current_ift_mimg);
    }
    if (this->current_markers != nullptr){
        iftDestroyImage(&this->current_markers);
    }
    if (this->current_activation != nullptr){
        iftDestroyImage(&this->current_activation);
    }
    if (this->current_qt_img != nullptr){
        QImage qimg = QImage(*this->current_qt_img);
        qimg.fill(Qt::white);
        this->current_display_image->setPixmap(QPixmap::fromImage(qimg));
        delete this->current_qt_img;
        this->current_qt_img = nullptr;
    }
}

void MainWindow::enable3DImageVisualization()
{
    ui->pbFitWindow->setEnabled(true);
    ui->pbOriginalSize->setEnabled(true);
    ui->pbZoomOut->setEnabled(true);
    ui->pbZoomIn->setEnabled(true);
    ui->hsBrightness->setEnabled(true);
    ui->hsContrast->setEnabled(true);

    ui->cb3DPlane->setEnabled(true);
    ui->sbSliceNum->setEnabled(true);
    ui->sbBandNum->setEnabled(false);

    ui->cbMarkers->setEnabled(true);
    ui->pbAddMarker->setEnabled(true);
    ui->pbRemoveMarker->setEnabled(true);

    ui->cbMarkersVisibility->setEnabled(true);
    ui->cbActivationVisibility->setEnabled(true);
}

void MainWindow::enable2DImageVisualization()
{
    ui->pbFitWindow->setEnabled(true);
    ui->pbOriginalSize->setEnabled(true);
    ui->pbZoomOut->setEnabled(true);
    ui->pbZoomIn->setEnabled(true);
    ui->hsBrightness->setEnabled(true);
    ui->hsContrast->setEnabled(true);

    ui->cb3DPlane->setEnabled(false);
    ui->sbSliceNum->setValue(0);
    ui->sbSliceNum->setEnabled(false);
    ui->sbBandNum->setValue(0);
    ui->sbBandNum->setEnabled(false);

    ui->cbMarkers->setEnabled(true);
    ui->pbAddMarker->setEnabled(true);
    ui->pbRemoveMarker->setEnabled(true);

    ui->cbMarkersVisibility->setEnabled(true);
    ui->cbActivationVisibility->setEnabled(true);
}

void MainWindow::enable3DMImageVisualization()
{
    ui->pbFitWindow->setEnabled(true);
    ui->pbOriginalSize->setEnabled(true);
    ui->pbZoomOut->setEnabled(true);
    ui->pbZoomIn->setEnabled(true);
    ui->hsBrightness->setEnabled(true);
    ui->hsContrast->setEnabled(true);

    ui->cb3DPlane->setEnabled(true);
    ui->sbSliceNum->setEnabled(true);
    ui->sbBandNum->setEnabled(true);

    ui->cbMarkers->setEnabled(true);
    ui->pbAddMarker->setEnabled(true);
    ui->pbRemoveMarker->setEnabled(true);

    ui->cbMarkersVisibility->setEnabled(true);
    ui->cbActivationVisibility->setEnabled(true);
}

void MainWindow::enable2DMImageVisualization()
{
    ui->pbFitWindow->setEnabled(true);
    ui->pbOriginalSize->setEnabled(true);
    ui->pbZoomOut->setEnabled(true);
    ui->pbZoomIn->setEnabled(true);
    ui->hsBrightness->setEnabled(true);
    ui->hsContrast->setEnabled(true);

    ui->cb3DPlane->setEnabled(false);
    ui->sbSliceNum->setValue(0);
    ui->sbSliceNum->setEnabled(false);
    ui->sbBandNum->setValue(0);
    ui->sbBandNum->setEnabled(true);

    ui->cbMarkers->setEnabled(true);
    ui->pbAddMarker->setEnabled(true);
    ui->pbRemoveMarker->setEnabled(true);

    ui->cbMarkersVisibility->setEnabled(true);
    ui->cbActivationVisibility->setEnabled(true);
}

void MainWindow::resetImageVisualization()
{
    ui->pbFitWindow->setEnabled(false);
    ui->pbOriginalSize->setEnabled(false);
    ui->pbZoomOut->setEnabled(false);
    ui->pbZoomIn->setEnabled(false);
    ui->hsBrightness->blockSignals(true);
    ui->hsBrightness->setValue(50);
    ui->hsBrightness->setEnabled(false);
    ui->hsBrightness->blockSignals(false);
    ui->hsContrast->blockSignals(true);
    ui->hsContrast->setValue(0);
    ui->hsContrast->setEnabled(false);
    ui->hsContrast->blockSignals(false);

    ui->cb3DPlane->setEnabled(false);
    ui->cb3DPlane->blockSignals(true);
    ui->cb3DPlane->setCurrentIndex(0);
    ui->cb3DPlane->blockSignals(false);

    ui->sbSliceNum->blockSignals(true);
    ui->sbSliceNum->setValue(0);
    ui->sbSliceNum->setEnabled(false);
    ui->sbSliceNum->blockSignals(false);

    ui->sbBandNum->blockSignals(true);
    ui->sbBandNum->setValue(0);
    ui->sbBandNum->setEnabled(false);
    ui->sbBandNum->blockSignals(true);

    ui->cbMarkers->setEnabled(false);
    ui->pbAddMarker->setEnabled(false);
    ui->pbRemoveMarker->setEnabled(false);

    ui->cbLayer->blockSignals(true);
    ui->cbLayer->clear();
    ui->cbLayer->setEnabled(false);
    ui->cbLayer->setCurrentIndex(-1);
    ui->cbLayer->setCurrentText("");
    ui->cbLayer->blockSignals(false);
    ui->pbSelectKernelsManual->setEnabled(false);
    ui->pbProjectKernels->setEnabled(false);
    ui->pbInvertKernels->setEnabled(false);

    ui->cbMarkersVisibility->setEnabled(false);
    ui->cbActivationVisibility->setEnabled(false);
    ui->cbGtVisibility->setEnabled(false);

    ui->lblSize->setText("Size");
    ui->lblMinMax->setText("Min./Max.");
    ui->lblBPP->setText("BPP");
}

void MainWindow::enableActivationVisualization()
{
    // Create combobox items and set first item
    // -- Clean combobox items
    ui->cbLayer->blockSignals(true);
    ui->cbLayer->clear();
    // -- Write combobox items
    QDir dir(this->model_path);
    QFileInfoList layers = dir.entryInfoList(QStringList() << "Layers" << "layer*",QDir::Dirs);
    foreach(QFileInfo layer_path, layers) {
        ui->cbLayer->addItem(layer_path.baseName());
    }
    // -- Enables combobox
    ui->cbLayer->setEnabled(true);
    // -- Set item to first
    ui->cbLayer->setCurrentIndex(0);
    ui->cbLayer->blockSignals(false);
    // -- Enables manual kernel selection
    ui->pbSelectKernelsManual->setEnabled(true);
    ui->pbProjectKernels->setEnabled(true);
    ui->pbInvertKernels->setEnabled(true);
}

void MainWindow::disableActivationVisualization()
{
    // Create combobox items and set first item
    // -- Clean combobox items
    ui->cbLayer->blockSignals(true);
    ui->cbLayer->clear();
    ui->lwActivation->clear();
    // -- Disable combobox
    ui->cbLayer->setEnabled(false);
    // -- Set item to first
    ui->cbLayer->blockSignals(false);
    // -- Enables manual kernel selection
    ui->pbSelectKernelsManual->setEnabled(false);
    ui->pbProjectKernels->setEnabled(false);
    ui->pbInvertKernels->setEnabled(false);
}

void MainWindow::createWidgetsConnections()
{
    connect(ui->lwTrain, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_lwTrain_itemDoubleClicked(QListWidgetItem*)), Qt::UniqueConnection);

    connect(ui->pbOpenTrainFolder, SIGNAL(clicked()), this, SLOT(on_pbOpenTrainFolder_clicked()), Qt::UniqueConnection);
    connect(ui->pbSaveMarkers, SIGNAL(clicked()), this, SLOT(on_pbSaveMarkers_clicked()), Qt::UniqueConnection);
    connect(ui->pbOpenMarkers, SIGNAL(clicked()), this, SLOT(on_pbOpenMarkers_clicked()), Qt::UniqueConnection);
    connect(ui->pbRemoveDrawnMarkers, SIGNAL(clicked()), this, SLOT(on_pbRemoveDrawnMarkers_clicked()), Qt::UniqueConnection);
    connect(ui->pbLoadGroundTruth, SIGNAL(clicked()), this, SLOT(on_pbLoadGroundTruth_clicked()), Qt::UniqueConnection);
    connect(ui->pbRemoveGroundTruth, SIGNAL(clicked()), this, SLOT(on_pbRemoveGroundTruth_clicked()), Qt::UniqueConnection);
    connect(ui->lwListOfMarkers, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_lwListOfMarkers_itemDoubleClicked(QListWidgetItem*)), Qt::UniqueConnection);

    connect(ui->pbZoomIn, SIGNAL(clicked()), this, SLOT(on_pbZoomIn_clicked()), Qt::UniqueConnection);
    connect(ui->pbZoomOut, SIGNAL(clicked()), this, SLOT(on_pbZoomOut_clicked()), Qt::UniqueConnection);
    connect(ui->pbFitWindow, SIGNAL(clicked()), this, SLOT(on_pbFitWindow_clicked()), Qt::UniqueConnection);
    connect(ui->pbOriginalSize, SIGNAL(clicked()), this, SLOT(on_pbOriginalSize_clicked()), Qt::UniqueConnection);
    connect(ui->hsBrightness, SIGNAL(valueChanged(int)), this, SLOT(on_hsBrightness_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->hsContrast, SIGNAL(valueChanged(int)), this, SLOT(on_hsContrast_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->cbMarkersVisibility, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbMarkersVisibility_currentTextChanged(QString)), Qt::UniqueConnection);
    connect(ui->cbActivationVisibility, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbActivationVisibility_currentTextChanged(QString)), Qt::UniqueConnection);
    connect(ui->cbGtVisibility, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbGtVisibility_currentTextChanged(QString)), Qt::UniqueConnection);
    connect(ui->sbSliceNum, SIGNAL(valueChanged(int)), this, SLOT(on_sbSliceNum_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->cb3DPlane, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cb3DPlane_currentIndexChanged(int)), Qt::UniqueConnection);
    connect(ui->sbBandNum, SIGNAL(valueChanged(int)), this, SLOT(on_sbBandNum_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->pbAddMarker, SIGNAL(clicked()), this, SLOT(on_pbAddMarker_clicked()), Qt::UniqueConnection);
    connect(ui->pbRemoveMarker, SIGNAL(clicked()), this, SLOT(on_pbRemoveMarker_clicked()), Qt::UniqueConnection);
    connect(ui->cbMarkers, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbMarkers_currentTextChanged(QString)), Qt::UniqueConnection);

    connect(ui->gvDisplay, SIGNAL(printMarker(int,int)), this, SLOT(on_gvDisplay_printMarker(int,int)), Qt::UniqueConnection);
    connect(ui->gvDisplay, SIGNAL(eraseMarker(int,int)), this, SLOT(on_gvDisplay_eraseMarker(int,int)), Qt::UniqueConnection);
    connect(ui->gvDisplay, SIGNAL(released()), this, SLOT(on_gvDisplay_released()), Qt::UniqueConnection);
    connect(ui->gvDisplay, SIGNAL(forwardSlice(int)), this, SLOT(on_gvDisplay_forwardSlice(int)), Qt::UniqueConnection);
    connect(ui->gvDisplay, SIGNAL(backwardSlice(int)), this, SLOT(on_gvDisplay_backwardSlice(int)), Qt::UniqueConnection);
    connect(ui->gvDisplay, SIGNAL(showIntensity(int,int)), this, SLOT(on_gvDisplay_showIntensity(int,int)), Qt::UniqueConnection);
    connect(ui->sbBrush, SIGNAL(valueChanged(int)), this, SLOT(on_sbBrush_valueChanged(int)), Qt::UniqueConnection);
    ui->gvDisplay->blockSignals(true); // signals are restored when images are loaded

    connect(ui->pbLoadArch, SIGNAL(clicked()), this, SLOT(on_pbLoadArch_clicked()), Qt::UniqueConnection);
    connect(ui->pbSaveArch, SIGNAL(clicked()), this, SLOT(on_pbSaveArch_clicked()), Qt::UniqueConnection);
    connect(ui->teFLIM_Arch, SIGNAL(focusChanged()), this, SLOT(on_teFLIM_Arch_focusChanged()), Qt::UniqueConnection);

    connect(ui->pbOpenModelDir, SIGNAL(clicked()), this, SLOT(on_pbOpenModelDir_clicked()), Qt::UniqueConnection);
    connect(ui->cbSelectTrainingLayers, SIGNAL(itemClicked(int)), this, SLOT(on_cbSelectTrainingLayers_itemClicked(int)), Qt::UniqueConnection);
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(on_pbRun_clicked()), Qt::UniqueConnection);

    connect(ui->cbLayer, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbLayer_currentIndexChanged(int)), Qt::UniqueConnection);
    connect(ui->lwActivation, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_lwActivation_itemDoubleClicked(QListWidgetItem*)), Qt::UniqueConnection);
    connect(ui->lwActivation, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(on_lwActivation_itemChanged(QListWidgetItem*)), Qt::UniqueConnection);
    connect(ui->pbSelectKernelsManual, SIGNAL(clicked()), this, SLOT(on_pbSelectKernelsManual_clicked()), Qt::UniqueConnection);
    connect(ui->pbProjectKernels, SIGNAL(clicked()), this, SLOT(on_pbProjectKernels_clicked()), Qt::UniqueConnection);
    connect(ui->pbInvertKernels, SIGNAL(clicked()), this, SLOT(on_pbInvertKernels_clicked()), Qt::UniqueConnection);
}


