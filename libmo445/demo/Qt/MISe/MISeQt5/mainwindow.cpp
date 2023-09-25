#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <segmentation/segmentation.h>
#include <segmentation/dynamicift/dynamicift.h>
#include <views/slice/sliceview.h>
#include <segmentation/manual/manualsegmentation.h>
#include <segmentation/gradient/magnitude/magnitudegradient.h>
#include <segmentation/gradient/flim/flimfeatures.h>
#include <aboutdialog.h>
#include <segmentation/gradient/arcweightfunction.h>
#include <segmentation/gradient/manual/manualarcweight.h>
#include <segmentation/gradient/saliency/saliency.h>
#include <postprocessing/postprocessing.h>
#include <segmentation/livewire/livewire.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    this->showMaximized();    

    /* Setting Icons for the buttons */
    setIcons();

    // Creating pen for the lines to be drawn. The width is set to 0 so the lines wont scale with the image.
    pen = new QPen();
    pen->setColor(Qt::green);
    pen->setStyle(Qt::SolidLine);
    pen->setWidth(0);

    // Fitting the columns to the size of its contents
    ui->twObjects->resizeColumnsToContents();

    // Initializing SliceViews
    ui->sliceAxial->initializeSlice(AXIAL, ui->sliceCoronal, ui->sliceSagittal);
    ui->sliceSagittal->initializeSlice(SAGITTAL, ui->sliceAxial, ui->sliceCoronal);
    ui->sliceCoronal->initializeSlice(CORONAL, ui->sliceAxial, ui->sliceSagittal);

    // Instantiating classes
    view = View::instance();
    vol_info = new VolumeInformation();
    ms = nullptr;

    // Creating Actions
    createHotkeys();

    // Load arc-weight functions
    ArcWeightFunction *magnitude = new MagnitudeGradient(this);
    ArcWeightFunction *flimFeat  = new FLIMFeatures(this);
    ArcWeightFunction *manualAW  = new ManualArcWeight(this);
    ArcWeightFunction *saliency  = new Saliency(this);
    currentArcWeightFunction = magnitude;
    ui->cbGradient->addItem(magnitude->name(),  QVariant::fromValue(magnitude));
    ui->cbGradient->addItem(flimFeat->name(),   QVariant::fromValue(flimFeat));
    ui->cbGradient->addItem(manualAW->name(),   QVariant::fromValue(manualAW));
    ui->cbGradient->addItem(saliency->name(),   QVariant::fromValue(saliency));
    // TODO move to createConnections
    connect(magnitude, SIGNAL(update()), this, SLOT(slotResetGradient()));
    connect(manualAW, SIGNAL(update()), this, SLOT(slotResetGradient()));
    connect(saliency, SIGNAL(update()), this, SLOT(slotResetGradient()));
    connect(flimFeat, SIGNAL(update()), this, SLOT(slotResetGradient()));


    // Load segmentation methods
    SemiAutomatic *interactive      = new SemiAutomatic(this, view);
    ALTIS *altis                    = new ALTIS(this, view, interactive);
    //ALTISFLIM *altisFlim            = new ALTISFLIM(this, view, interactive);
    //ALTISFLIMSupervoxel *altisFlimS = new ALTISFLIMSupervoxel(this, view, interactive);
    Segmentation *manual            = new ManualSegmentation(this, view);
    Threshold *threshold            = new Threshold(this, view);
    DynamicIFT *dynIftRoot          = new DynamicIFT(this, view);
    LiveWire *livewire              = new LiveWire(this, view);

    segmentationViews.append(interactive);
    segmentationViews.append(altis);
    //segmentationViews.append(altisFlim);
    //segmentationViews.append(altisFlimS);
    segmentationViews.append(manual);
    segmentationViews.append(dynIftRoot);
    segmentationViews.append(threshold);
    segmentationViews.append(livewire);

    //altisFlimS->loadGraphicalComponents();
    //altisFlim->loadGraphicalComponents();

    // The following method is only available in the QT 5.15 release
    //ui->cbSegmentation->setPlaceholderText(QString("--Select the method--"));
    ui->cbSegmentation->addItem("-- Select the method --");

    ui->cbSegmentation->addItem(altis->name(),       QVariant::fromValue(altis));
    //ui->cbSegmentation->addItem(altisFlim->name(),   QVariant::fromValue(altisFlim));
    //ui->cbSegmentation->addItem(altisFlimS->name(),  QVariant::fromValue(altisFlimS));
    ui->cbSegmentation->addItem(threshold->name(),   QVariant::fromValue(threshold));
    ui->cbSegmentation->addItem(interactive->name(), QVariant::fromValue(interactive));
    ui->cbSegmentation->addItem(dynIftRoot->name(),  QVariant::fromValue(dynIftRoot));
    ui->cbSegmentation->addItem(manual->name(),      QVariant::fromValue(manual));
    ui->cbSegmentation->addItem(livewire->name(),    QVariant::fromValue(livewire));

    // Creating the Menu buttons and shortcuts
    setActions();

    setSystemMode("INITIALIZATION");

    setAnnotationVisibility(false);

    QHeaderView *headerView = ui->twMarkers->horizontalHeader();
    headerView->setSectionResizeMode(2, QHeaderView::Stretch);
    headerView = ui->twObjects->horizontalHeader();
    headerView->setSectionResizeMode(4, QHeaderView::Stretch);

    objectsCSV = nullptr;

    set2DVisualizationMode();
}

MainWindow::~MainWindow()
{
    delete view;
    delete ui;
    delete axial;
    delete coronal;
    delete sagittal;
    delete openFileAct;
    delete importLabelAct;
    delete exportLabelAct;
    delete importMarkersAct;
    delete exportMarkersAct;
    delete exportImageSequenceAct;
    delete exitAct;
    delete zoomInAct;
    delete zoomOutAct;
    delete normalSizeAct;
    delete aboutAct;
    delete lineVisibilityAct;
    delete processingAct;
    delete fileMenu;
    delete importMenu;
    delete exportMenu;
    delete viewMenu;
    delete processingMenu;
    delete helpMenu;
    delete pen;
    if (postProcessingWindow)
        delete postProcessingWindow;
    if (vol_info)
        delete vol_info;
    for (int i = 0; i < 6; i++)
        delete L[i];
    for (int i = 0; i < 4; i++)
        delete P[i];
    for (int i = 0; i < 12; i++)
        delete T[i];

    for (Segmentation* segm: segmentationViews) {
        delete segm;
    }

    iftDestroyCSV(&objectsCSV);
}

void MainWindow::loadImageFromCommandLine(QStringList imgs)
{
    QSettings settings;
    settings.setValue(DEFAULT_IMAGE_DIR_KEY,
                      QFileInfo(imgs.at(0)).absolutePath());

    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Loading image", 0);

    iftImageSequence *imgseq = nullptr;
    int b,c;
    double bf,cf;

    setSystemMode("OPEN_FILE");

    if (ms != nullptr){
        delete ms;
        ms = nullptr;
    }

    if (!view->isImageEmpty()){
        destroyActions();
        destroyMarkersTableActions();
        ui->twMarkers->clearContents();
        ui->twMarkers->setRowCount(0);
    }

    if (!view->isLabelEmpty()){
        view->destroyLabel();
        destroyObjectTableActions();
        ui->twObjects->clearContents();
        ui->twObjects->setRowCount(0);
    }

    imgs.sort();
    for (QString img_path: imgs) {
        if (QFileInfo(img_path).suffix() == "imsq") {
            imgseq = iftReadImageSequence(img_path.toUtf8().constData());
        } else {
            iftImage *img = iftReadImageByExt(img_path.toUtf8().constData());
            if (imgseq == nullptr)
                imgseq = iftVolumeToImageSequence(img);
            else if (iftIsImageCompatibleToImageSequence(imgseq, img)) {
                iftAppendVolumeToImageSequence(imgseq, img);
            } else {
                QMessageBox::warning((QWidget*) parent(),
                                     tr("Warning"),
                                     "Could not load the image " + img_path + ". The image dimensions do not match with"
                                        " the image " + imgs.at(0));
                imgs.removeOne(img_path);
            }
            iftDestroyImage(&img);
        }
    }
    char * filename = iftFilename(imgs.at(0).toUtf8().constData(),nullptr);
    QString title = "MISe - " + QString::fromLocal8Bit(filename);
    setWindowTitle(title);

    view->setImageSequence(imgseq, filename);
    iftFree(filename);

    view->annotation->addItemInColorTable(0);
    addBackgroundInListOfMarkers();

    slotAddNewMarker();

    view->createRendition();

    b = (100-ui->hsBrightness->value());
    c = (100-ui->hsContrast->value());

    bf = b*view->maxNormalizedValue/100.0;
    cf = c*view->maxNormalizedValue/100.0;

    double f1, f2, g1, g2;

    f1 = (2*bf - cf)/2;
    f2 = cf + f1;
    g1 = 0;
    g2 = 255;

    ui->widgetRendering->updateBrightnessAndConstrast(f1, f2, g1, g2);
    SliceView::updateBrightnessAndConstrast(f1, f2, g1, g2); //TODO remove it

    createActions();
    createMarkersTableActions();
    slotChangeGradientMethod();


    initializeAllGraphicalViews();
    updateAllGraphicalViews();

    for (Segmentation *segm: segmentationViews) {
        // TODO maybe replace with emit
        segm->notifyImageUpdate();
    }

    if (iftIs3DImageSequence(view->getImageSequence())) {
        set3DVisualizationMode();
    } else {
        set2DVisualizationMode();
    }

    if (iftIs4DImageSequence(imgseq)) {
        setMultiTemporalVisualizationMode();
    } else {
        setUniTemporalVisualizationMode();
    }

    iftWriteOnLog("Loading image took", elapsedTime.elapsed());

    return;
}

