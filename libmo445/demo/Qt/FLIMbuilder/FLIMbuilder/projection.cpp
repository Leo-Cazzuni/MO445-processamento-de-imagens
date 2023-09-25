#include "flim.h"
#include "projection.h"
#include "ui_projection.h"
#include "graphedge.h"

Projection::Projection(QWidget *parent, QString model_path, QString seeds_path, QString label_path, int layer_id):
    QDialog(parent),
    ui(new Ui::Projection)
{
    ui->setupUi(this);
    setWindowTitle("Projection");

    this->layer_id     = layer_id;
    this->model_path   = model_path;
    this->seeds_path   = seeds_path;
    this->label_path   = label_path;
    this->project_type = PROJECT_KERNELS;
    this->targetMarker = 1;
    this->workingDataSet = nullptr;

    /* Initializing projection */
    initScene();
    initGraphicsView();
    enableWidgetsByMode("beforeProjection");
    updateNumbSamplesShown();
    drawShadowInPoints = false;

    ui->cbNodeColorByRelevance->addItem("DICE");
    ui->cbNodeColorByRelevance->addItem("Weights");
    ui->cbNodeColorByRelevance->setCurrentIndex(0);
    this->currentNodeColorOption = ui->cbNodeColorByRelevance->currentText();

    ui->projGraphicsView->autoReprojection = true;

    threadTimer = new QTimer(this);
    connect(threadTimer, SIGNAL(timeout()), this, SLOT(timer_slot()));

    ui->gbPatchesMenu->hide();
    ui->pbSaveKernels->hide();

    createConnections();
}

Projection::Projection(QWidget *parent, QString img_path, QString seeds_path, QJsonObject json_arch, int current_training_layer, QString model_path):
    QDialog(parent),
    ui(new Ui::Projection)
{
    ui->setupUi(this);
    setWindowTitle("Projection");

    if (current_training_layer == 1){
        this->image_path         = img_path;
    } else {
        this->image_path         = model_path+QString("/layer%1/").arg(current_training_layer-1);
    }
    this->model_path             = model_path;
    this->seeds_path             = seeds_path;
    this->project_type           = PROJECT_PATCHES;
    this->flim_arch              = JSONArchToFLIMArch(json_arch);
    this->current_training_layer = current_training_layer;
    this->workingDataSet         = nullptr;

    /* Initializing projection */
    initScene();
    initGraphicsView();
    enableWidgetsByMode("beforeProjection");
    updateNumbSamplesShown();
    drawShadowInPoints = false;

    ui->cbNodeColorByRelevance->addItem("DICE");
    ui->cbNodeColorByRelevance->addItem("Weights");
    ui->cbNodeColorByRelevance->setCurrentIndex(0);
    this->currentNodeColorOption = ui->cbNodeColorByRelevance->currentText();

    ui->projGraphicsView->autoReprojection = true;

    threadTimer = new QTimer(this);
    connect(threadTimer, SIGNAL(timeout()), this, SLOT(timer_slot()));

    // hiding layer selection option
    ui->gbSaveRelevance->hide();
    ui->pbSaveRelevance->hide();

    createConnections();
}

Projection::~Projection()
{
    delete ui;
    iftDestroyDataSet(&this->originalDataSet);
    iftDestroyDataSet(&this->workingDataSet);
    iftDestroyMatrix(&this->DICEimportance);
    iftDestroyMatrix(&this->kernelimportance);
}

void Projection::closeEvent(QCloseEvent *event)
{
    emit(projectionClosed());
    event->accept();
}

void Projection::keyPressEvent(QKeyEvent *e) {
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}

void Projection::on_pbProject_clicked()
{
    qDebug();

    clearAllData();

    if (this->project_type == PROJECT_KERNELS){
        ProjectKernels();
    } else if (this->project_type == PROJECT_PATCHES){
        ProjectPatches();
    }
}

void Projection::on_pbSaveProjection_clicked()
{
    qDebug();

    QString save_proj_path = QFileDialog::getSaveFileName(this,tr("Saving Projection"),QDir::currentPath(),tr("PNG (*.png) ;; JPG (*.jpg)"));

    if (save_proj_path == "")
        return;

    QGraphicsScene *scene = ui->projGraphicsView->scene();

    scene->clearSelection(); // Unselect samples as they render to the file
    scene->setSceneRect(scene->itemsBoundingRect()); // Re-shrink the scene to it's bounding contents
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
    image.fill(Qt::white); // White background

    QPainter painter(&image);
    scene->render(&painter);
    image.save(save_proj_path);
}

void Projection::on_hsPerplexity_valueChanged(int i)
{
    qDebug();
    ui->sbPerplexity->setValue(i);
}

void Projection::on_hsNumIterTSNE_valueChanged(int i)
{
    qDebug();
    ui->sbNumIterTSNE->setValue(i);
}

void Projection::on_sbPerplexity_valueChanged(int i)
{
    qDebug();
    ui->hsPerplexity->setValue(i);
}

void Projection::on_sbNumIterTSNE_valueChanged(int i)
{
    qDebug();
    ui->hsNumIterTSNE->setValue(i);
}

void Projection::on_cbNodeColorByRelevance_currentTextChanged(QString text)
{
    this->currentNodeColorOption = text;

    /* update scene and paint nodes */
    ui->projGraphicsView->computeSceneScalingFactor();
    projGraphicsScene->setSceneRect(0,0,sceneScalingFactor,sceneScalingFactor);
    projGraphicsScene->update();
    updateSampleTooltip();
    ui->projGraphicsView->paintNodes();
}

void Projection::on_cbNodeType_currentTextChanged(QString text)
{
    qDebug();
    Q_UNUSED(text);

    if(hashSampleId2GraphNodeRef.size() <= 0){
        return;
    }

    /* change the drawing option for each node */
    for(ulong i = 0; i < ulong(workingDataSet->nsamples); i++) {
        GraphNode* node = hashSampleId2GraphNodeRef[i];
        if(node == nullptr){
            continue;
        }
        node->drawOption = nodeTypeToDrawOption(ui->cbNodeType->currentText());
        node->mountToolTip();
    }

    /* paint the nodes acording to the chosen node color scheme */
    ui->projGraphicsView->paintNodes();
    projGraphicsScene->update();
}

void Projection::on_cbFilterClass_currentIndexChanged(int i)
{
    updateSampleTooltip();
    if (i != 0)
        this->targetMarker = i-1;
    ui->projGraphicsView->targetClass = i-1;
    ui->projGraphicsView->paintNodes();
}

