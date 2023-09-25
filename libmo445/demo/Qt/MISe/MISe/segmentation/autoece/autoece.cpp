#include "autoece.h"
#include "ui_autoece.h"
#include <QTime>
#include <QVector>

AutoECE::AutoECE(MainWindow *parent, View *view) :
    Segmentation(parent, view),
    ui(new Ui::AutoECE)
{
    ui->setupUi(this);

    _name = "Automatic ECE";
    annotationVisible = false;

    mean_intensity_superspels = NULL;

    connect(ui->pbAdvancedOptions, SIGNAL(toggled(bool)), this, SLOT(toggleAdvancedOptions(bool)));
    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->sliderTimeImages, SIGNAL(valueChanged(int)), this, SLOT(changeValueSliderTime(int)));
    connect(ui->teImages,SIGNAL(userTimeChanged(QTime)),this, SLOT(changeTimeUserTimerEdit(QTime)));
    connect(ui->pbViewECECurves, SIGNAL(clicked()), this, SLOT(viewECECurves()));
    connect(ui->pbBackgroundSeeds, SIGNAL(toggled(bool)), this, SLOT(toggleBackgroundSeeds(bool)));

    ui->formAdvancedOptions->hide();
    curves = nullptr;

    n_final_labels = -1;
    //ui->pbRun->setDisabled(true);
    ui->pbViewECECurves->setDisabled(true);


}

AutoECE::~AutoECE()
{
    delete ui;
}

iftImage** iftMesoSuperspels2(iftImage** imgs, iftImage** masks_meso, iftLabeledSet** background_seeds,
                             int num_init_seeds, int num_superpixels,
                             char* SUPERVOXEL_METHOD, char* SEGMENTATION_METHOD, int n_images,
                             float*** mean_intensity_superspels){

    iftImage **orig_imgs = (iftImage **) iftAlloc(n_images, sizeof(iftImage *));
    iftMImage **mimgs       = (iftMImage**) iftAlloc(n_images, sizeof(iftMImage*));
    iftImage **labels       = (iftImage**) iftAlloc(n_images, sizeof(iftImage*));

    for (int i = 0; i < n_images; i++) {

        iftImage *masked = iftMask(imgs[i], masks_meso[i]);
        orig_imgs[i] = imgs[i];//iftDestroyImage(&imgs[i]);
        imgs[i] = masked;
        mimgs[i] = iftImageToMImage(imgs[i], GRAY_CSPACE);
    }

    if(!strcmp (SUPERVOXEL_METHOD, "DISF")){
        iftAdjRel *A_DISF;
        A_DISF = iftSpheric(sqrtf(1.0));
        labels[0] = iftDISF(mimgs[0], A_DISF, num_init_seeds, num_superpixels, masks_meso[0]); // DISF result

    }
    else if(!strcmp (SUPERVOXEL_METHOD, "SICLE")){

        iftSICLEArgs *sargs;
        iftSICLE *sicle;

        sargs = iftCreateSICLEArgs();
        sargs->n0 = num_init_seeds;
        sargs->nf = num_superpixels;
        sicle = iftCreateSICLE(imgs[0], NULL, masks_meso[0]);
        labels[0] = iftRunSICLE(sicle, sargs);

    }
    // calcular o centroide dos superpixels da imagem 1

    iftLabeledSet **centroids = (iftLabeledSet**) iftAlloc(n_images, sizeof(iftLabeledSet*));
    centroids[0] = iftGeodesicCenters(labels[0]);

    (*mean_intensity_superspels) = (float**) iftAlloc(n_images, sizeof(float*));
    float*  mean_background_imgs      = iftAllocFloatArray(n_images);

    (*mean_intensity_superspels)[0] = iftMesoMeanIntensityCurves(imgs[0], labels[0], num_superpixels);
    mean_background_imgs[0]      = iftMesoMeanBackgroundSeeds(orig_imgs[0],background_seeds[0]);

    for (int i = 1; i < n_images; i++) {
        printf("Processing %d-th image...\n", i+1);
        labels[i]                   = iftMesoProjectSeedsOnNextImage(imgs[i-1], imgs[i], masks_meso[i], mimgs[i], centroids[i-1], SEGMENTATION_METHOD);
        centroids[i]                = iftGeodesicCenters(labels[i]);

        (*mean_intensity_superspels)[i] = iftMesoMeanIntensityCurves(imgs[i],labels[i],num_superpixels);
        mean_background_imgs[i]     = iftMesoMeanBackgroundSeeds(orig_imgs[i],background_seeds[i]);

        for(int j=0;j<num_superpixels;j++){
            (*mean_intensity_superspels)[i][j] -= mean_background_imgs[i];
        }
    }

return labels;
}