void MainWindow::loadLabelFromCommandLine(QString label_path)
{
    QSettings settings;
    settings.setValue(DEFAULT_LABEL_DIR_KEY,
                      QFileInfo(label_path).absolutePath());

    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Loading label image from command line", 0);

    iftImage *label = nullptr;

    if (view->isImageEmpty()){
        QMessageBox::critical(this,"Error","Error while importing the label map. A CT image must be loaded first.",QMessageBox::Close);
        return;
    }

    label = iftReadImageByExt(label_path.toUtf8().constData());

    if ((label->xsize != view->getXsize()) || (label->ysize != view->getYsize()) ||
            (label->zsize != view->getZsize())){
        QMessageBox::critical(this,"Error","The label map and image must have the same domain.",QMessageBox::Close);
        return;
    }

    setSystemMode("OPEN_LABEL");

    if (!view->isLabelEmpty()){
        destroyObjectTableActions();
        view->destroyLabel();
    }

    view->setLabel(label);
    iftDestroyImage(&label);

    view->setRenditionLabel();

    updateAllGraphicalViews();
    loadListOfObjects();

    iftWriteOnLog("Loading label image from command line took", elapsedTime.elapsed());

    return;

}

void MainWindow::slotOpenFile()
{
    QSettings settings;

    QFileDialog dialog(this);
    QStringList filenames, filters;

    dialog.setViewMode(QFileDialog::List);
    dialog.setDirectory(settings.value(DEFAULT_IMAGE_DIR_KEY).toString());

    filters << "Multidimensional Image files (*.imsq *.scn *.nii *.nii.gz *.hdr *.png *.pgm *.jpg *.jpeg)"
            << "Image Sequence (*.imsq)"
            << "Scene (*.scn)"
            << "NIfTI (*.nii)"
            << "Compressed NIfTI (*.nii.gz)"
            << "Analyze (*.hdr)"
            << "JPEG (*.jpg *.jpeg)"
            << "Portable Network Graphics (*.png)"
            << "Portable Graymap Format (*.pgm)";
    dialog.setNameFilters(filters);
    dialog.setFileMode(QFileDialog::ExistingFiles);


    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (filenames.isEmpty()){
        return;
    }

    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Loading image from graphical interface", 0);

    loadImageFromCommandLine(filenames);

    iftWriteOnLog("Loading image from graphical interface took", elapsedTime.elapsed());

    return;
}

void MainWindow::slotImportLabel()
{
    QSettings settings;
    if (view->isImageEmpty()){
        QMessageBox::critical(this,"Error","Error while importing the label map. A CT image must be loaded first.",QMessageBox::Close);
        return;
    }

    QFileDialog dialog(this);
    QStringList filenames, filters;
    dialog.setDirectory(settings.value(DEFAULT_LABEL_DIR_KEY).toString());

    dialog.setViewMode(QFileDialog::List);

    filters << "Multidimensional Image files (*.scn *.nii *.nii.gz *.hdr *.pgm *.png)"
            << "Scene (*.scn)"
            << "NIfTI (*.nii)"
            << "Compressed NIfTI (*.nii.gz)"
            << "Analyze (*.hdr)"
            << "Portable Network Graphics (*.png)"
            << "Portable Graymap Format (*.pgm)";
    dialog.setNameFilters(filters);

    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (filenames.isEmpty()){        
        return;
    }

    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Loading label image from graphical interface", 0);

    loadLabelFromCommandLine(filenames.at(0));

    iftWriteOnLog("Loading label image from graphical interface took", elapsedTime.elapsed());

    return;
}

void MainWindow::slotExportLabel()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Exporting label", 0);

    if (view->getLabel() == nullptr){
           QMessageBox::critical(this,"Error","Error while exporting the label. There must exist a label first.",QMessageBox::Close);
           return;
    }

    QString filter = QString("Multidimensional Image files (*.scn *.nii *.nii.gz *.hdr *.png *.pgm);;") +
                     "Scene (*.scn);;NIfTI (*.nii);;Compressed NIfTI (*.nii.gz);;" +
                     "Analyze (*.hdr);;Portable Graymap Format (*.pgm);;Portable Network Graphics (*.png)";

    char *_basename = iftBasename(getFileName().toUtf8().data());
    QString export_filename_template = iftIs3DImage(view->getImage()) ? QString(_basename)+".nii.gz" : QString(_basename)+".png";
    iftFree(_basename);

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Export Label"), export_filename_template,
            filter);

    if (fileName.isEmpty())
            return;

    iftWriteImageByExt(view->getLabel(), fileName.toUtf8());

    iftWriteOnLog("Exporting label took", elapsedTime.elapsed());
}

void MainWindow::loadListOfObjects(bool useMarkerInfo)
{
    iftDestroyCSV(&objectsCSV);

    ui->twObjects->clearContents();
    ui->pbExportObjects->setEnabled(!view->isLabelEmpty());
    if (view->isLabelEmpty()){
        ui->twObjects->setRowCount(0);
        return;
    }

    const iftImage *img = view->getLabel();
    QString spelsHeaderName = iftIs3DImage(img)? "Vol. (units³)" : "Area (units²)";
    ui->twObjects->horizontalHeaderItem(4)->setText(spelsHeaderName);

    QTableWidgetItem *newItem = nullptr;
    QColor qc;
    iftColorTable *t = view->getColorTable();

    ui->twObjects->setRowCount(t->ncolors);

    objectsCSV = iftCreateCSV(t->ncolors, 2);

    for (int i = 0; i < t->ncolors; i++){        
        QToolButton *tb = new QToolButton();
        tb->setIcon(QIcon(":/Images/icons/visibility.svg"));
        tb->setCheckable(true);
        tb->setChecked(false);
        connect(tb, SIGNAL(clicked()), this, SLOT(slotChangeObjectVisibility()));
        ui->twObjects->setCellWidget(i,0, tb);

        newItem = new QTableWidgetItem();
        // make the cell not editable
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        ui->twObjects->setItem(i,1,newItem);
        qc = iftColorToQColor(t->color[i]);
        ui->twObjects->item(i,1)->setBackground(qc);


        QString name;
        if (useMarkerInfo) {
            int index = view->annotation->getIndexFromLabel(i+1);
            if (index >= 0) {
                name = view->annotation->getMarkerInfoArray()[index].name();
            }
        } else {
            name = "obj"+QString::number(i+1);
        }

        newItem = new QTableWidgetItem(name);
        newItem->setTextAlignment(Qt::AlignCenter);
        ui->twObjects->setItem(i,2,newItem);

        newItem = new QTableWidgetItem("1.0");
        newItem->setTextAlignment(Qt::AlignCenter);
        ui->twObjects->setItem(i,3,newItem);

        float voxel_size = iftIs3DImage(img) ? img->dx * img->dy * img->dz : img->dx * img->dy;
        int spels = iftCountObjectSpels(img, i+1);
        float volume = spels*voxel_size;
        QString volume_formatted;

        if (volume > 1000000) {
            volume_formatted = QString::number(volume*0.000001, 'f', 3) + "M";
        } else if (volume > 1000) {
            volume_formatted = QString::number(volume*0.001, 'f', 3) + "K";
        } else {
            volume_formatted = QString::number(volume);
        }
        newItem = new QTableWidgetItem(volume_formatted);
        newItem->setToolTip(QString::number((int) volume)+ " ("+QString::number(spels*100.0/img->n)+"%)");
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignCenter);
        ui->twObjects->setItem(i,4,newItem);

        tb = new QToolButton();
        tb->setIcon(QIcon(":/Images/icons/remove.svg"));
        connect(tb, SIGNAL(clicked()), this, SLOT(slotTableDeleteObjectClicked()));
        ui->twObjects->setCellWidget(i,5, tb);

        strcpy(objectsCSV->data[i][0], name.toUtf8().data());
        sprintf(objectsCSV->data[i][1], "%f", volume);
    }

    if (t->ncolors > 0){
        ui->cbMarkAll->setEnabled(true);
        createObjectTableActions();
    } else {
        ui->cbMarkAll->setEnabled(false);
    }

    iftDestroyColorTable(&t);
}