void Projection::ProjectKernels()
{
    if (this->originalDataSet != nullptr)
        iftDestroyDataSet(&this->originalDataSet);

    // Reading layer ID
    QString kernels_path = this->model_path+"conv"+QString::number(this->layer_id)+"-kernels.npy";

    // Reading Kernel matrix
    iftMatrix *kernels = iftReadMatrix(kernels_path.toUtf8().data());

    // Transforming matrix in dataset
    iftMatrix *kernelsT = iftTransposeMatrix(kernels);
    iftDestroyMatrix(&kernels);
    this->originalDataSet = iftFeatureMatrixToDataSet(kernelsT);
    iftDestroyMatrix(&kernelsT);

    for (int i = 0; i < this->originalDataSet->nsamples; i++){
        this->originalDataSet->sample[i].id = i+1;
    }

    // Computing kernel's weights with single layer decoder
    this->setCursor(QCursor(Qt::WaitCursor));
    //computeKernelsPurityPerClass();
    computeKernelsImportancePerClass();
    this->setCursor(QCursor(Qt::ArrowCursor));

    // t-SNE projection
    updateDataSetInfoTextBoxes();
    tsneProjectionOnCurrentDataSet();
}

void Projection::ProjectPatches()
{
    QMessageBox msgBox;
    QStringList failed_load;

    if (this->originalDataSet != nullptr)
        iftDestroyDataSet(&this->originalDataSet);

    // Converting seeds to patches
    // --- checking if paths exist
    QDir img_dir(this->image_path);
    QDir seeds_dir(this->seeds_path);
    if (!img_dir.exists()){
        msgBox.setText(QString("Images not found. Certify the path ")+this->image_path+QString(" exists."));
        msgBox.show();
        return;
    }
    if (!seeds_dir.exists()){
        msgBox.setText(QString("Seeds not found. Certify the path ")+this->seeds_path+QString(" exists."));
        msgBox.show();
        return;
    }

    // --- discovering image extension
    QFileInfoList images_list = img_dir.entryInfoList(QStringList() << "Images" << "*",QDir::Files);
    QString ext = images_list.at(0).completeSuffix();

    // --- loading seeds
    QFileInfoList seeds_list = seeds_dir.entryInfoList(QStringList() << "Seeds" << "*.txt",QDir::Files);

    // --- initializing variables
    QString fileimage = this->image_path+seeds_list.at(0).baseName().replace("-seeds","")+"."+ext;;
    iftMImage *mimg = nullptr;
    if (ext == "mimg"){
        mimg = iftReadMImage(fileimage.toUtf8().data());
    }else{
        mimg = ReadInputMImage(fileimage.toUtf8().data());
    }
    iftImage *band = iftMImageToImage(mimg,255,0);
    iftAdjRel *A = iftFLIMAdjRelFromKernel(this->flim_arch->layer[this->current_training_layer-1], iftIs3DImage(band));
    iftDestroyImage(&band);
    this->ninputchannels = mimg->m;
    iftDestroyMImage(&mimg);

    // --- computing mean and stdev
    iftFileSet *fs = iftLoadFileSetFromDir(this->seeds_path.toUtf8().data(),0);
    if (this->mean != nullptr)
        iftFree(this->mean);
    if (this->stdev != nullptr)
        iftFree(this->stdev);
    this->mean = iftAllocFloatArray(this->ninputchannels);
    this->stdev = iftAllocFloatArray(this->ninputchannels);
    StatisticsFromAllSeeds(fs,this->image_path.toUtf8().data(),A,mean,stdev,this->flim_arch->stdev_factor,ext.toUtf8().data());

    // --- extracting patches
    foreach (QFileInfo seed, seeds_list){

        fileimage = this->image_path+seed.baseName().replace("-seeds","")+"."+ext;
        if (!QFileInfo(fileimage).exists()){
            failed_load.append(seed.fileName());
            continue;
        }
        if (!seed.exists()){
            failed_load.append(seed.fileName());
            continue;
        }


        if (ext == "mimg"){
            mimg = iftReadMImage(fileimage.toUtf8().data());
        }else{
            mimg = ReadInputMImage(fileimage.toUtf8().data());
        }

        NormalizeImageByZScore(mimg, mean, stdev);

        iftLabeledSet *S = iftMReadSeeds(mimg,seed.filePath().toUtf8().data());

        iftLabeledSet *Saux = S;
        int size = 0;

        while (Saux != nullptr){
            size++;
            Saux = Saux->next;
        }

        iftDataSet *Z1 = ComputeSeedDataSet(mimg, S, A, size);

        iftMatrix **kernels = (iftMatrix **) calloc(Z1->nclasses + 1, sizeof(iftMatrix *));
        int total_nkernels  = 0;
        for (int c = 1; c <= Z1->nclasses; c++) {
            iftDataSet *Z2  = iftExtractSamplesFromClass(Z1, c);
            int nkernels    = this->flim_arch->layer[this->current_training_layer - 1].nkernels_per_marker;
            /* 0: kmeans, 1: iterated watershed and 2: OPF c clusters */
//            qDebug() << "Z2 nsamples" << Z2->nsamples;
            kernels[c]      = ComputeKernelBank(Z2, &nkernels, 0);
//            qDebug() << "nkernels[" << c << "] = " << nkernels;
            total_nkernels += nkernels;
            iftDestroyDataSet(&Z2);
        }


        iftMatrix *kernelbank = iftCreateMatrix(total_nkernels, mimg->m * A->n);

        iftMatrix *kernelbank_classes = iftCreateMatrix(total_nkernels, 1);

        int k = 0;
        for (int c = 1; c <= Z1->nclasses; c++) {
            if (kernels[c]->ncols > 0) {
                for (int col = 0; col < kernels[c]->ncols; col++, k++) {
                    iftMatrixElem(kernelbank_classes, k, 0) = c;
                    for (int row = 0; row < kernels[c]->nrows; row++) {
                        iftMatrixElem(kernelbank, k, row) = iftMatrixElem(kernels[c], col, row);
                    }
                }
            }
        }

        for (int c = 0; c <= Z1->nclasses; c++) {
            iftDestroyMatrix(&kernels[c]);
        }

        iftTransposeMatrixInPlace(kernelbank);
        iftDataSet *Z = iftFeatureMatrixToDataSet(kernelbank);

        for (k = 0; k < Z->nsamples; k++) {
            Z->sample[k].truelabel = iftMatrixElem(kernelbank_classes, k, 0);
        }

        iftDataSet *Zaux = iftMergeDataSets(this->originalDataSet,Z);

        iftFree(kernels);
        iftDestroyMImage(&mimg);
        iftDestroyLabeledSet(&S);
        iftDestroyDataSet(&Z1);
        iftDestroyDataSet(&Z);
        iftDestroyDataSet(&this->originalDataSet);

        this->originalDataSet = Zaux;
    }
    iftDestroyAdjRel(&A);
    iftAddStatus(this->originalDataSet,IFT_TRAIN);
    iftAddStatus(this->originalDataSet,IFT_SUPERVISED);
    iftSetRefData(this->originalDataSet,nullptr,IFT_REF_DATA_NULL);
    this->originalDataSet->nclasses = iftCountNumberOfClassesDataSet(this->originalDataSet);

    // Setting dataset id
    for (int i = 0; i < this->originalDataSet->nsamples; i++)
        this->originalDataSet->sample[i].id = i+1;

    // t-SNE projection
    updateDataSetInfoTextBoxes();
    tsneProjectionOnCurrentDataSet();
}