QList<iftImage*> AutoECE::generateLabel()
{
    int tsize = view->getImageSequence()->tsize;

    float dilation_ratio = ui->sbDilation->value();
    int   initial_num_seeds_superpixels = ui->sbInitialNSuperpixels->value();
    int   final_num_superpixels = ui->sbFinalNSuperpixels->value();

    iftLabeledSet **bg_seeds = (iftLabeledSet **)iftAlloc(tsize, sizeof (iftLabeledSet *));
    const iftImage *markers = view->annotation->getMarkers();


//    QList<char const *> markers_path = {"background_1.txt","background_2.txt",
//                                  "background_3.txt","background_4.txt",
//                                  "background_5.txt","background_6.txt",
//                                  "background_7.txt"};
//    int k = 0;
//    for (char const*mpth : markers_path) {
//        bg_seeds[k++] = iftReadSeeds(markers, "/data_lids/home/taylla/CLionProjects/meso/cmake-build-debug/background/%s", mpth);
//    }
    for (int i = 0; i < markers->n; i++) {
        if (markers->val[i] >= 0)  {
            for (int t = 0; t < tsize; t++) {
                iftInsertLabeledSet(&(bg_seeds[t]), i, 1);
            }
        }
    }


    const iftImageSequence *imgseq = view->getImageSequence();
    const iftImageSequence *labelseq = view->getLabelSequence();
    iftImage** imgs     = (iftImage**) iftAlloc(tsize, sizeof(iftImage*));
    iftImage** labels   = (iftImage**) iftAlloc(tsize, sizeof(iftImage*));
    iftImage** labels_ECE = (iftImage**) iftAlloc(tsize, sizeof(iftImage*));

    //QList<const char *> imgpths = {"0001.nii.gz","0002.nii.gz","0003.nii.gz",
    //                               "0004_corrigido.nii.gz","0005.nii.gz",
    //                               "0006.nii.gz","0007.nii.gz"};

    //QList<const char *> maskspth = {"f_mask_1.nii.gz","f_mask_2.nii.gz",
    //                                "f_mask_3.nii.gz","f_mask_4.nii.gz",
    //                                "f_mask_5.nii.gz","f_mask_6.nii.gz",
    //                                "f_mask_7.nii.gz"};

    for (int t = 0; t < tsize; t++) {
        imgs[t]   = iftExtractVolumeFromImageSequence(imgseq, t);
        labels[t] = iftExtractVolumeFromImageSequence(labelseq, t);
        //imgs[t]   = iftReadImageByExt("/data_lids/home/taylla/CLionProjects/meso/cmake-build-debug/img/%s", imgpths[t]); //iftExtractVolumeFromImageSequence(imgseq, t);
        //labels[t]   = iftReadImageByExt("/data_lids/home/taylla/CLionProjects/meso/cmake-build-debug/masks/%s", maskspth[t]);

    }
    QString SUPERVOXEL_METHOD = ui->cBoxSupervoxel->currentText();
    QString SEGMENTATION_METHOD = ui->cBoxSegmentation->currentText();

    if(!(ui->cBoxRun->currentText().compare("All"))){
        mean_intensity_superspels = NULL;
        final_mean_intensity_superspels=NULL;

        labels_ECE = iftMesoAutoECE2(imgs, labels,
                                    dilation_ratio,
                                    initial_num_seeds_superpixels,
                                    final_num_superpixels,
                                    tsize, bg_seeds,
                                    time_sec.data(), SUPERVOXEL_METHOD.toLocal8Bit().data(),
                                    SEGMENTATION_METHOD.toLocal8Bit().data(),
                                    &final_mean_intensity_superspels);

        n_final_labels = iftCountUniqueIntElems(labels_ECE[0]->val,labels_ECE[0]->n) - 1;

        for (int i = 0; i < tsize; i++) {

                iftDestroyLabeledSet(&bg_seeds[i]);
            }

            iftFree(bg_seeds);

    }
    else if(!(ui->cBoxRun->currentText().compare("Dilatation"))){

        labels_ECE = iftMesoMaskDilation(imgs, labels, dilation_ratio, tsize);

    }
    else if(!(ui->cBoxRun->currentText().compare("Superspels"))){
        mean_intensity_superspels = NULL;

        labels_ECE = iftMesoSuperspels2(imgs, labels, bg_seeds, initial_num_seeds_superpixels,
                                     final_num_superpixels,SUPERVOXEL_METHOD.toLocal8Bit().data(),
                                     SEGMENTATION_METHOD.toLocal8Bit().data(), tsize, &mean_intensity_superspels);
        n_final_labels = iftCountUniqueIntElems(labels_ECE[0]->val,labels_ECE[0]->n) - 1;

        for (int i = 0; i < tsize; i++) {

                iftDestroyLabeledSet(&bg_seeds[i]);
            }

            iftFree(bg_seeds);
    }

    else if(!(ui->cBoxRun->currentText().compare("Select Labels"))){
        final_mean_intensity_superspels = NULL;
        labels_ECE = iftMesoSelectLabels(imgs, labels, mean_intensity_superspels, tsize, final_num_superpixels,
                                         time_sec.data(), &final_mean_intensity_superspels);
        n_final_labels = iftCountUniqueIntElems(labels_ECE[0]->val,labels_ECE[0]->n) - 1;

    }
    QList<iftImage*> labels_finais;

    for(int t = 0; t < tsize; t++){
        labels_finais.append(labels_ECE[t]);
    }



    for(int t = 0; t < tsize; t++){
        iftDestroyImage(&imgs[t]);
        iftDestroyImage(&labels[t]);
        //iftDestroyImage(&labels_ECE[t]);

    }
    iftFree(labels_ECE);
    iftFree(imgs);
    iftFree(labels);

    return labels_finais;
}