void MainWindow::slotImportMarkers()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Importing markers", 0);

    if (view->isImageEmpty()){
           QMessageBox::critical(this,"Error","Error while importing markers. A CT image must be loaded first.",QMessageBox::Close);
           return;
    }
    QSettings settings;
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Import Markers"),
            settings.value(DEFAULT_MARKERS_DIR_KEY).toString(),
            tr("TXT file(*.txt);;"));

    if (fileName.isEmpty())
            return;

    settings.setValue(DEFAULT_MARKERS_DIR_KEY,
                       QFileInfo(fileName).absolutePath());

    const iftImage *markers = view->annotation->getMarkers();
    const char *path = fileName.toUtf8().data();
    iftLabeledSet *S = iftReadSeeds(markers,path);

    int max = 0;
    while (S != nullptr){
        int label;
        int p = iftRemoveLabeledSet(&S,&label);
        iftVoxel u = iftGetVoxelCoord(markers,p);
        if (!iftValidVoxel(markers,u))
            continue;
        markers->val[p] = label;
        if (label > max)
            max = label;
    }


    for (int i = 0; i <= max; i++){
        view->annotation->addItemInColorTable();
    }

    view->annotation->setMarkers(markers);
    //iftDestroyImage(&markers);

    loadListOfMarkers();
    updateAllGraphicalViews();

    iftWriteOnLog("Importing markers took", elapsedTime.elapsed());
}

void MainWindow::slotExportMarkers()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Exporting markers", 0);

    if (view->isImageEmpty()){
           QMessageBox::critical(this,"Error","Error while exporting markers. A CT image must be loaded first.",QMessageBox::Close);
           return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Export Markers"), "",
            tr("TXT file(*.txt);;"));

    if (fileName.isEmpty())
            return;

    iftLabeledSet *S = nullptr;
    const iftImage *markers = view->annotation->getMarkers();
    const char *out_path = nullptr;

    for (int p = 0; p < markers->n; p++)
        if (markers->val[p] >= 0){ // TODO remind this
            int label = markers->val[p];
            iftInsertLabeledSet(&S,p,label);
        }

    out_path = fileName.toUtf8().data();
    if (strcmp(out_path,"") == 0){
        messageBox.critical(0,"Error","An error has occured while exporting markers! The output path is empty, please try again.");
        messageBox.setFixedSize(500,200);
        return;
    }
    iftWriteSeeds(S,markers,out_path);
    iftDestroyLabeledSet(&S);
    //iftDestroyImage(&markers);

    iftWriteOnLog("Exporting markers took", elapsedTime.elapsed());
}

void MainWindow::slotExportObjectsCSV()
{
    if (objectsCSV != nullptr) {
        QString fileName = QFileDialog::getSaveFileName(this,
                tr("Export Label"), "objects.csv",
                tr("comma-separated values file(*.csv);;"));

        FILE *fp = fopen(fileName.toUtf8().data(), "w");
        if (fp == NULL) {
            QMessageBox::warning((QWidget*) parent(),
                                 tr("Warning"),
                                 "Could not export CSV file to desired path.");
        } else {
            fclose(fp);
            iftWriteCSV(objectsCSV, fileName.toUtf8().data(), ',');
        }
    }
}

void MainWindow::slotExportImageSequence()
{
    const iftImageSequence *imgseq = view->getImageSequence();
    if (imgseq != nullptr) {
        QString fileName = QFileDialog::getSaveFileName(this,
                tr("Export Label"), "image.imsq",
                tr("Image Sequence file(*.imsq);;"));

        FILE *fp = fopen(fileName.toUtf8().data(), "wb");
        if (fp == NULL) {
            QMessageBox::warning((QWidget*) parent(),
                                 tr("Warning"),
                                 "Could not export Image Sequence to desired path.");
        } else {
            fclose(fp);
            iftWriteImageSequence(imgseq, fileName.toUtf8().data());
        }
    }
}

void MainWindow::slotOpenProcessingOptions()
{
    postProcessingWindow = PostProcessing::instance();
    postProcessingWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    postProcessingWindow->show();
}

void MainWindow::loadListOfMarkers()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Loading list of markers", 0);

    const QVector<MarkerInformation> markerInfoArray = view->annotation->getMarkerInfoArray();

    ui->twMarkers->clearContents();
    if (markerInfoArray.empty()){
        ui->twMarkers->setRowCount(0);
        return;
    }

    QTableWidgetItem *newItem = nullptr;
    QColor qc;

    int size = markerInfoArray.size();
    ui->twMarkers->setRowCount(size);

    addBackgroundInListOfMarkers();

    int i = 1;
    for (MarkerInformation markerInfo: markerInfoArray){
        if (markerInfo.label() == 0)
            continue; //TODO insert description field in markerInformation

        qDebug() << i;
        QToolButton *tb = new QToolButton();
        tb->setIcon(QIcon(":/Images/icons/visibility.svg"));
        tb->setCheckable(true);
        tb->setChecked(false);
        connect(tb, SIGNAL(clicked()), this, SLOT(slotChangeMarkerVisibility()));
        ui->twMarkers->setCellWidget(i,0, tb);

        newItem = new QTableWidgetItem();
        // make the cell not editable
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        ui->twMarkers->setItem(i,1,newItem);
        qc = iftColorToQColor(markerInfo.color());
        ui->twMarkers->item(i,1)->setBackground(qc);

        newItem = new QTableWidgetItem("obj"+QString::number(markerInfo.label()));
        newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
        newItem->setTextAlignment(Qt::AlignCenter);
        ui->twMarkers->setItem(i,2,newItem);

        tb = new QToolButton();
        tb->setIcon(QIcon(":/Images/icons/pen.svg"));
        connect(tb, SIGNAL(clicked()), this, SLOT(slotTableOpenMarkerClicked()));
        ui->twMarkers->setCellWidget(i,3, tb);

        i++;

    }
    createObjectTableActions();
    ui->twMarkers->setRowCount(size);
    qDebug() << ui->twMarkers->rowCount();

    iftWriteOnLog("Loading list of markers took", elapsedTime.elapsed());
}

void MainWindow::addItemInListOfMarkers()
{
    QElapsedTimer elapsedTime; elapsedTime.start();

    int rows = ui->twMarkers->rowCount();

    addItemInListOfMarkers(rows, "obj" + QString::number(view->annotation->getNextLabel() - 1));

    iftWriteOnLog("Item addition to the list of markers took", elapsedTime.elapsed());
}

void MainWindow::addItemInListOfMarkers(int row, QString desc)
{
    QColor qc;
    iftColorTable *t = view->annotation->generateColorTable();
    int rows = ui->twMarkers->rowCount()+1;

    ui->twMarkers->setRowCount(rows);

    QTableWidgetItem *newItem = nullptr;

//    newItem = new QTableWidgetItem(QString::number(row));
//    newItem->setTextAlignment(Qt::AlignCenter);
//    newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
//    newItem->setCheckState(Qt::Checked);
//    ui->twMarkers->setItem(row,0,newItem);

    QToolButton *tb = new QToolButton();
    tb->setIcon(QIcon(":/Images/icons/visibility.svg"));
    tb->setCheckable(true);
    tb->setChecked(false);
    connect(tb, SIGNAL(clicked()), this, SLOT(slotChangeMarkerVisibility()));
    ui->twMarkers->setCellWidget(row,0, tb);

    newItem = new QTableWidgetItem();
    // make the cell not editable
    newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
    ui->twMarkers->setItem(row,1,newItem);
    qc = iftColorToQColor(t->color[view->annotation->getLabel(row)]);
    ui->twMarkers->item(row,1)->setBackground(qc);


    newItem = new QTableWidgetItem(desc);
    newItem->setFlags(newItem->flags() ^ Qt::ItemIsEditable);
    newItem->setTextAlignment(Qt::AlignCenter);
    ui->twMarkers->setItem(row,2,newItem);

//    newItem = new QTableWidgetItem("1.0");
//    newItem->setTextAlignment(Qt::AlignCenter);
//    ui->twMarkers->setItem(row,3,newItem);

    tb = new QToolButton();
    tb->setIcon(QIcon(":/Images/icons/pen.svg"));
    connect(tb, SIGNAL(clicked()), this, SLOT(slotTableOpenMarkerClicked()));
    ui->twMarkers->setCellWidget(row,3, tb);

    // Put a model in ui->twMarkers->setCellWidget();

    iftDestroyColorTable(&t);
}