void Projection::on_pbSaveKernels_clicked()
{
    /* Transforming selected kernels into matrix */

    QMessageBox msgBox;
    if ((this->workingDataSet == nullptr) || (this->originalDataSet == nullptr)){
        msgBox.setText("Dataset not found.");
        msgBox.exec();
        return;
    }
    if ((this->mean == nullptr) || (this->stdev == nullptr)){
        msgBox.setText("Mean and Standard deviation not found. Please run the projection first.");
        msgBox.exec();
        return;
    }
    if (this->numbSamplesSelected == 0){
        msgBox.setText("No samples selected.");
        msgBox.exec();
        return;
    }

    // Saving mean and stdev
    QString out_basename = this->model_path+QString("conv%1").arg(current_training_layer);
    WriteMeanStdev(out_basename.toUtf8().data(),mean,stdev,this->ninputchannels);

    // Samples to kernels
    int nfeats = this->originalDataSet->nfeats;
    int kernel_index = 0;
    iftMatrix *kernels = iftCreateMatrix(numbSamplesSelected,nfeats);
    for (ulong i = 0; i < ulong(workingDataSet->nsamples); i++) {
        GraphNode* node = hashSampleId2GraphNodeRef[i];
        if(node == nullptr)
            continue;
        if(node->isSelected()){
            iftSample *s = node->samplePtr;
            for (int f = 0; f < nfeats; f++){
                iftMatrixElem(kernels,kernel_index,f) = s->feat[f];
            }
            kernel_index++;
        }
    }

    // Reading layer ID
    int layer_id = this->current_training_layer - 1;
    int noutput_channels = this->flim_arch->layer[layer_id].noutput_channels;
    iftMatrix* Rkernels = nullptr;
    if (kernels->ncols <= noutput_channels) {
        Rkernels = iftCopyMatrix(kernels);
    } else {
        printf("number of kernels %d, feature space dimension %d\n",kernels->ncols,kernels->nrows);
        if (kernels->ncols > kernels->nrows){
            Rkernels = SelectRelevantKernelsByPCA(kernels, noutput_channels);
            printf("\n By PCA\n");
        } else {
            Rkernels = SelectRelevantKernelsByKmeans(kernels, noutput_channels);
            printf("\n By Kmeans\n");
        }
    }

    iftDestroyMatrix(&kernels);

    // Transforming kernels into unit kernels
    iftMatrix *kernels_unit = iftUnitNormMatrix(Rkernels);
    iftDestroyMatrix(&Rkernels);

    // Saving kernels
    QString kernel_name = this->model_path+QString("/conv")+QString().number(current_training_layer)+QString("-kernels.npy");
    iftWriteMatrix(kernels_unit,kernel_name.toUtf8().data());
    iftDestroyMatrix(&kernels_unit);

    msgBox.setText("Kernels saved.");
    msgBox.exec();

    this->didSomething = true;
}