void AutoECE::notifyImageUpdate()
{
    int tsize = view->getImageSequence()->tsize;
    this->time_sec.clear();
    time_sec = {0,40,80,120,270,540,810};
    //time_sec = {0,270,540,810};

    //for(int i=0; i<tsize;i++){
    //    this->time_sec.append(0);
    //}
    ui->sliderTimeImages->setMaximum(tsize);
}

void AutoECE::toggleAdvancedOptions(bool checked)
{
    ui->formAdvancedOptions->setVisible(checked);
}

void AutoECE::changeValueSliderTime(int time)
{
    int time_min = time_sec[time-1]/60;
    int time_final_sec = time_sec[time-1]%60;
    QTime time_final(0,time_min, time_final_sec);// = time_sec[time]
    ui->teImages->setTime(time_final);
    ui->lbCurrentImageIndex->setText(QString::number(time));

}

void AutoECE::changeTimeUserTimerEdit(QTime time)
{
    int time_changed_in_sec = time.second()+time.minute()*60;
    int current_time_index = ui->sliderTimeImages->value() - 1;
    time_sec[current_time_index] = time_changed_in_sec;
}

void AutoECE::viewECECurves()
{


    if(n_final_labels>0) {

        if(!(ui->cBoxRun->currentText().compare("Superspels"))){
            curves = CurvesView::getInstance();
            curves->initializeParameters(time_sec,
                                         mean_intensity_superspels,
                                         n_final_labels);
           // curves->setWindowFlags(Qt::WindowStaysOnTopHint);
            curves->show();
        }
        else{
            curves = CurvesView::getInstance();
            curves->initializeParameters(time_sec,
                                         final_mean_intensity_superspels,
                                         n_final_labels);
           // curves->setWindowFlags(Qt::WindowStaysOnTopHint);
            curves->show();
        }



    }
}

void AutoECE::toggleBackgroundSeeds(bool checked)
{
    parent->setAnnotationVisibility(checked);
    if(checked==false){
        ui->pbRun->setEnabled(true);
        ui->pbViewECECurves->setEnabled(true);
    }
}



void AutoECE::on_pushButton_clicked()
{
    if (!view->isLabelEmpty()){
        parent->destroyObjectTableActions();
    }

    const iftImageSequence *label = view->getLabelSequence();

    QString roi_path = QFileDialog::getOpenFileName((QWidget*) parent,
                                                             tr("Open image ROI"));


    iftImageSequence *roi = iftReadImageSequence(roi_path.toUtf8().data());

    int max = ift4DMaximumValue(label, 0);

    IMGSEQ_FORLOOP(label) {

        if (IMGSEQ_ITE_VAL(label, 0) == 0) {
            IMGSEQ_ITE_VAL(label, 0) = (IMGSEQ_ITE_VAL(roi, 0) != 0) * (max+1);
        }
    }

    for (int t = 0; t < roi->tsize; t++) {
        iftImage *tth_lbl = iftExtractVolumeFromImageSequence(label, t);

        iftColorTable *ctb = nullptr;
        if (labelInfo)
            ctb = labelInfo->generateColorTable();
        else {
            int max = iftMaximumValue(tth_lbl);
            ctb = iftCreateColorTable(max+1);
            // TODO remove from here
            for (int i = 0; i < ctb->ncolors; i++) {
                ctb->color[i] = MarkerInformation::defaultColor(i);
            }
            //
        }

        view->setLabel(tth_lbl, t, ctb, false);
        iftDestroyColorTable(&ctb);

        iftDestroyImage(&tth_lbl);
    }

    iftDestroyImageSequence(&roi);
    parent->updateAllGraphicalViews();
    bool useMarkerInfo = labelInfo != nullptr;
    parent->loadListOfObjects(useMarkerInfo);
    view->setRenditionLabel();
}