void MainWindow::addBackgroundInListOfMarkers()
{
    addItemInListOfMarkers(0, "background");
}

void MainWindow::set2DVisualizationMode()
{
    ui->sliceSagittal->hide();
    ui->sliceCoronal->hide();
    ui->widgetRendering->hide();
    ui->sliceAxial->set2DVisualizationMode();
}

void MainWindow::set3DVisualizationMode()
{
    ui->sliceAxial->set3DVisualizationMode();
    ui->sliceSagittal->show();
    ui->sliceCoronal->show();
    ui->widgetRendering->show();
}

void MainWindow::setMultiTemporalVisualizationMode()
{
    ui->sliderTime->setMaximum(view->getImageSequence()->tsize - 1);
    ui->lbTime->setText(QString::number(view->getImageSequence()->tsize));
    ui->gpTimeControl->show();
}

void MainWindow::setUniTemporalVisualizationMode()
{
    ui->gpTimeControl->hide();
}

void MainWindow::initializeAllGraphicalViews()
{
    ui->sliceAxial->initalizeGraphicalView();
    ui->sliceSagittal->initalizeGraphicalView();
    ui->sliceCoronal->initalizeGraphicalView();
}

void MainWindow::slotShowVolumeInformation()
{
    QElapsedTimer elapsedTime; elapsedTime.start();

    if (!view->isImageEmpty()){
        const iftImage *aux = view->getImage();
        vol_info->showVolumeInformation(aux,getFileName());
        vol_info->show();
    }

    iftWriteOnLog("Show volume of interest took", elapsedTime.elapsed());
}

void MainWindow::updateAllGraphicalViews()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Updating all graphics view", 0);

    ui->sliceAxial->updateAllGraphicalViews(); // TODO: REVIEW

    //TODO remove
    ui->widgetRendering->updateRendition();

    //updateAllLines();

    iftWriteOnLog("Updating all graphics view took", elapsedTime.elapsed());
}

void MainWindow::zoomIn()
{
    this->ui->sliceAxial->slotZoomIn();
    this->ui->sliceSagittal->slotZoomIn();
    this->ui->sliceCoronal->slotZoomIn();
    this->ui->widgetRendering->slotZoomIn();
}

void MainWindow::zoomOut()
{
    this->ui->sliceAxial->slotZoomOut();
    this->ui->sliceSagittal->slotZoomOut();
    this->ui->sliceCoronal->slotZoomOut();
    this->ui->widgetRendering->slotZoomOut();
}

void MainWindow::normalSize()
{
    this->ui->sliceAxial->slotNormalSize();
    this->ui->sliceSagittal->slotNormalSize();
    this->ui->sliceCoronal->slotNormalSize();
    this->ui->widgetRendering->slotNormalSize();
}

void MainWindow::slotChangeGradientMethod()
{
    QVariant variant = ui->cbGradient->currentData();
    ArcWeightFunction *method = variant.value<ArcWeightFunction*>();

    if (method != currentArcWeightFunction && view->getGradient() != nullptr) {

        QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Change Arc-weight function"),
                             "Arc-weight function " + currentArcWeightFunction->name() + " is already loaded."
                             " Do you want to proceed?",
                             QMessageBox::Yes | QMessageBox::No);

       if (bt_clicked == QMessageBox::Yes) {
            view->setGradient(nullptr, 0);            
            ui->cbShowGradient->setCheckState(Qt::Unchecked);

       } else {
            QVariant var = QVariant::fromValue(currentArcWeightFunction);
            int index = ui->cbGradient->findData(var);
            disconnect(ui->cbGradient, SIGNAL(currentIndexChanged(int)), nullptr, nullptr);
            ui->cbGradient->setCurrentIndex(index);
            connect(ui->cbGradient, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGradientMethod()));
            return;
       }
    }

    currentArcWeightFunction = nullptr;

    for (Segmentation *segm: segmentationViews) {
        segm->notifyGradientUpdate(method);
    }

    if (ui->widgetGradientModule->layout()->count() > 0) {
        ui->widgetGradientModule->layout()->takeAt(ui->widgetGradientModule->layout()->count()-1)->widget()->hide();
        ui->widgetGradientModule->layout()->update();
    }

    method->show();
    ui->widgetGradientModule->layout()->addWidget(method);
    method->preprocess();
}

void MainWindow::slotCalculateGradient()
{
    QVariant variant = ui->cbGradient->currentData();
    ArcWeightFunction *method = variant.value<ArcWeightFunction*>();
    method->generate();
    currentArcWeightFunction = method;
}

void MainWindow::slotResetGradient()
{
    view->setGradient(nullptr, 0);
    for (Segmentation *segm: segmentationViews) {
        segm->notifyGradientUpdate(nullptr);
    }
    ui->cbShowGradient->setCheckState(Qt::Unchecked);
    currentArcWeightFunction = nullptr;
    updateAllGraphicalViews();
}

void MainWindow::slotAbout()
{
    AboutDialog *about = new AboutDialog(this);
    about->exec();
    delete about;
}

void MainWindow::slotChangeObjectOpacity(QTableWidgetItem* item)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing object opacity", 0);

    if (item->column() != 3)
        return;

    QString q = item->text();
    float val = q.toFloat();
    if (val > 1.0f){
        QMessageBox::critical(this,"Error","The maximum opacity of an object is 1.0.",QMessageBox::Close);
        return;
    }
    this->view->rendition->setObjectOpacity(item->row()+1,val);
    ui->widgetRendering->updateRendition();

    iftWriteOnLog("Changing object opacity took", elapsedTime.elapsed());
}

void MainWindow::slotChangeObjectVisibility()
{
    QToolButton *tb = qobject_cast<QToolButton *>(sender());
    if(tb){
        QElapsedTimer elapsedTime; elapsedTime.start();
        iftWriteOnLog("Changing object visibility", 0);

        int row = ui->twObjects->indexAt(tb->pos()).row();

        bool visible = !tb->isChecked();
        tb->setChecked(!visible);
        if (visible) {
            tb->setIcon(QIcon(":/Images/icons/visibility.svg"));
        } else {
            tb->setIcon(QIcon(":/Images/icons/visibility_off.svg"));
        }

        view->rendition->setObjectVisibility(row+1,visible);

        updateAllGraphicalViews();

        iftWriteOnLog("Changing object visibility took", elapsedTime.elapsed());
    }
}

void MainWindow::slotChangeObjectColor(int row, int col)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing object color", 0);

    if (col != 1)
        return;

    /*
     * Changing Color Object
     */

    iftColorTable *t;
    iftColor RGB, YCbCr;
    QColor color, c;

    t = this->view->getColorTable();
    RGB = iftYCbCrtoRGB(t->color[row],255);
    iftDestroyColorTable(&t);

    c.setRed(RGB.val[0]);
    c.setGreen(RGB.val[1]);
    c.setBlue(RGB.val[2]);

    color = QColorDialog::getColor(c, this, "Select Color");
    if (!color.isValid())
        return;
    color.getRgb(&(RGB.val[0]),&(RGB.val[1]),&(RGB.val[2]));
    YCbCr = iftRGBtoYCbCr(RGB,255);
    this->view->setObjectColorInColorTable(row,YCbCr);

    ui->twObjects->item(row,1)->setBackground(color);

    updateAllGraphicalViews();
    iftWriteOnLog("Changing object color took", elapsedTime.elapsed());
}

void MainWindow::slotChangeMarkerVisibility()
{
    QToolButton *tb = qobject_cast<QToolButton *>(sender());
    if(tb){
        int row = ui->twMarkers->indexAt(tb->pos()).row();

        bool visible = !tb->isChecked();

        slotChangeMarkerVisibility(row, visible);
    }
}

void MainWindow::slotChangeMarkerVisibility(int row, bool visible)
{
    int value = visible ? 1 : 0;
    int label = view->annotation->getLabel(row);
    view->annotation->setMarkerVisibility(label, value);

    QToolButton *tb = (QToolButton*) ui->twMarkers->cellWidget(row,0);
    tb->setChecked(!visible);

    if (ms != nullptr) {
        ms->setMarkerVisibility(row, visible);
    }

    if (visible) {
        tb->setIcon(QIcon(":/Images/icons/visibility.svg"));
    } else {
        tb->setIcon(QIcon(":/Images/icons/visibility_off.svg"));
    }

    updateAllGraphicalViews();
}