void Projection::on_pbSaveRelevance_clicked()
{
    QMessageBox msgBox;
    QFileDialog save_dialog;
    QString save_path;

    if ((ui->cbNodeColorByRelevance->currentText() == "Weights") && (kernelimportance == nullptr)) {
        msgBox.warning(this,tr("Saving Relevance"), tr("Importance weights not found."),QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if ((ui->cbNodeColorByRelevance->currentText() == "DICE") && (DICEimportance == nullptr)) {
        msgBox.warning(this,tr("Saving Relevance"), tr("DICE relevance not found."),QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    // Selecting dir to save relevance weights
    save_dialog.setLabelText(QFileDialog::Accept, "Save");
    save_dialog.setOption(QFileDialog::ShowDirsOnly,true);
    save_dialog.setFileMode(QFileDialog::Directory);
    save_dialog.setDirectory(this->model_path);

    if (!save_dialog.exec()){
        return;
    }
    save_path = save_dialog.directory().canonicalPath();

    if (!iftDirExists(save_path.toUtf8().data())){
        iftMakeDir(save_path.toUtf8().data());
    }

    iftMatrix *relevance_norm = nullptr;
    QString prefix;
    if (ui->cbNodeColorByRelevance->currentText() == "DICE"){
         relevance_norm = iftCopyMatrix(this->DICEimportance);
         prefix = QString("dice");
    } else if (ui->cbNodeColorByRelevance->currentText() == "Weights"){
         relevance_norm = iftCopyMatrix(this->kernelimportance);
         prefix = QString("importance");
    }

    // Normalizing weights at 0 and 1
    for (int c = 0; c < relevance_norm->ncols; c++){
        double max = iftMatrixMaxOneColumn(relevance_norm,c), min = iftMatrixMinOneColumn(relevance_norm,c);
        for (int r = 0; r < relevance_norm->nrows; r++){
            iftMatrixElem(relevance_norm,c,r) = (iftMatrixElem(relevance_norm,c,r) - min)/(max - min);
        }
    }

    if (this->numbSamplesSelected == 0){ // Save all samples
        for (int c = 0; c < relevance_norm->ncols; c++){
            QString filename = QString(save_path+"/"+prefix+QString::number(this->layer_id)+"-"+QString::number(c+1)+".txt");
            FILE *fp_relevance = fopen(filename.toUtf8().data(),"w");
            for (int r = 0; r < relevance_norm->nrows; r++){
                fprintf(fp_relevance,"%f ",iftMatrixElem(relevance_norm,c,r));
            }
            fprintf(fp_relevance,"\n");
            fclose(fp_relevance);
        }
    } else { // Saving only selected kernels
        for (int c = 0; c < relevance_norm->ncols; c++){
            QString filename = QString(save_path+"/"+prefix+QString::number(this->layer_id)+"-"+QString::number(c+1)+".txt");
            FILE *fp_relevance = fopen(filename.toUtf8().data(),"w");
            for (int r = 0; r < relevance_norm->nrows; r++){
                GraphNode* node = hashSampleId2GraphNodeRef[r];
                if(node == nullptr)
                    continue;
                if (node->isSelected()){
                    fprintf(fp_relevance,"%f ",iftMatrixElem(relevance_norm,c,r));
                }
            }
            fprintf(fp_relevance,"\n");
            fclose(fp_relevance);
        }
    }

    iftDestroyMatrix(&relevance_norm);


    msgBox.setText("Relevance weights saved successfully.");
    msgBox.exec();
}

void Projection::setModelPath(QString path)
{
    this->model_path = path;
}

QString Projection::getModelPath()
{
    return this->model_path;
}

void Projection::initScene()
{
    qDebug();

    if(projGraphicsScene != nullptr){
        projGraphicsScene->clear();
        delete projGraphicsScene;
    }

    projGraphicsScene = new QGraphicsScene(this);
    ui->projGraphicsView->setScene(projGraphicsScene);
}

void Projection::initGraphicsView()
{
    qDebug();

    ui->projGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->projGraphicsView->setInteractive(true);
    ui->projGraphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->projGraphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}

void Projection::clearAllData(bool clearDatasets)
{
    qDebug();

    projGraphicsScene->clear();
    projGraphicsScene->update();
    graphNodes.clear();
    graphEdges.clear();

    // clear hash tables
    hashGraphNodeRef2SampleId.clear();
    hashClassId2ClassName.clear();
    hashClassId2ClassColor.clear();

    // clear variables
    globalMinFeature = IFT_INFINITY_FLT;
    globalMaxFeature = IFT_INFINITY_FLT_NEG;
    globalMinFeatPerplexity = IFT_INFINITY_FLT;
    globalMaxFeatPerplexity = IFT_INFINITY_FLT_NEG;
    globalMinFeatMean = IFT_INFINITY_FLT;
    globalMaxFeatMean = IFT_INFINITY_FLT_NEG;
    globalMinFeatStdev= IFT_INFINITY_FLT;
    globalMaxFeatStdev = IFT_INFINITY_FLT_NEG;
    //hashSampleId2CurrentItemInList.clear();
    //hashSampleId2CurrentSelected.clear();
    hashSampleId2GraphNodeRef.clear();

    // clear datasets
    if(clearDatasets) {
      if(originalDataSet != nullptr) iftDestroyDataSet(&originalDataSet);
      if(workingDataSet != nullptr)  iftDestroyDataSet(&workingDataSet);
      fileSet = nullptr;
    }
    updateNumbSamplesShown();
}

void Projection::tsneProjectionOnCurrentDataSet()
{
    qDebug();

    if(originalDataSet == nullptr)
        return;

    double perplexity = double(ui->hsPerplexity->value());
    if(originalDataSet->nsamples - 1 < 3 * perplexity) {
        QMessageBox::warning(this, tr("Error"), tr("Perplexity too large for the number of data points"), QMessageBox::Ok);
        return;
    }

    /* set timers */
    this->threadElapsedTime.start();
    this->threadTimer->start(100);

    /* create and run thread */
    enableWidgetsByMode("duringProjection");
    threadTaskType = THREAD_TSNE_PROJECTION;
    threadProjection = new MyThread(this, threadTaskType);
    threadProjection->start(QThread::HighestPriority);
    connect(threadProjection, SIGNAL(projectionFinished_signal(bool)), this, SLOT(projectionFinished_slot(bool)));
    connect(threadProjection, SIGNAL(finished()), threadProjection, SLOT(deleteLater()));
    connect(ui->cbFilterClass, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbFilterClass_currentIndexChanged(int)), Qt::UniqueConnection);
}

void Projection::updateDataSetInfoTextBoxes()
{
    qDebug();

    iftDataSet *dataset = this->workingDataSet != nullptr ? this->workingDataSet : this->originalDataSet;

    ui->leDataSetInfo->setText(QString("Info: %1 samples, %2 feats, %3 classes, %4 groups, %5 train samples").
                                  arg(dataset->nsamples).
                                  arg(dataset->nfeats).
                                  arg(dataset->nclasses).
                                  arg(dataset->ngroups).
                                  arg(dataset->ntrainsamples));
}

void Projection::NormalizeImagePerBand(iftMImage *activ, iftImage *label_img)
{
    if (label_img == nullptr){
        for (int b=0; b < int(activ->m); b++){
            float Imin=IFT_INFINITY_FLT, Imax=IFT_INFINITY_FLT_NEG;
            for (int p = 0; p < int(activ->n); p++){
                if (activ->val[p][b] > Imax)
                    Imax = activ->val[p][b];
                if ((activ->val[p][b] < Imin) && (activ->val[p][b] > 0))
                    Imin = activ->val[p][b];
            }
            if (Imin < Imax){
                for (int p = 0; p < int(activ->n); p++){
                    activ->val[p][b] = (activ->val[p][b] - Imin)/(Imax-Imin);
                    if (activ->val[p][b] < 0)
                        activ->val[p][b] = 0;
                }
            }
        }
    } else {
        for (int b=0; b < int(activ->m); b++){
            float Imin=IFT_INFINITY_FLT, Imax=IFT_INFINITY_FLT_NEG;
            for (int p = 0; p < int(activ->n); p++){
                if (label_img->val[p] > 0){
                    if (activ->val[p][b] > Imax)
                        Imax = activ->val[p][b];
                    if ((activ->val[p][b] < Imin) && (activ->val[p][b] > 0))
                        Imin = activ->val[p][b];
                }
            }
            if (Imin < Imax){
                for (int p = 0; p < int(activ->n); p++){
                    if (label_img->val[p] > 0){
                        activ->val[p][b] = (activ->val[p][b] - Imin)/(Imax-Imin);
                        if (activ->val[p][b] < 0)
                            activ->val[p][b] = 0;
                    } else {
                        activ->val[p][b] = 0;
                    }
                }
            }
        }
    }
}

void Projection::computeKernelsImportancePerClass()
{
    QMessageBox msgbox;

    iftMImage *activ = nullptr;

    /* The weights for each kernels are computed based on the markers and target object.
     * For that purpose, the algorithm requires:
     * 1) Activation images for that specific layer; and
     * 2) User-drawn markers.
     * Then, we can compute the importance of each kernel for every class.
    */

    QString activation_path = this->model_path+"layer"+QString::number(this->layer_id); // getting output activations of layer X as the input of layer X+1

    // Checking number of classes
    iftFileSet *fs_seeds = iftLoadFileSetFromDir(this->seeds_path.toUtf8().data(),1);
    iftFileSet *fs_activ = iftLoadFileSetFromDir(activation_path.toUtf8().data(),1);
    int n_images = int(fs_activ->n);
    int n_channels = this->originalDataSet->nsamples;

    if (fs_seeds->n == 0){
        iftDestroyFileSet(&fs_seeds);
        iftDestroyFileSet(&fs_activ);
        msgbox.setText("No markers detected. Unable to compute kernel's importance");
        msgbox.exec();
        return;
    }

    //Creating matrix of kernel importance per class
    if (this->kernelimportance != nullptr){
        iftDestroyMatrix(&this->kernelimportance);
    }
    this->kernelimportance = iftCreateMatrix(this->hashMarkerName2MarkerColor.size(),this->originalDataSet->nsamples);

    for (int i = 0; i < n_images; i++)
    {
        activ = iftReadMImage(fs_activ->files[i]->path);

        if (this->label_path != ""){

            iftImage *label_img = nullptr;
            QDirIterator it(this->label_path, QDirIterator::Subdirectories); // load label path so the files can be serached without the extension

            while (it.hasNext()){
                QString filename = it.next();
                QFileInfo file(filename);
                if (file.isDir()) { // Check if it's a dir
                    continue;
                }
                // Checking if file has the same filename as the activ
                if (file.fileName().contains(iftFilename(fs_activ->files[i]->path,".mimg"), Qt::CaseInsensitive)) {
                    // reading gt for image i
                    label_img    = iftReadImageByExt(file.absoluteFilePath().toUtf8().data());
                    break;
                }
            }
            NormalizeImagePerBand(activ,label_img);
            iftDestroyImage(&label_img);
        } else {
            NormalizeImagePerBand(activ,nullptr);
        }

        iftLabeledSet *S = iftMReadSeeds(activ,fs_seeds->files[i]->path);

        while (S != nullptr){
            int lbl;
            int p = iftRemoveLabeledSet(&S, &lbl);
            for (int c = 0; c < this->hashMarkerName2MarkerColor.size(); c++){
                if (lbl == c){
                    for (int b=0; b < int(activ->m); b++){
                        iftMatrixElem(this->kernelimportance,c,b) += activ->val[p][b];
                    }
                } else {
                    for (int b=0; b < int(activ->m); b++){
                        iftMatrixElem(this->kernelimportance,c,b) += 1-activ->val[p][b];
                    }
                }
            }
        }

        iftDestroyMImage(&activ);
    }

    // Centralizing

    for (int c = 0; c < this->hashMarkerName2MarkerColor.size(); c++ ){
        // float wT = 0.0;
        // for (int b = 0; b < n_channels; b++)
        //     wT += iftMatrixElem(this->kernelimportance,c,b);
        // float mean_weight = wT/n_channels;
        float max_weight  = IFT_INFINITY_FLT_NEG;

        for (int b = 0; b < n_channels; b++){
      //            iftMatrixElem(this->kernelimportance,c,b) = iftMatrixElem(this->kernelimportance,c,b) - mean_weight;
	  if (fabsf(iftMatrixElem(this->kernelimportance,c,b)) > max_weight){
	    max_weight = fabsf(iftMatrixElem(this->kernelimportance,c,b));
	  }
        }

        for (int b = 0; b < n_channels; b++){
	  iftMatrixElem(this->kernelimportance,c,b) = iftMatrixElem(this->kernelimportance,c,b)/max_weight; //fabsf(iftMatrixElem(this->kernelimportance,b,target_class)/max_weight);
        }
    }

    iftDestroyFileSet(&fs_seeds);
    iftDestroyFileSet(&fs_activ);

}

void Projection::computeKernelsImportancePerClassOnMask()
{
    QMessageBox msgbox;

    iftMImage *activ = nullptr;

    /* The weights for each kernels are computed based on the markers and target object.
     * For that purpose, the algorithm requires:
     * 1) Activation images for that specific layer; and
     * 2) User-drawn markers.
     * Then, we can compute the importance of each kernel for every class.
    */

    QString activation_path = this->model_path+"layer"+QString::number(this->layer_id); // getting output activations of layer X as the input of layer X+1

    if (this->label_path == ""){
        qDebug() << "Masks not found.";
        return;
    }

    // Checking number of classes
    iftFileSet *fs_label = iftLoadFileSetFromDir(this->label_path.toUtf8().data(),1);
    iftFileSet *fs_activ = iftLoadFileSetFromDir(activation_path.toUtf8().data(),1);
    int n_images = int(fs_activ->n);
    int n_channels = this->originalDataSet->nsamples;

    if (fs_label->n == 0){
        iftDestroyFileSet(&fs_label);
        iftDestroyFileSet(&fs_activ);
        msgbox.setText("GT provided but directory is empty.");
        msgbox.exec();
        return;
    }

    // Finding out the number of classes
    int n_classes = 0;
    for (int i = 0; i < n_images; i++){
        iftImage *tmp = iftReadImageByExt(fs_label->files[i]->path);
        int c = iftMaximumValue(tmp);
        if (c > n_classes)
            n_classes = c;
    }

    //Creating matrix of kernel importance per class
    if (this->kernelimportance != nullptr){
        iftDestroyMatrix(&this->kernelimportance);
    }
    this->kernelimportance = iftCreateMatrix(n_classes+1,this->originalDataSet->nsamples); //+1 to add the background

    for (int i = 0; i < n_images; i++)
    {
        activ = iftReadMImage(fs_activ->files[i]->path);

        iftImage *gt_img = nullptr;
        QDirIterator it(this->label_path, QDirIterator::Subdirectories); // load label path so the files can be serached without the extension

        while (it.hasNext()){
            QString filename = it.next();
            QFileInfo file(filename);
            if (file.isDir()) { // Check if it's a dir
                continue;
            }
            // Checking if file has the same filename as the activ
            if (file.fileName().contains(iftFilename(fs_activ->files[i]->path,".mimg"), Qt::CaseInsensitive)) {
                // reading gt for image i
                gt_img    = iftReadImageByExt(file.absoluteFilePath().toUtf8().data());
                break;
            }
        }
        NormalizeImagePerBand(activ,gt_img);

        iftIntArray *n_voxels = iftCreateIntArray(n_classes+1);
        for (int p = 0; p < gt_img->n; p++){
            if (gt_img->val[p] == 0){
                n_voxels->val[0]++;
                for (int b=0; b < int(activ->m); b++){
                    iftMatrixElem(this->kernelimportance,0,b) += 1 - activ->val[p][b];
                }
            } else {
                for (int c = 1; c <= n_classes; c++){
                    if (gt_img->val[p] == c){
                        n_voxels->val[c]++;
                        for (int b=0; b < int(activ->m); b++){
                            iftMatrixElem(this->kernelimportance,0,b) += activ->val[p][b];
                        }
                    } else {
                        n_voxels->val[gt_img->val[p]]++;
                        for (int b=0; b < int(activ->m); b++){
                            iftMatrixElem(this->kernelimportance,0,b) += 1 - activ->val[p][b];
                        }
                    }
                }
            }
        }


        /*while (S != nullptr){
            int lbl;
            int p = iftRemoveLabeledSet(&S, &lbl);
            for (int c = 0; c < this->hashMarkerName2MarkerColor.size(); c++){
                if (lbl == c){
                    for (int b=0; b < int(activ->m); b++){
                        iftMatrixElem(this->kernelimportance,c,b) += activ->val[p][b];
                    }
                } else {
                    for (int b=0; b < int(activ->m); b++){
                        iftMatrixElem(this->kernelimportance,c,b) += 1-activ->val[p][b];
                    }
                }
            }
        }*/

        iftDestroyIntArray(&n_voxels);
        iftDestroyMImage(&activ);
    }

    // Centralizing

    for (int c = 0; c < this->hashMarkerName2MarkerColor.size(); c++ ){
        float max_weight  = IFT_INFINITY_FLT_NEG;

        for (int b = 0; b < n_channels; b++){
      //            iftMatrixElem(this->kernelimportance,c,b) = iftMatrixElem(this->kernelimportance,c,b) - mean_weight;
      if (fabsf(iftMatrixElem(this->kernelimportance,c,b)) > max_weight){
        max_weight = fabsf(iftMatrixElem(this->kernelimportance,c,b));
      }
        }

        for (int b = 0; b < n_channels; b++){
      iftMatrixElem(this->kernelimportance,c,b) = iftMatrixElem(this->kernelimportance,c,b)/max_weight; //fabsf(iftMatrixElem(this->kernelimportance,b,target_class)/max_weight);
        }
    }

    iftDestroyFileSet(&fs_label);
    iftDestroyFileSet(&fs_activ);
}

DrawingOption Projection::nodeTypeToDrawOption(QString nodeType)
{
    DrawingOption drawOption;

    if(nodeType == "Point")
        drawOption = optionPoint;
    if(nodeType == "Feature pie")
        drawOption = optionFeatPie;
    if(nodeType == "Feature histogram")
        drawOption = optionFeatHistogram;
    if(nodeType == "Number")
        drawOption = optionText;
    if(nodeType == "Image")
        drawOption = optionImage;
    if(nodeType == "Point and Edge"){
        drawOption = optionDrawingEdges;
    }
    return drawOption;
}

void Projection::setMarkersHash(QHash<QString, QColor> hash)
{
    this->hashMarkerName2MarkerColor.clear();
    this->hashMarkerName2MarkerColor = hash;
}

void Projection::setDICEImportance(iftMatrix *DICE)
{
    if (DICE == nullptr)
        this->DICEimportance = nullptr;
    else
        this->DICEimportance = iftCopyMatrix(DICE);
}

void Projection::selectSampleFromMainWindow(int sample)
{
    GraphNode* node = hashSampleId2GraphNodeRef[sample];
    if(node == nullptr)
        return;
    node->setSelected(true);
    updateNumbSamplesShown();
}

void Projection::deselectSampleFromMainWindow(int sample)
{
    GraphNode* node = hashSampleId2GraphNodeRef[sample];
    if(node == nullptr)
        return;
    node->setSelected(false);
    updateNumbSamplesShown();
}

void Projection::paintNodes(){
    ui->projGraphicsView->paintNodes();
}

QHash<QString, QColor> Projection::getMarkersHash()
{
    return this->hashMarkerName2MarkerColor;
}

void Projection::timer_slot()
{
    qDebug();

    /* print the corresponding message and start the timer again */
    QString text;
    if(threadTaskType == THREAD_TSNE_PROJECTION) {
        text = QString("Creating projection with t-SNE ... (%1)").arg(formatTime(int(threadElapsedTime.elapsed())));
    }

    ui->lblStatusBar->setText(text);
    threadTimer->start(1000);
}

QString Projection::formatTime(int miliseconds)
{
    qDebug();

    int secs = miliseconds / 1000;
    int mins = (secs / 60) % 60;
    //int hours = (secs / 3600);
    secs = secs % 60;

    return QString("%1:%2").arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
}

void Projection::updateNumbSamplesShown()
{
    qDebug();

    this->numbSamplesShown = 0;
    this->numbSamplesSelected = 0;
    iftSet *kernel_indexes = nullptr; // must be dynamic as the number of selected kernels is arbritary

    if(workingDataSet != nullptr) {
        for (ulong i = 0; i < ulong(workingDataSet->nsamples); i++) {
            GraphNode* node = hashSampleId2GraphNodeRef[i];
            if(node == nullptr)
                continue;
            if(node->isVisible())
                numbSamplesShown++;
            if(node->isSelected()){
                numbSamplesSelected++;
                iftInsertSet(&kernel_indexes,node->samplePtr->id);
            }
        }
    }

    ui->leNumbSamplesShown->setText(QString("Shown samples: %1").arg(numbSamplesShown) + QString(", currently selected samples: %1").arg(this->numbSamplesSelected));

    // emit signal to select kernels in the main window
    if (this->project_type == PROJECT_KERNELS)
        emit(kernelsSelected(kernel_indexes));
    iftDestroySet(&kernel_indexes);

}

void Projection::projectionFinished_slot(bool tsneExecuted)
{
    qDebug();

    ui->projGraphicsView->computeSceneScalingFactor();
    //createHashBetweenClassesAndColors();

    /* create nodes that will be added to the scene */
    qreal rx = qreal(pointDefaultRx);
    qreal ry = qreal(pointDefaultRy);
    qreal factor = sceneScalingFactor - rx*4;

    for (int i = 0; i < workingDataSet->nsamples; i++) {
        /* create the graph node */
        GraphNode *node = new GraphNode(&workingDataSet->sample[i], workingDataSet->nfeats);
        connect(node->signalhandler,SIGNAL(doubleClicked(int)),this,SLOT(on_doubleClicked(int)), Qt::UniqueConnection);

        /* sample position */
        qreal x = (workingDataSet->projection->val[i*2])*factor  + rx*2;
        qreal y = (workingDataSet->projection->val[i*2 + 1])*factor + ry*2;
        node->setPos(x, y);

        /* set other variables */
        node->setFlag(QGraphicsItem::ItemIsSelectable);
        node->numberTimesChecked = ulong(workingDataSet->sample[i].numberTimesChecked);

        /* initialize sample status */
        bool isSupervised = false, isLabelPropagated = false;
        if(iftHasSampleStatus(workingDataSet->sample[i], IFT_SUPERVISED)) isSupervised = true;
        if(iftHasSampleStatus(workingDataSet->sample[i], IFT_LABELPROPAGATED)) isLabelPropagated = true;
        iftSetSampleStatus(&workingDataSet->sample[i], IFT_TRAIN); // all the samples must have the IFT_TRAIN status
        if(isSupervised) iftAddSampleStatus(&workingDataSet->sample[i], IFT_SUPERVISED);
        if(isLabelPropagated) iftAddSampleStatus(&workingDataSet->sample[i], IFT_LABELPROPAGATED);

        /* insert the node in the vector of nodes (graph) and create corresponding hash tables */
        graphNodes.push_back(node);
        hashSampleId2GraphNodeRef.insert(ulong(i), node); // the sample's ID does not correspond to the dataset index. It must be used only as an index for the reference data
        hashGraphNodeRef2SampleId.insert(node, ulong(i)); // the sample's ID does not correspond to the dataset index. It must be used only as an index for the reference data


        /* compute min and max values for node data */
        globalMinFeature = iftMin(globalMinFeature, iftMinFloatArray(node->samplePtr->feat, workingDataSet->nfeats));
        globalMaxFeature = iftMax(globalMaxFeature, iftMaxFloatArray(node->samplePtr->feat, workingDataSet->nfeats));
        globalMinFeatPerplexity = iftMin(globalMinFeatPerplexity, node->featPerplexity);
        globalMaxFeatPerplexity = iftMax(globalMaxFeatPerplexity, node->featPerplexity);
        globalMinFeatMean = iftMin(globalMinFeatMean, node->featMean);
        globalMaxFeatMean = iftMax(globalMaxFeatMean, node->featMean);
        globalMinFeatStdev = iftMin(globalMinFeatStdev, node->featStdev);
        globalMaxFeatStdev = iftMax(globalMaxFeatStdev, node->featStdev);
    }

    /* update info for each sample */
    for (ulong i = 0; i < ulong(workingDataSet->nsamples); ++i) {
        GraphNode* node = hashSampleId2GraphNodeRef[i];
        if(node == nullptr){
            continue;
        }
        node->setMaxGlobalFeatPerplexity(globalMaxFeatPerplexity);
        node->setMinGlobalFeatPerplexity(globalMinFeatPerplexity);
        node->setMaxGlobalFeatMean(globalMaxFeatMean);
        node->setMinGlobalFeatMean(globalMinFeatMean);
        node->setMaxGlobalFeatStdev(globalMaxFeatStdev);
        node->setMinGlobalFeatStdev(globalMinFeatStdev);
        node->setDisplayNodeDataAsPointBorder(true);
        //node->setCurrentNodeDataMode(ui->comboBoxNodeDataFilter->currentText());
    }

    /* add projected items to the scene */
    for (int i = 0; i < graphNodes.size(); ++i){
        //unsupervised
        if(!(iftHasSampleStatus(workingDataSet->sample[i], IFT_SUPERVISED))){
            graphNodes.at(i)->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            projGraphicsScene->addItem(graphNodes.at(i));
            graphNodes.at(i)->Z = 0.0;
            graphNodes.at(i)->setZValue(0.0);
        }
    //for (int i = 0; i < graphNodes.size(); ++i)
        //supervised
        if(iftHasSampleStatus(workingDataSet->sample[i], IFT_SUPERVISED)){
            graphNodes.at(i)->setFlag(QGraphicsItem::ItemIgnoresTransformations);
            projGraphicsScene->addItem(graphNodes.at(i));
            graphNodes.at(i)->Z = 0.5;
            graphNodes.at(i)->setZValue(0.5);
        }
    }

    threadElapsedTime.start();
    threadTimer->start(100);

    /* update some ui elements */
    enableWidgetsByMode("afterProjection");
    createComboBoxForClassFilters();
    //setValuesForNodeDataFilterSpinBoxes();

    if(tsneExecuted)
        ui->lblStatusBar->setText(QString("t-SNE projection created ... (Elapsed time: %1)").arg(formatTime(int(threadElapsedTime.elapsed()))));
    else
        ui->lblStatusBar->setText(QString("t-SNE projection loaded from the given dataset"));
    threadTimer->stop();

    updateDataSetInfoTextBoxes();
    updateNumbSamplesShown();
    updateSampleTooltip();

    /* update scene and paint nodes */
    projGraphicsScene->setSceneRect(0,0,sceneScalingFactor,sceneScalingFactor);
    projGraphicsScene->update();
    ui->projGraphicsView->paintNodes();
}

void Projection::on_doubleClicked(int index)
{
    qDebug();
    emit(kernelDoubleClicked(index));
}

void Projection::createHashBetweenClassesAndColors()
{
    qDebug();

    if(workingDataSet == nullptr)
        return;

    hashClassId2ClassColor.clear();
    hashClassId2ClassName.clear();

    int nclasses = workingDataSet->nclasses;

    if(nclasses > 0) {

        /* create a color table */
        iftColorTable *ctb = iftCategoricalColorTable(nclasses);

        /* create new colors according to the number of classes */
        for (int i = 0; i < nclasses; i++) {
            iftColor RGB = iftYCbCrtoRGB(ctb->color[i], 255);
            QColor color;
            color.setRgb(RGB.val[0], RGB.val[1], RGB.val[2]);
            color.setAlpha(200);
            hashClassId2ClassColor.insert(i+1, QColor(color));
            QString trueLabelName = tr("Class ") + QString::number(i+1);
            hashClassId2ClassName.insert(i+1, trueLabelName);
        }
        iftDestroyColorTable(&ctb);
    }
}

void Projection::createComboBoxForClassFilters()
{
    qDebug();

    /* marker class filter */
    ui->cbFilterClass->clear();

    // filter by all markers
    ui->cbFilterClass->addItem(QString("All"));

    // background marker

    QHash<QString,QColor>::iterator it;
    for (it = this->hashMarkerName2MarkerColor.begin();
         it != this->hashMarkerName2MarkerColor.end();
         it++){
        QPixmap pix(15,15);
        pix.fill(it.value());
        ui->cbFilterClass->addItem(pix,it.key());
    }
    ui->cbFilterClass->model()->sort(0);
    ui->cbFilterClass->setCurrentIndex(0);
}

void Projection::updateSampleTooltip()
{
    qDebug();

    if (this->workingDataSet == nullptr)
        return;

    for (ulong i = 0; i < ulong(this->workingDataSet->nsamples); i++) {
        GraphNode* node = hashSampleId2GraphNodeRef[i];
        if(node == nullptr){
            continue;
        }

        if (currentNodeColorOption == "Weights"){
            if (this->kernelimportance != nullptr){
                workingDataSet->sample[i].weight = iftMatrixElem(this->kernelimportance,this->targetMarker,int(i));
                node->featWeight = workingDataSet->sample[i].weight;
            }
            node->sampleName = QString("id: %1").arg(workingDataSet->sample[i].id);
        }
        else if(currentNodeColorOption == "DICE"){
            if (this->DICEimportance != nullptr){
                workingDataSet->sample[i].weight = iftMatrixElem(this->DICEimportance,0,int(i));
                node->featWeight = workingDataSet->sample[i].weight;
            }
            node->sampleName = QString("id: %1").arg(workingDataSet->sample[i].id);
        }

        node->drawOption = nodeTypeToDrawOption(ui->cbNodeType->currentText());
        node->mountToolTip();
    }
}

void Projection::enableWidgetsByMode(QString mode)
{
    if (mode == "beforeProjection")
    {
        ui->projGraphicsView->setEnabled(false);
        ui->pbProject->setEnabled(true);
        ui->hsPerplexity->setEnabled(true);
        ui->hsNumIterTSNE->setEnabled(true);
        ui->sbPerplexity->setEnabled(true);
        ui->sbNumIterTSNE->setEnabled(true);
        ui->cbNodeColorByRelevance->setEnabled(false);
        ui->cbNodeType->setEnabled(false);
        ui->cbFilterClass->setEnabled(false);
        ui->pbSaveRelevance->setEnabled(false);
        ui->pbSaveProjection->setEnabled(false);
    } else if (mode == "duringProjection")
    {
        ui->projGraphicsView->setEnabled(false);
        ui->pbProject->setEnabled(false);
        ui->hsPerplexity->setEnabled(false);
        ui->hsNumIterTSNE->setEnabled(false);
        ui->sbPerplexity->setEnabled(false);
        ui->sbNumIterTSNE->setEnabled(false);
        ui->cbNodeColorByRelevance->setEnabled(false);
        ui->cbNodeType->setEnabled(false);
        ui->cbFilterClass->setEnabled(false);
        ui->pbSaveRelevance->setEnabled(false);
        ui->pbSaveProjection->setEnabled(false);
    } else if (mode == "afterProjection")
    {
        ui->projGraphicsView->setEnabled(true);
        ui->pbProject->setEnabled(true);
        ui->hsPerplexity->setEnabled(true);
        ui->hsNumIterTSNE->setEnabled(true);
        ui->sbPerplexity->setEnabled(true);
        ui->sbNumIterTSNE->setEnabled(true);
        ui->cbNodeColorByRelevance->setEnabled(true);
        ui->cbNodeType->setEnabled(true);
        ui->cbFilterClass->setEnabled(true);
        ui->pbSaveRelevance->setEnabled(true);
        ui->pbSaveProjection->setEnabled(true);
    }
}

void Projection::createConnections()
{
    connect(ui->pbProject, SIGNAL(clicked()), this, SLOT(on_pbProject_clicked()), Qt::UniqueConnection);
    connect(ui->hsPerplexity, SIGNAL(valueChanged(int)), this, SLOT(on_hsPerplexity_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->hsNumIterTSNE, SIGNAL(valueChanged(int)), this, SLOT(on_hsNumIterTSNE_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->sbPerplexity, SIGNAL(valueChanged(int)), this, SLOT(on_sbPerplexity_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->sbNumIterTSNE, SIGNAL(valueChanged(int)), this, SLOT(on_sbNumIterTSNE_valueChanged(int)), Qt::UniqueConnection);
    connect(ui->pbSaveProjection, SIGNAL(clicked()), this, SLOT(on_pbSaveProjection_clicked()), Qt::UniqueConnection);

    connect(ui->cbNodeColorByRelevance, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbNodeColorByRelevance_currentTextChanged(QString)), Qt::UniqueConnection);
    connect(ui->cbNodeType, SIGNAL(currentTextChanged(QString)), this, SLOT(on_cbNodeType_currentTextChanged(QString)), Qt::UniqueConnection);

    if (this->project_type == PROJECT_PATCHES){
        connect(ui->pbSaveKernels, SIGNAL(clicked()), this, SLOT(on_pbSaveKernels_clicked()), Qt::UniqueConnection);
    } else if (this->project_type == PROJECT_KERNELS){
        connect(ui->pbSaveRelevance, SIGNAL(clicked()), this, SLOT(on_pbSaveRelevance_clicked()), Qt::UniqueConnection);
    }
}

/*//////////////////////////////////////////// MYTHREAD /////////////////////////////////////////////*/
// WARNING: The code that is being executed by thread must not modify graphical components (ui)
// All the processing using the ui pointer (to modify its components) must be performed inside the MainWindow class.
// This also includes calling functions of MainWindows that modify the ui content. They must not be called
/*///////////////////////////////////////////////////////////////////////////////////////////////////*/

MyThread::MyThread(Projection* projection, ThreadTaskType taskType)
{
    qDebug();

    this->projection = projection;
    this->taskType = taskType;
}

void MyThread::run()
{
    qDebug();

    bool tsneExecuted = false;
    if(taskType == THREAD_TSNE_PROJECTION) {
        // perform t-SNE projection or copy a previous projection (if exists)
        if(projection->originalDataSet->projection != nullptr) {
            projection->workingDataSet = iftCopyDataSet(projection->originalDataSet, true);
            tsneExecuted = false;
        } else {
            double perplexity = double(projection->ui->hsPerplexity->value());
            size_t maxIter = size_t(projection->ui->hsNumIterTSNE->value());
            if(projection->workingDataSet != nullptr)
                iftDestroyDataSet(&(projection->workingDataSet));
            projection->workingDataSet = iftDimReductionByTSNE(projection->originalDataSet, 2, perplexity, maxIter);
            // we must copy originalDataSet after the projection because it contains both the original and the projected data
            iftDestroyDataSet(&projection->workingDataSet);
            projection->workingDataSet = iftCopyDataSet(projection->originalDataSet,true);
            tsneExecuted = true;
        }

        emit projectionFinished_signal(tsneExecuted);
    }
}