void MainWindow::slotChangeBrightness()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing brightness", 0);

    int b, c;
    double bf,cf;

    b = (100-ui->hsBrightness->value());
    c = (100-ui->hsContrast->value());

    bf = double(b)*view->maxNormalizedValue/100.0;
    cf = double(c)*view->maxNormalizedValue/100.0;

    double f1, f2, g1, g2;

    f1 = (2*bf - cf)/2;
    f2 = cf + f1;
    g1 = 0;
    g2 = 255;

    SliceView::updateBrightnessAndConstrast(f1, f2, g1, g2);
    ui->widgetRendering->updateBrightnessAndConstrast(f1, f2, g1, g2);

    ui->lblBrightCount->setText(QString::number(ui->hsBrightness->value()));

    updateAllGraphicalViews();
    iftWriteOnLog("Changing brightness took", elapsedTime.elapsed());
}

void MainWindow::slotChangeContrast()
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Changing contrast", 0);

    int b, c;
    double bf,cf;

    b = (100-ui->hsBrightness->value());
    c = (100-ui->hsContrast->value());

    bf = double(b)*view->maxNormalizedValue/100.0;
    cf = double(c)*view->maxNormalizedValue/100.0;

    double f1, f2, g1, g2;

    f1 = (2*bf - cf)/2;
    f2 = cf + f1;
    g1 = 0;
    g2 = 255;

    SliceView::updateBrightnessAndConstrast(f1, f2, g1, g2);
    ui->widgetRendering->updateBrightnessAndConstrast(f1, f2, g1, g2);

    ui->lblContrastCount->setText(QString::number(ui->hsContrast->value()));

    updateAllGraphicalViews();

    iftWriteOnLog("Changing contrast took", elapsedTime.elapsed());
}

void MainWindow::slotMarkerDoubleClick(int row, int col)
{
    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Marker double clicked", 0);

    if (col == 1)
        slotChangeMarkerColor(row);
    else if (col == 2) {
        MarkerInformation marker_active = view->annotation->getMarkerInfoArray().getActive();
        if (view->annotation->getAnnotationMode() == FREE_FORM_ANNOTATION
                && row == view->annotation->getIndexFromLabel(marker_active.label())) {
            slotHaltAnnotation();
        } else {
            int label = view->annotation->getLabel(row);
            view->annotation->getMarkerInfoArray().activate(label);
            slotStartAnnotation(FREE_FORM_ANNOTATION);
        }
    }
    else if (col == 0) {
//        iftColor YCbCr = view->annotation->getMarkerColor(row);
//        QColor c = iftColorToQColor(YCbCr);
//        int value = row;//ui->twMarkers->item(row, 0)->text().toInt();
//        QString s = ui->twMarkers->item(row,2)->text();
//        float radius = view->annotation->getBrushRadius();
//        bool visible = ((QToolButton*) ui->twMarkers->cellWidget(row, 0))->isChecked();
//        //ui->twMarkers->item(row, 0)->checkState() == Qt::Checked;
//        if (ms != nullptr)
//            ms->fillMarkerData(value,c,s,radius, visible);
//        else {
//            ms = new MarkerSettings(this);
//            ms->setWindowFlag(Qt::Tool,true);
//            ms->setAttribute(Qt::WA_DeleteOnClose);
//            ms->fillMarkerData(value,c,s,radius, visible);
//            ms->show();
//            createMarkerSettingsConnections();
//        }
    }
    iftWriteOnLog("Marker double clicked", elapsedTime.elapsed());
}

void MainWindow::slotMarkerChanged(int row, int col)
{
    if (col == 2){
       view->annotation->getMarkerInfoArray()[row].setName(ui->twMarkers->item(row,2)->text());
       view->annotation->setMarkerName(row, ui->twMarkers->item(row,2)->text());
       if (ms != nullptr)
           //TODO create signal in MarkerInfo that notify MarkerSettings
           ms->updateMarkerNameOnForm(ui->twMarkers->item(row,2)->text());
   }
}

void MainWindow::slotAddNewMarker()
{
    view->annotation->addItemInColorTable();
    addItemInListOfMarkers();
}

void MainWindow::slotRemoveMarker()
{

    int r = ui->twMarkers->currentRow();
    if (r == -1)
        return;

    QMessageBox::StandardButton bt_clicked = QMessageBox::warning(this, tr("Remover marker"),
                         "This operation cannot be undone. Do you want to proceed?",
                         QMessageBox::Yes | QMessageBox::No);

    if (bt_clicked == QMessageBox::Yes) {
        QElapsedTimer elapsedTime; elapsedTime.start();
        iftWriteOnLog("Removing marker", 0);

        int label = view->annotation->getLabel(r);

        view->annotation->removeMarker(label);
        loadListOfMarkers();
        updateAllGraphicalViews();
        iftWriteOnLog("Removing marker took", elapsedTime.elapsed());
    }


}

void MainWindow::slotChangeMarkerColor(int row)
{
    /*
     * Changing Color of a Marker
     */

    QElapsedTimer elapsedTime; elapsedTime.start();
    iftWriteOnLog("Chaging marker color", 0);

    iftColor RGB, YCbCr;
    QColor color, c;

    RGB = iftYCbCrtoRGB(view->annotation->getMarkerInfoArray()[row].color(),255);

    c.setRed(RGB.val[0]);
    c.setGreen(RGB.val[1]);
    c.setBlue(RGB.val[2]);

    color = QColorDialog::getColor(c, this, "Select Color");
    if (!color.isValid())
        return;
    color.getRgb(&(RGB.val[0]),&(RGB.val[1]),&(RGB.val[2]));
    YCbCr = iftRGBtoYCbCr(RGB,255);
    int label = view->annotation->getLabel(row);
    this->view->annotation->setObjectColorInColorTable(label,YCbCr);

    ui->twMarkers->item(row,1)->setBackground(color);

    updateAllGraphicalViews();

    if (ms != nullptr)
        ms->updateMarkerColorOnForm(color);

    iftWriteOnLog("Chaging marker color took", elapsedTime.elapsed());
}

void MainWindow::slotMarkAll()
{
    int row;
    bool visible = false;

    if (ui->cbMarkAll->isChecked()){
        visible = true;
    }

    for (row = 0; row < ui->twObjects->rowCount(); row++){
        QToolButton *tb = (QToolButton *) ui->twObjects->cellWidget(row,0);
        if ( (visible && tb->isChecked()) || (!visible && !tb->isChecked())) {
            tb->click(); // TODO this loses performance
        }
    }
}

void MainWindow::slotUpdateBrushRadius(float r)
{
    view->annotation->setBrushRadius(r);
    ui->sliceAxial->startAnnotation(0);//TODO
    ui->sliceCoronal->startAnnotation(0);//TODO
    ui->sliceSagittal->startAnnotation(0);//TODO
}

void MainWindow::slotUpdateMarkerName(int label, QString s)
{
    int index = view->annotation->getIndexFromLabel(label);

    view->annotation->getMarkerInfoArray()[index].setName(s);
    ui->twMarkers->item(index, 2)->setText(s);
    return;
}

void MainWindow::slotUpdateMarkerColor(int label, QColor color)
{
    int index = view->annotation->getIndexFromLabel(label);

    iftColor RGB, YCbCr;

    color.getRgb(&(RGB.val[0]),&(RGB.val[1]),&(RGB.val[2]));
    YCbCr = iftRGBtoYCbCr(RGB,255);
    this->view->annotation->setObjectColorInColorTable(label,YCbCr);

    ui->twMarkers->item(index, 1)->setBackground(color);
    updateAllGraphicalViews();

    return;
}

void MainWindow::slotDestroyMarkerSettingsWindow()
{
    ui->sliceAxial->startAnnotation(0);//TODO refactor
    ui->sliceCoronal->startAnnotation(0);
    ui->sliceSagittal->startAnnotation(0);
    destroyMarkerSettingsConnections();
    ms = nullptr;
}

void MainWindow::slotStartAnnotation(int mode)
{   
    view->annotation->setAnnotationMode(mode);
    ui->lblAnnotationMode->setText("Status: Started");
    ui->lblAnnotationMode->setStyleSheet("color: rgb(78, 154, 6);");

    ui->sliceAxial->startAnnotation(mode);
    ui->sliceCoronal->startAnnotation(mode);
    ui->sliceSagittal->startAnnotation(mode);

    if (view->annotation->getMarkerInfoArray().getActive().label() == -1) {
        int lastActive = view->annotation->getMarkerInfoArray().lastActive();
        view->annotation->getMarkerInfoArray().activate(lastActive);
    }

    int label_active = view->annotation->getMarkerInfoArray().getActive().label();
    for (int r = 0; r < ui->twMarkers->rowCount(); r++) {
        QFont font = ui->twMarkers->item(r, 2)->font();
        font.setBold(view->annotation->getLabel(r) == label_active);
        ui->twMarkers->item(r, 2)->setFont(font);
    }
}

void MainWindow::slotEraseAnnotation()
{
    view->annotation->setAnnotationMode(ERASING_ANNOTATION);
    ui->lblAnnotationMode->setText("Status: Erasing");
    ui->lblAnnotationMode->setStyleSheet("color: rgb(196, 160, 0);");
    ui->sliceAxial->startAnnotation(0);//TODO refactor
    ui->sliceCoronal->startAnnotation(0);
    ui->sliceSagittal->startAnnotation(0);
}

void MainWindow::slotHaltAnnotation()
{
    view->annotation->setAnnotationMode(HALT_ANNOTATION);
    ui->lblAnnotationMode->setText("Status: Halted");
    ui->lblAnnotationMode->setStyleSheet("color: rgb(204, 0, 0);");
    ui->sliceAxial->startAnnotation(0);//TODO refactor
    ui->sliceCoronal->startAnnotation(0);
    ui->sliceSagittal->startAnnotation(0);
    view->annotation->getMarkerInfoArray().deactivate();
    for (int r = 0; r < ui->twMarkers->rowCount(); r++) {
        QFont font = ui->twMarkers->item(r, 2)->font();
        font.setBold(false);
        ui->twMarkers->item(r, 2)->setFont(font);
    }
}

void MainWindow::slotTableOpenMarkerClicked()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if(w){
        int row = ui->twMarkers->indexAt(w->pos()).row();

        // TODO remove duplicity
        int label = view->annotation->getLabel(row);//ui->twMarkers->item(row, 0)->text().toInt();
        iftColor YCbCr = view->annotation->getMarkerColor(label);
        QColor c = iftColorToQColor(YCbCr);
        QString s = ui->twMarkers->item(row,2)->text();
        float radius = view->annotation->getBrushRadius();
        bool visible = !((QToolButton*) ui->twMarkers->cellWidget(row, 0))->isChecked();
        view->annotation->getMarkerInfoArray().activate(label);
        if (ms != nullptr)
            ms->fillMarkerData(label,c,s,radius, visible);
        else {
            ms = MarkerSettings::instance(this);
            ms->setWindowFlag(Qt::Tool,true);
            ms->setAttribute(Qt::WA_DeleteOnClose);
            ms->fillMarkerData(label,c,s,radius, visible);
            createMarkerSettingsConnections();
            ms->move(QCursor::pos());
            ms->show();
        }
    }
}

void MainWindow::slotTableDeleteObjectClicked()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if(w){
        int row = ui->twObjects->indexAt(w->pos()).row();

        int label = row + 1; //TODO fix it

        iftImage *label_img = iftCopyImage(view->getLabel());
        #pragma omp parallel for
        for(int i = 0; i < label_img->n; i++) {
            if (label_img->val[i] == label) {
                label_img->val[i] = 0;
            }
        }

        iftColorTable *ctb = view->annotation->getMarkerInfoArray().generateColorTable();

        view->setLabel(label_img, ctb);//TODO unify with loadLabelFromCommandLine to LoadLabel(iftImage *)
        iftDestroyImage(&label_img);

        view->setRenditionLabel();

        updateAllGraphicalViews();
        loadListOfObjects();
    }
}


void MainWindow::setSystemMode(QString mode)
{
    if (mode == "INITIALIZATION"){
        ui->cbMarkAll->setEnabled(false);
        ui->hsBrightness->setEnabled(false);
        ui->hsContrast->setEnabled(false);
        ui->pbAddMarker->setEnabled(false);
        ui->pbNormalSize->setEnabled(false);
        ui->pbRemoveMarker->setEnabled(false);
        ui->sliceAxial->setEnabled(false);
        ui->sliceSagittal->setEnabled(false);
        ui->sliceCoronal->setEnabled(false);
        ui->widgetRendering->setEnabled(false);
        ui->gbSegmentation->setEnabled(false);
        ui->gpTimeControl->setEnabled(false);
        ui->pbZoomIn->setEnabled(false);
        ui->pbZoomOut->setEnabled(false);
        ui->twMarkers->setEnabled(false);
        ui->gpObjects->setEnabled(false);
        ui->gbGradient->setEnabled(false);
        openFileAct->setEnabled(true);
        importLabelAct->setEnabled(false);
        exportLabelAct->setEnabled(false);
        exportImageSequenceAct->setEnabled(false);
        importMarkersAct->setEnabled(false);
        exportMarkersAct->setEnabled(false);
        openVolumeInformationAct->setEnabled(false);
        zoomInAct->setEnabled(false);
        zoomOutAct->setEnabled(false);
        exitAct->setEnabled(true);
        normalSizeAct->setEnabled(false);
        aboutAct->setEnabled(true);
        lineVisibilityAct->setEnabled(false);
    }else if (mode == "OPEN_FILE"){
        ui->cbMarkAll->setEnabled(false);
        ui->hsBrightness->setEnabled(true);
        ui->hsContrast->setEnabled(true);
        ui->pbAddMarker->setEnabled(true);
        ui->pbNormalSize->setEnabled(true);
        ui->pbRemoveMarker->setEnabled(true);
        ui->sliceAxial->setEnabled(true);
        ui->sliceSagittal->setEnabled(true);
        ui->sliceCoronal->setEnabled(true);
        ui->widgetRendering->setEnabled(true);
        ui->gbSegmentation->setEnabled(true);
        ui->gpTimeControl->setEnabled(true);
        ui->pbZoomIn->setEnabled(true);
        ui->pbZoomOut->setEnabled(true);
        ui->twMarkers->setEnabled(true);
        ui->gpObjects->setEnabled(false);
        ui->gbGradient->setEnabled(true);
        openFileAct->setEnabled(true);
        importLabelAct->setEnabled(true);
        exportLabelAct->setEnabled(false);
        exportImageSequenceAct->setEnabled(true);
        importMarkersAct->setEnabled(true);
        exportMarkersAct->setEnabled(true);
        openVolumeInformationAct->setEnabled(true);
        zoomInAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
        exitAct->setEnabled(true);
        normalSizeAct->setEnabled(true);
        aboutAct->setEnabled(true);
        lineVisibilityAct->setEnabled(true);
    } else if(mode == "OPEN_LABEL"){
        ui->cbMarkAll->setEnabled(true);
        ui->hsBrightness->setEnabled(true);
        ui->hsContrast->setEnabled(true);
        ui->pbAddMarker->setEnabled(true);
        ui->pbNormalSize->setEnabled(true);
        ui->pbRemoveMarker->setEnabled(true);
        ui->gbGradient->setEnabled(true);
        ui->sliceAxial->setEnabled(true);
        ui->sliceSagittal->setEnabled(true);
        ui->sliceCoronal->setEnabled(true);
        ui->widgetRendering->setEnabled(true);
        ui->gbSegmentation->setEnabled(true);
        ui->gpTimeControl->setEnabled(true);
        ui->pbZoomIn->setEnabled(true);
        ui->pbZoomOut->setEnabled(true);
        ui->twMarkers->setEnabled(true);
        ui->gpObjects->setEnabled(true);
        openFileAct->setEnabled(true);
        importLabelAct->setEnabled(true);
        exportLabelAct->setEnabled(true);
        exportImageSequenceAct->setEnabled(true);
        importMarkersAct->setEnabled(true);
        exportMarkersAct->setEnabled(true);
        openVolumeInformationAct->setEnabled(true);
        zoomInAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
        exitAct->setEnabled(true);
        normalSizeAct->setEnabled(true);
        aboutAct->setEnabled(true);
        lineVisibilityAct->setEnabled(true);
    }
}

void MainWindow::showSegmentationModule(Segmentation *module)
{
    QVariant var = QVariant::fromValue(module);
    int index = ui->cbSegmentation->findData(var);
    ui->cbSegmentation->setCurrentIndex(index);
}

void MainWindow::setMarkers(iftImage *markers)
{
    int max = iftMaximumValue(markers);

    for (int i = 0; i <= max; i++){
        view->annotation->addItemInColorTable();
    }

    view->annotation->setMarkers(markers);
    //iftDestroyImage(&markers);

    loadListOfMarkers();
    updateAllGraphicalViews();
}

//TODO move definition to Marker
void MainWindow::setMarkers(iftLabeledSet *S)
{
    iftImage *markers = iftCreateImageFromImage(view->annotation->getMarkers());
    iftInitMarkersImage(markers);

    int max = 0;
    while (S != nullptr){
        int label;
        int p = iftRemoveLabeledSet(&S,&label);
        iftVoxel u = iftGetVoxelCoord(markers,p);
        if (!iftValidVoxel(markers,u))
            continue;
        markers->val[p] = label;
        if (label > max)
            max = label;
    }

    view->annotation->destroyColorTable();

    // copy label color table to marker color table
    iftColorTable *labelCtb = view->getColorTable();
    view->annotation->addItemInColorTable(0);
    for (int i = 1; i <= max; i++){
        iftColor *c = nullptr;
        if (labelCtb->ncolors == max) {
            c = &labelCtb->color[i-1];
        }
        view->annotation->addItemInColorTable(i, c);
    }

    view->annotation->setMarkers(markers);

    loadListOfMarkers();
    updateAllGraphicalViews();

    iftDestroyColorTable(&labelCtb);
}

void MainWindow::setAnnotationVisibility(bool visible)
{
    ui->layoutAnnotation->setVisible(visible);
}

QString MainWindow::getFileName()
{
    return view->getFilename();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (view->annotation->getAnnotationMode() == FREE_FORM_ANNOTATION)
            slotHaltAnnotation();
        else
            slotStartAnnotation(FREE_FORM_ANNOTATION);
    }
}

void MainWindow::createHotkeys()
{
    openFileAct = new QAction(tr("Open"),this);
    openFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(openFileAct,SIGNAL(triggered()),this,SLOT(slotOpenFile()));

    importLabelAct = new QAction(tr("Label"),this);
    importLabelAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(importLabelAct,SIGNAL(triggered()),this,SLOT(slotImportLabel()));

    exportLabelAct = new QAction(tr("Label"),this);
    exportLabelAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_L));
    connect(exportLabelAct,SIGNAL(triggered()),this,SLOT(slotExportLabel()));

    processingAct = new QAction(tr("Open post-processing options"),this);
    connect(processingAct, SIGNAL(triggered()), this, SLOT(slotOpenProcessingOptions()));

    importMarkersAct = new QAction(tr("Markers"),this);
    importMarkersAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(importMarkersAct,SIGNAL(triggered()),this,SLOT(slotImportMarkers()));

    exportImageSequenceAct = new QAction(tr("Image Sequence"),this);
    exportImageSequenceAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    connect(exportImageSequenceAct,SIGNAL(triggered()),this,SLOT(slotExportImageSequence()));

    exportMarkersAct = new QAction(tr("Markers"),this);
    exportMarkersAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_M));
    connect(exportMarkersAct,SIGNAL(triggered()),this,SLOT(slotExportMarkers()));

    exitAct = new QAction(tr("Exit"),this);
    exitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    connect(exitAct,SIGNAL(triggered()),this,SLOT(close()));

    openVolumeInformationAct = new QAction(tr("Volume Information"),this);
    connect(openVolumeInformationAct,SIGNAL(triggered()),this,SLOT(slotShowVolumeInformation()));

    zoomInAct = new QAction(tr("Zoom In (25%)"),this);
    zoomInAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
    zoomInAct->setEnabled(false);
    connect(zoomInAct,SIGNAL(triggered()),this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom Out (25%)"),this);
    zoomOutAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct,SIGNAL(triggered()),this,SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("Normal Size"),this);
    normalSizeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct,SIGNAL(triggered()),this,SLOT(normalSize()));

    aboutAct = new QAction(tr("About"),this);
    aboutAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    connect(aboutAct,SIGNAL(triggered()),this,SLOT(slotAbout()));

    lineVisibilityAct = new QAction(tr("Lines"),this);
    lineVisibilityAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(lineVisibilityAct,SIGNAL(triggered()),ui->sliceAxial,SLOT(slotToogleLinesVisibility()));
    connect(lineVisibilityAct,SIGNAL(triggered()),ui->sliceCoronal,SLOT(slotToogleLinesVisibility()));
    connect(lineVisibilityAct,SIGNAL(triggered()),ui->sliceSagittal,SLOT(slotToogleLinesVisibility()));
}

void MainWindow::createActions()
{
    connect(ui->cbShowGradient, SIGNAL(stateChanged(int)), ui->sliceAxial, SLOT(slotSetGradientVisibility(int)));
    connect(ui->cbShowGradient, SIGNAL(stateChanged(int)), ui->sliceCoronal, SLOT(slotSetGradientVisibility(int)));
    connect(ui->cbShowGradient, SIGNAL(stateChanged(int)), ui->sliceSagittal, SLOT(slotSetGradientVisibility(int)));

    connect(ui->cbGradient, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeGradientMethod()));

    connect(ui->sliderTime, SIGNAL(valueChanged(int)), this, SLOT(slotShowVolumeTimeThumbnail(int)));
    connect(ui->sliderTime, SIGNAL(sliderReleased()), this, SLOT(slotChangeVolume()));

    connect(ui->sliceAxial, SIGNAL(requestGradientCalculation()), this, SLOT(slotCalculateGradient()));
    connect(ui->sliceCoronal, SIGNAL(requestGradientCalculation()), this, SLOT(slotCalculateGradient()));
    connect(ui->sliceCoronal, SIGNAL(requestGradientCalculation()), this, SLOT(slotCalculateGradient()));

    connect(ui->pbZoomIn, SIGNAL(clicked(bool)), this, SLOT(zoomIn()));
    connect(ui->pbZoomOut, SIGNAL(clicked(bool)), this, SLOT(zoomOut()));
    connect(ui->pbNormalSize, SIGNAL(clicked(bool)), this, SLOT(normalSize()));
    connect(ui->sliceAxial->gvSlice(), SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    connect(ui->sliceSagittal->gvSlice(), SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    connect(ui->sliceCoronal->gvSlice(), SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    connect(ui->sliceAxial->gvSlice(), SIGNAL(zoomOut()), this, SLOT(zoomOut()));
    connect(ui->sliceSagittal->gvSlice(), SIGNAL(zoomOut()), this, SLOT(zoomOut()));
    connect(ui->sliceCoronal->gvSlice(), SIGNAL(zoomOut()), this, SLOT(zoomOut()));

    connect(ui->hsBrightness, SIGNAL(sliderReleased()), this, SLOT(slotChangeBrightness()));
    connect(ui->hsContrast, SIGNAL(sliderReleased()), this, SLOT(slotChangeContrast()));

    connect(ui->pbAddMarker, SIGNAL(clicked(bool)), this, SLOT(slotAddNewMarker()));
    connect(ui->pbRemoveMarker, SIGNAL(clicked(bool)), this, SLOT(slotRemoveMarker()));

    connect(ui->pbExportObjects, SIGNAL(clicked(bool)), this, SLOT(slotExportObjectsCSV()));

    connect(ui->cbSegmentation, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeSegmentationMethod(int)));

    connect(ui->widgetRendering, SIGNAL(updatedGraphicsViews()), this, SLOT(updateAllGraphicalViews()));

    ui->sliceAxial->createActions();
    ui->sliceSagittal->createActions();
    ui->sliceCoronal->createActions();
    ui->widgetRendering->createActions();
}

void MainWindow::destroyActions()
{
    disconnect(ui->cbGradient, SIGNAL(currentIndexChanged(int)), nullptr, nullptr);
    disconnect(ui->cbShowGradient, SIGNAL(stateChanged(int)), nullptr, nullptr);
    disconnect(ui->sliceAxial, SIGNAL(requestGradientCalculation()), nullptr, nullptr);
    disconnect(ui->sliceCoronal, SIGNAL(requestGradientCalculation()), nullptr, nullptr);
    disconnect(ui->sliceSagittal, SIGNAL(requestGradientCalculation()), nullptr, nullptr);

    disconnect(ui->sliderTime, SIGNAL(valueChanged(int)), nullptr, nullptr);
    disconnect(ui->sliderTime, SIGNAL(sliderReleased()), nullptr, nullptr);

    disconnect(ui->pbZoomIn, SIGNAL(clicked(bool)),nullptr,nullptr);
    disconnect(ui->pbZoomOut, SIGNAL(clicked(bool)),nullptr,nullptr);
    disconnect(ui->pbNormalSize, SIGNAL(clicked(bool)),nullptr,nullptr);

    disconnect(ui->sliceAxial->gvSlice(), SIGNAL(zoomIn()), nullptr,nullptr);
    disconnect(ui->sliceSagittal->gvSlice(), SIGNAL(zoomIn()), nullptr,nullptr);
    disconnect(ui->sliceCoronal->gvSlice(), SIGNAL(zoomIn()), nullptr,nullptr);
    disconnect(ui->sliceAxial->gvSlice(), SIGNAL(zoomOut()), nullptr,nullptr);
    disconnect(ui->sliceSagittal->gvSlice(), SIGNAL(zoomOut()), nullptr,nullptr);
    disconnect(ui->sliceCoronal->gvSlice(), SIGNAL(zoomOut()), nullptr,nullptr);

    disconnect(ui->hsBrightness, SIGNAL(valueChanged(int)),nullptr,nullptr);
    disconnect(ui->hsContrast, SIGNAL(valueChanged(int)),nullptr,nullptr);

    disconnect(ui->pbAddMarker, SIGNAL(clicked(bool)), nullptr,nullptr);
    disconnect(ui->pbRemoveMarker, SIGNAL(clicked(bool)), nullptr, nullptr);

    disconnect(ui->pbExportObjects, SIGNAL(clicked(bool)), nullptr, nullptr);

    disconnect(ui->cbSegmentation, SIGNAL(currentIndexChanged(int)), nullptr, nullptr);

    ui->sliceAxial->destroyActions();
    ui->sliceSagittal->destroyActions();
    ui->sliceCoronal->destroyActions();
    ui->widgetRendering->destroyActions();
}

void MainWindow::createObjectTableActions()
{
    connect(ui->twObjects, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slotChangeObjectColor(int,int)));
    //connect(ui->twObjects, SIGNAL(cellClicked(int,int)), this, SLOT(slotChangeObjectVisibility(int,int)));
    connect(ui->twObjects, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotChangeObjectOpacity(QTableWidgetItem*)));
    connect(ui->cbMarkAll, SIGNAL(clicked(bool)), this, SLOT(slotMarkAll()));
}

void MainWindow::destroyObjectTableActions()
{
    disconnect(ui->twObjects, SIGNAL(cellDoubleClicked(int,int)), nullptr,nullptr);
    //disconnect(ui->twObjects, SIGNAL(cellClicked(int,int)), nullptr,nullptr);
    disconnect(ui->twObjects, SIGNAL(itemChanged(QTableWidgetItem*)), nullptr,nullptr);
    disconnect(ui->cbMarkAll, SIGNAL(clicked(bool)), nullptr,nullptr);
}

void MainWindow::createMarkersTableActions()
{
    connect(ui->twMarkers, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slotMarkerDoubleClick(int,int)));
    connect(ui->twMarkers, SIGNAL(cellChanged(int,int)), this, SLOT(slotMarkerChanged(int,int)));
}

void MainWindow::destroyMarkersTableActions()
{
    disconnect(ui->twMarkers, SIGNAL(cellDoubleClicked(int,int)), nullptr,nullptr);
    disconnect(ui->twMarkers, SIGNAL(cellChanged(int,int)), nullptr,nullptr);
}

void MainWindow::createMarkerSettingsConnections()
{
    connect(ms, SIGNAL(UpdateBrush(float)), this, SLOT(slotUpdateBrushRadius(float)));
    connect(ms, SIGNAL(UpdateMarkerName(int, QString)), this, SLOT(slotUpdateMarkerName(int,QString)));
    connect(ms, SIGNAL(UpdateMarkerColor(int,QColor)), this, SLOT(slotUpdateMarkerColor(int,QColor)));

    connect(ms, SIGNAL(StartAnnotation(int)), this, SLOT(slotStartAnnotation(int)));
    connect(ms, SIGNAL(eraseMarker()), this, SLOT(slotEraseAnnotation()));
    connect(ms, SIGNAL(HaltAnnotation()), this, SLOT(slotHaltAnnotation()));

    connect(ms, SIGNAL(updateMarkerVisibility(int, bool)), this, SLOT(slotChangeMarkerVisibility(int, bool)));

    connect(ms, SIGNAL(destroyed()), this, SLOT(slotDestroyMarkerSettingsWindow()));
}

void MainWindow::destroyMarkerSettingsConnections()
{
    disconnect(ms, SIGNAL(UpdateBrush(float)),nullptr,nullptr);
    disconnect(ms, SIGNAL(UpdateMarkerName(int, QString)), nullptr,nullptr);
    disconnect(ms, SIGNAL(UpdateMarkerColor(int,QColor)), nullptr,nullptr);

    disconnect(ms, SIGNAL(StartAnnotation(int)), nullptr,nullptr);
    disconnect(ms, SIGNAL(eraseMarker()), nullptr, nullptr);
    disconnect(ms, SIGNAL(HaltAnnotation()), nullptr,nullptr);

    disconnect(ms, SIGNAL(updateMarkerVisibility(int, bool)), nullptr, nullptr);

    disconnect(ms, SIGNAL(destroyed()), nullptr,nullptr);
}

void MainWindow::setActions()
{
    fileMenu = new QMenu(tr("&File"),this);    
    fileMenu->addAction(openFileAct);
    fileMenu->addSeparator();
    importMenu = new QMenu(tr("Import"),this);
    importMenu->addAction(importLabelAct);
    importMenu->addAction(importMarkersAct);
    fileMenu->addMenu(importMenu);
    fileMenu->addSeparator();
    exportMenu = new QMenu(tr("Export"),this);
    exportMenu->addAction(exportLabelAct);
    exportMenu->addAction(exportMarkersAct);
    exportMenu->addAction(exportImageSequenceAct);
    fileMenu->addMenu(exportMenu);
    fileMenu->addSeparator();    
    fileMenu->addAction(openVolumeInformationAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    processingMenu = new QMenu(tr("&Post-processing"),this);
    processingMenu->addAction(processingAct);

    viewMenu = new QMenu(tr("&View"),this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    helpMenu = new QMenu(tr("&Help"),this);
    helpMenu->addAction(aboutAct);

    methodsMenu = new QMenu(tr("Segmentation"), this);

    QSignalMapper *signalMapper = new QSignalMapper(this);

    for (int i = 1; i < ui->cbSegmentation->count(); i++) {
        QVariant var = ui->cbSegmentation->itemData(i);
        Segmentation *method = var.value<Segmentation*>();

        QAction *actionMethod = new QAction(method->name(), this);
        connect(actionMethod, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(actionMethod, i);
        methodsMenu->addAction(actionMethod);
    }

    connect (signalMapper, SIGNAL(mapped(int)), ui->cbSegmentation, SLOT(setCurrentIndex(int))) ;

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(methodsMenu);
    menuBar()->addMenu(processingMenu);
    menuBar()->addMenu(helpMenu);

    ui->sliceAxial->addAction(lineVisibilityAct);
    ui->sliceCoronal->addAction(lineVisibilityAct);
    ui->sliceSagittal->addAction(lineVisibilityAct);
}

void MainWindow::setIcons()
{
    ui->pbZoomIn->setIcon(QIcon(":/Images/icons/zoomIn.svg"));
    ui->pbZoomOut->setIcon(QIcon(":/Images/icons/zoomOut.svg"));    
    ui->pbNormalSize->setIcon(QIcon(":/Images/icons/normalSize.svg"));
}

void MainWindow::slotChangeSegmentationMethod(int index)
{
    // Hide the last module shown
    if (ui->gbSegmModule->layout()->count() > 0) {
        ui->gbSegmModule->layout()->takeAt(ui->gbSegmModule->layout()->count()-1)->widget()->hide();
    }
    ui->gbSegmModule->layout()->update();

    Segmentation *method = nullptr;
    // Show the selected module
    if (index > 0) {
        QVariant variant = ui->cbSegmentation->currentData();
        method = variant.value<Segmentation*>();
        method->show();
        ui->gbSegmModule->layout()->addWidget(method);
    }
    view->setSegmentationMethod(method);
}

void MainWindow::slotShowVolumeTimeThumbnail(int time)
{
    iftImage *img = view->getThumbnail(time);
    QImage *qimg = iftImageToQImage(img);
    iftDestroyImage(&img);


    QByteArray data;
    QBuffer buffer(&data);
    qimg->save(&buffer, "PNG", 100);
    QString html = QString("<img src='data:image/png;base64, %0'>").arg(QString(data.toBase64()));
    QToolTip::showText(QCursor::pos(), html, this, QRect(), 5000);
    delete qimg;
}

void MainWindow::slotChangeVolume()
{
    int time = ui->sliderTime->value();
    view->displayImageSequenceTime(time);
    updateAllGraphicalViews();
    for (Segmentation *segm: segmentationViews) {
        // TODO maybe replace with emit
        segm->notifyImageUpdate();
    }
}
