#include "altis.h"
#include "../executework.h"

#include <QFuture>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <mainwindow.h>
#include <global.h>

ALTIS::ALTIS(MainWindow *parent, View *view, SemiAutomatic *semiAutomaticSeg) :
    Segmentation(parent, view),
    ui(new Ui::ALTIS),
    semiAutomaticSeg(semiAutomaticSeg)
{
    ui->setupUi(this);

    _name = "ALTIS";

    connect(ui->pbRun, SIGNAL(clicked()), this, SLOT(execute()));
    connect(ui->pbCorrect, SIGNAL(clicked()), this, SLOT(correctSegmentation()));
    connect(ui->pbAdvancedOptions, SIGNAL(clicked(bool)), this, SLOT(toggleAdvancedOptions(bool)));
    connect(ui->cbPleuralCorrection, SIGNAL(stateChanged(int)), this, SLOT(togglePleuralCorretion(int)));
    ui->formAdvancedOptions->hide();
    enhanced = nullptr;
}

ALTIS::~ALTIS()
{
    delete ui;
}

void ALTIS::notifyImageUpdate()
{
    forest = nullptr;
    seeds = nullptr;
    ui->pbCorrect->setEnabled(false);
}

void ALTIS::correctSegmentation()
{
    bool will_correct_annotation = true;

    if (ui->cbPleuralCorrection->isChecked()) {
        QMessageBox::StandardButton button_clicked
                = QMessageBox::warning(this,
                                       "Pleural anomaly correction enabled",
                                       "The annotation correction will reflect the "
                                       "segmentation without the improvement provided "
                                       "by the pleural anomaly correction. "
                                       "Do you still want to proceed?",
                                       QMessageBox::Ok | QMessageBox::Cancel);
        will_correct_annotation = button_clicked == QMessageBox::Ok;
    }

    if (will_correct_annotation) {
        semiAutomaticSeg->loadForest(forest, seeds);
        //TODO pass MarkerInfo as a parameter to setLabel
        view->annotation->setMarkerName(1, "Left Lung");
        view->annotation->setMarkerName(2, "Right Lung");
        view->annotation->setMarkerName(3, "Trachea-bronchi");
        forest = nullptr;
        parent->showSegmentationModule(semiAutomaticSeg);
        ui->pbCorrect->setEnabled(false);
    }
}

void ALTIS::toggleAdvancedOptions(bool checked)
{
    ui->formAdvancedOptions->setVisible(checked);
}

void ALTIS::togglePleuralCorretion(int state)
{
    bool checked = state == Qt::CheckState::Checked;
    ui->lbRadiusAnomaly->setEnabled(checked);
    ui->lbGeodesicAnomaly->setEnabled(checked);
    ui->sbGeodesicAnomaly->setEnabled(checked);
    ui->sbRadiusAnomaly->setEnabled(checked);
}

iftImage *ALTIS::calculateImageGradient()
{

    if (view->getGradient() == nullptr)
        parent->slotCalculateGradient();

    const iftMImage *mimg = view->getGradient();

    iftAdjRel *B = iftSpheric(1.0);

    iftImage *grad = MImageGradient(mimg, B);

    iftDestroyAdjRel(&B);

//    iftAdjRel *B   = iftSpheric(1.0); // 1.732
//    iftImage *grad = iftMImageBasins(enhanced,B);
//    iftDestroyAdjRel(&B);

    return grad;
}

/* Geodesic-based Pleural Anomaly Detection */
iftImage *iftPleuralAnomalyDetectionByGeodesicDistance(iftImage *segm, float radius, float geo_th);
iftImage *iftGeodesicDistanceInPleura(iftImage *segm, iftImage *lung, float geo_th);
iftImage *iftRestrictedCloseBin(iftImage *bin, float radius, iftImage *forbidden);
iftImage *iftRemoveMediastinumForest(iftImage *mediastinum_bin, iftImage *closed, iftImage *close_root);
iftImage *iftSortComponentsByAreaVolume(iftImage *label_map);
/*************************************************************/

//#define DEBUG
//#define SHOW_STEP_BY_STEP

iftImage *ALTIS::generateLabel() {
    adjRelRadius          = ui->sbAdjRel->value();
    closingRadius         = ui->sbClosing->value();
    dilationSquaredRadius = ui->sbDilation->value();
    erosionSquaredRadius  = ui->sbErosion->value();
    otsuThreshold         = ui->sbOtsu->value();
    tracheaThreshold      = ui->sbTracheaThreshold->value();
    gaussianFilterApplied = ui->cbGaussianFilter->isChecked();
    removeNoiseApplied    = ui->cbRemoveNoise->isChecked();

    char *formatted         = NULL;
    iftImage *scn = iftCopyImage(view->getImage()), *grad = NULL, *voi = NULL, *segm = NULL;//, *closed_segm = NULL;
    iftAdjRel *A = NULL;
    timer *t1,*t2;

    // Checking if input image is isotropic
    if (!iftIsIsotropic(scn)){
        puts("\nWarning in iftALTIS");
        puts("   Input Image is must be Isotropic (dx = dy = dz).");
        printf("   Voxel size of input image: (%.2f, %.2f, %.2f)\n",scn->dx,scn->dy,scn->dz);
        puts("   The result may not be as expected.");
    }


    // Creating Adjacency Realtion
    A = iftSpheric((adjRelRadius*scn->dx/STANDARD_VOXEL_SIZE)<1?1:(adjRelRadius*scn->dx/STANDARD_VOXEL_SIZE));

    // Image Pre-processing
    iftGaussianFilter(gaussianFilterApplied, &scn);
    iftRemoveNoise(removeNoiseApplied, &scn, A);

    updateProgress(0.1, "Extracting volume of interest.");

    /* --------------------------------------------   Start ALTIS   -------------------------------------------- */
    t1 = iftTic();

    // Respiratory System Extraction
    iftSet *S = NULL;
    #ifdef SHOW_STEP_BY_STEP
        puts("");
        puts("Extracting Volume of Interest");
    #endif
    voi   = iftExtractVolumeOfInterest(scn,A,&grad,&S);

    updateProgress(0.3, "Segmenting respiratory system.");

    #ifdef DEBUG
        sprintf(filename,"%s/DEBUG/voi/%s",img_parent_folder,file);
        iftWriteImageByExt(voi,filename);
        sprintf(filename,"%s/DEBUG/grad/%s",img_parent_folder,file);
        iftWriteImageByExt(grad,filename);
    #endif

    // Lungs and Trachea Segmentation
    #ifdef SHOW_STEP_BY_STEP
        puts("");
        puts("Segmenting Respiratory System");
    #endif
    segm = iftSegmentRespiratorySystem(grad, voi, S, A, scn);

    if (ui->cbPleuralCorrection->isChecked()) {
        updateProgress(0.55, "Performing Pleural Correction by Geodesic Distance.");
        double radius = ui->sbRadiusAnomaly->value();
        double geodesic_threshold= ui->sbGeodesicAnomaly->value();
        segm = iftPleuralAnomalyDetectionByGeodesicDistance(segm, radius, geodesic_threshold);
    }

    updateProgress(0.6);

    //closed_segm = iftPleuralAnomalyDetectionByMathMorphology(segm);
    //iftDestroyImage(&segm);
    //segm = closed_segm;
    //closed_segm = NULL;

    t2 = iftToc();
    /* --------------------------------------------   End ALTIS   -------------------------------------------- */

    // Show computational time
    puts("\nDone...");
    printf("Total Thorax Segmentation took ");
    formatted = iftFormattedTime(iftCompTime(t1, t2));
    puts(formatted);
    iftFree(formatted);

    #ifdef SHOW_STEP_BY_STEP
        puts("");
        puts("Exporting Segmentation");
    #endif

    #ifdef SHOW_STEP_BY_STEP
        puts("");
        puts("Deallocating Memory Space");
        puts("");
    #endif
    // Deallocation Memory
    iftDestroyImage(&voi);
    //iftDestroyImage(&grad);
    iftDestroyImage(&scn);
    iftDestroyAdjRel(&A);

    return segm;
}

iftImage *ALTIS::iftExtractVolumeOfInterest(iftImage *img, iftAdjRel *A, iftImage **grad, iftSet **S)
{
    timer *t1,*t2;
    char *formatted = NULL;
    t1 = iftTic();

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Enhancing Volume of Interest");
    #endif
    /* Enhance volume of interest */
    enhanced = iftCreateImage(img->xsize,img->ysize,img->zsize);
    iftCopyVoxelSize(img,enhanced);
    #pragma omp parallel for shared(img)
        for (int z=0; z < img->zsize; z++) {
            iftImage *slice = iftGetXYSlice(img,z);
            iftImage *cbas  = iftCloseBasins(slice,NULL,NULL);
            iftImage *res   = iftSub(cbas,slice);
            iftDestroyImage(&slice);
            iftDestroyImage(&cbas);
            iftPutXYSlice(enhanced,res,z);
            iftDestroyImage(&res);
        }

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Computing Gradient Image");
    #endif
    *grad = calculateImageGradient();

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Extracting VOI");
    #endif
//    /* Extract volume of interest and its boundary as a set of voxels */
//    iftImage *bin[2], *voi;
//    bin[0] = iftThreshold(enhanced,otsuThreshold*iftOtsu(enhanced),IFT_INFINITY_INT,1);
//    iftDestroyImage(&enhanced);
//    bin[1] = iftSelectLargestComp(bin[0],A);
//    iftDestroyImage(&bin[0]);
//    *S     = NULL;
//    bin[0] = iftDilateBin(bin[1],S,closingRadius);
//    iftDestroyImage(&bin[1]);
//    bin[1] = iftErodeBin(bin[0],S,closingRadius);
//    voi    = iftCloseBasins(bin[1],NULL,NULL);
//    iftDestroyImage(&bin[0]);
//    iftDestroyImage(&bin[1]);

        /* Extract volume of interest and its boundary as a set of voxels */
        iftImage *bin[2], *voi;
        bin[0] = iftThreshold(enhanced,this->otsuThreshold*iftOtsu(enhanced),IFT_INFINITY_INT,1);
        iftDestroyImage(&enhanced);

        int obj_index = 1;
        bin[1] = iftSelectKLargestComp(bin[0],A,2);
        iftImage *comps = iftLabelComp(bin[1], A);
        iftImage *sorted_comps = iftSortComponentsByAreaVolume(comps);
        iftDestroyImage(&comps);
        if (iftMaximumValue(sorted_comps) > 1){
            float obj1_vol = iftAreaVolumeOfObject(sorted_comps,1);
            float obj2_vol = iftAreaVolumeOfObject(sorted_comps,2);
            if ((obj2_vol/obj1_vol) > 0.3) {
    #ifdef SHOW_STEP_BY_STEP
                puts("----- Stretcher detected.");
    #endif
                iftVoxel gc;
                iftBoundingBox bb = iftMinObjectBoundingBox(sorted_comps,1,&gc);
                if (((bb.begin.x == 0) && (bb.end.x = sorted_comps->xsize-1)) ||
                    ((bb.begin.y == 0) && (bb.end.y = sorted_comps->ysize-1)) ||
                    ((bb.begin.z == 0) && (bb.end.z = sorted_comps->zsize-1)))
                    obj_index = 2;
            }
        }
        iftImage *obj = iftExtractObject(sorted_comps, obj_index);
        iftDestroyImage(&bin[1]);
        bin[1] = iftThreshold(obj, 1, 2, 1);
        iftDestroyImage(&obj);

        iftDestroyImage(&bin[0]);
        *S     = NULL;
        bin[0] = iftDilateBin(bin[1],S,this->closingRadius);
        iftDestroyImage(&bin[1]);
        bin[1] = iftErodeBin(bin[0],S,this->closingRadius);
        voi    = iftCloseBasins(bin[1],NULL,NULL);
        iftDestroyImage(&bin[0]);
        iftDestroyImage(&bin[1]);



    t2 = iftToc();
    fprintf(stdout,"Extraction of the Volume of Interest took ");
    formatted = iftFormattedTime(iftCompTime(t1, t2));
    puts(formatted);
    iftFree(formatted);

    return(voi);
}

iftImage *ALTIS::iftSegmentRespiratorySystem(iftImage *grad, iftImage *voi, iftSet *S, iftAdjRel *A, iftImage *img)
{
    Q_UNUSED(img);
    timer *t1,*t2;
    char *formatted = NULL;
    iftImage *label=NULL, *dil=NULL;
    //iftImage *pred=NULL;
    iftSet   *Si=NULL, *Se=NULL, *St=NULL, *forbidden=NULL;
    iftLabeledSet *seeds = NULL;
    //iftLabeledSet *newS = NULL;

    t1 = iftTic();

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Estimating External and Internal Lung Markers");
    #endif
    updateProgress(0.4);
    dil   = iftExternalInternalLungMarkers(voi, S, &Si, &Se);

    #ifdef DEBUG
        sprintf(filename,"%s/DEBUG/dilate/%s",img_parent_folder,file);
        iftWriteImageByExt(dil,filename);
    #endif

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Estimating Trachea Markers");
    #endif
    St    = iftTracheaMarker(voi,Si);

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Labeling Markers");
    #endif
    seeds = iftLabelMarkers(dil, Si, St, Se, &forbidden);

    #ifdef DEBUG
        sprintf(filename,"%s/DEBUG/markers/%s.txt",img_parent_folder,base);
        iftWriteSeeds(seeds,img,filename);
    #endif

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Performing Watershed");
    #endif
    updateProgress(0.5);

    if (grad == nullptr)
        return nullptr;

    forest = iftCreateImageForest(grad, iftCopyAdjacency(A));
    this->seeds = iftCopyLabeledSet(seeds);
    iftFastDiffWatershed(forest, seeds, NULL);
    label = iftCopyImage(forest->label);
    ui->pbCorrect->setEnabled(true);

    #ifdef SHOW_STEP_BY_STEP
        puts("--- Deallocating memory space");
    #endif
    iftDestroyImage(&dil);
    iftDestroyLabeledSet(&seeds);
    iftDestroySet(&forbidden);
    iftDestroySet(&Si);
    iftDestroySet(&St);
    iftDestroySet(&Se);

    t2 = iftToc();
    fprintf(stdout,"Respiratory System Segmentation took ");
    formatted = iftFormattedTime(iftCompTime(t1, t2));
    puts(formatted);
    iftFree(formatted);

    return label;
}

iftImage *ALTIS::iftExternalInternalLungMarkers(iftImage *bin, iftSet *S, iftSet **Si, iftSet **Se)
{
    iftImage *dist=NULL,*root=NULL,*dil=NULL;
    iftGQueue *Q=NULL;
    int i,p,q,tmp;
    iftVoxel u,v,r;
    iftSet *Saux;
    iftAdjRel *A;

    *Si   = *Se = NULL;
    dist  = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    dil   = iftCopyImage(bin);
    root  = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    Q     = iftCreateGQueue(IFT_QSIZE,bin->n,dist->val);

    for (p=0; p < dist->n; p++) {
        dist->val[p]= IFT_INFINITY_INT;
    }

    /* Use the boundary of the mask and its neighbors outside the mask
       as seeds */

    Saux = S;
    A     = iftSpheric(1.0);
    while (Saux != NULL) {
        p = Saux->elem;
        dist->val[p]=0;
        root->val[p]=p;
        iftInsertGQueue(&Q,p);
        u = iftGetVoxelCoord(bin,p);
        for (i=1; i < A->n; i++){
            v = iftGetAdjacentVoxel(A,u,i);
            if (iftValidVoxel(bin,v)){
                q = iftGetVoxelIndex(bin,v);
                if ((bin->val[q]==0)&&
                    (Q->L.elem[q].color==IFT_WHITE)){
                    dist->val[q]=0;
                    root->val[q]=q;
                    iftInsertGQueue(&Q,q);
                }
            }
        }
        Saux = Saux->next;
    }
    iftDestroyAdjRel(&A);
    A = iftSpheric(sqrtf(3.0));


    /* Compute the internal and external seeds */

    while(!iftEmptyGQueue(Q)) {

        p=iftRemoveGQueue(Q);
        u = iftGetVoxelCoord(bin,p);
        r = iftGetVoxelCoord(bin,root->val[p]);

        if (bin->val[p]==0){ /* external voxel */
            dil->val[p]=1;
            if (dist->val[p] <= dilationSquaredRadius){
                for (i=1; i < A->n; i++){
                    v = iftGetAdjacentVoxel(A,u,i);
                    if (iftValidVoxel(bin,v)){
                        q = iftGetVoxelIndex(bin,v);
                        if ((dist->val[q] > dist->val[p])&&
                            (bin->val[q]==0)){
                            tmp = iftSquaredVoxelDistance(v,r);
                            if (tmp < dist->val[q]){
                                if (dist->val[q] != IFT_INFINITY_INT)
                                    iftRemoveGQueueElem(Q, q);
                                dist->val[q]  = tmp;
                                root->val[q]  = root->val[p];
                                iftInsertGQueue(&Q, q);
                            }
                        }
                    }
                }
            }
            else {
                iftInsertSet(Se,p);
            }
        } else { /* interval voxel */
            if (dist->val[p] <= erosionSquaredRadius){
                for (i=1; i < A->n; i++){
                    v = iftGetAdjacentVoxel(A,u,i);
                    if (iftValidVoxel(bin,v)){
                        q = iftGetVoxelIndex(bin,v);
                        if ((dist->val[q] > dist->val[p])&&
                            (bin->val[q]!=0)){
                            tmp = iftSquaredVoxelDistance(v,r);
                            if (tmp < dist->val[q]){
                                if (dist->val[q] != IFT_INFINITY_INT)
                                    iftRemoveGQueueElem(Q, q);
                                dist->val[q]  = tmp;
                                root->val[q]  = root->val[p];
                                iftInsertGQueue(&Q, q);
                            }
                        }
                    }
                }
            }
            else {
                iftInsertSet(Si,p);
            }
        }
    }

    #ifdef DEBUG
        iftImage *extint = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
        Saux = *Si;
        while (Saux != NULL){
            p = Saux->elem;
            extint->val[p] = 1;
            Saux = Saux->next;
        }
        Saux = *Se;
        while (Saux != NULL){
            p = Saux->elem;
            extint->val[p] = 2;
            Saux = Saux->next;
        }
        sprintf(filename,"%s/DEBUG/externalInternalMarkers/%s",img_parent_folder,file);
        iftWriteImageByExt(extint,filename);
        iftDestroyImage(&extint);
    #endif

    iftDestroyAdjRel(&A);
    iftDestroyGQueue(&Q);
    iftDestroyImage(&root);
    iftDestroyImage(&dist);

    return(dil);
}

iftSet *ALTIS::iftTracheaMarker(iftImage *bin, iftSet *Si)
{
    iftImage  *dist=NULL;
    iftGQueue *Q=NULL;
    int i,p,q,tmp,z,n;
    iftVoxel u,v;
    iftSet *Saux, *St=NULL;
    iftAdjRel *A;

    dist  = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    Q     = iftCreateGQueue(IFT_QSIZE,bin->n,dist->val);

    /* initialize distance map and compute the z coordinate of the
       geodesic center of the mask */

    for (p=0,z=0,n=0; p < bin->n; p++) {
        if (bin->val[p]!=0){
            u = iftGetVoxelCoord(bin,p);
            z = z + u.z; n = n + 1;
            dist->val[p]= IFT_INFINITY_INT;
        }
    }
    z = z / n;
    Saux = Si;

    while (Saux != NULL) {
        p = Saux->elem;
        dist->val[p]=0;
        iftInsertGQueue(&Q,p);
        Saux = Saux->next;
    }

    A     = iftSpheric(sqrtf(3.0));

    /* Compute the geodesic distance transform from the lung seeds */

    while(!iftEmptyGQueue(Q)) {

        p=iftRemoveGQueue(Q);
        u = iftGetVoxelCoord(bin,p);

        for (i=1; i < A->n; i++){
            v = iftGetAdjacentVoxel(A,u,i);
            if (iftValidVoxel(bin,v)){
                q = iftGetVoxelIndex(bin,v);
                if ((dist->val[q] > dist->val[p])&&
                    (bin->val[q]!=0)){
                    tmp = dist->val[p] + (int)(10*iftVoxelDistance(u,v));
                    if (tmp < dist->val[q]){
                        if (dist->val[q] != IFT_INFINITY_INT)
                            iftRemoveGQueueElem(Q, q);
                        dist->val[q]  = tmp;
                        iftInsertGQueue(&Q, q);
                    }
                }
            }
        }
    }
    iftDestroyGQueue(&Q);
    //iftDestroyAdjRel(&A);

    /* Find the maximum distance value above the z coordinate of the
       geometric center of the mask */

    tmp = 0;
    for (p=0; p < bin->n; p++){
        if (bin->val[p]!=0){
            u = iftGetVoxelCoord(bin,p);
            if ((u.z >= z)&&
                (dist->val[p]>tmp))
                tmp = dist->val[p];
        }
    }

    /* Select as trachea marker all voxels whose distance from the lung
       marker is above 70% of the maximum distance and z coordinate is
       above the geometric center */

    tmp = tracheaThreshold*tmp;
    for (p=0; p < bin->n; p++) {
        if ((u.z >= z) && (dist->val[p] > tmp) && (bin->val[p] != 0)) {
            iftInsertSet(&St, p);
        }
    }


    #ifdef DEBUG
        iftImage *export_trachea = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
        Saux = St;
        while (Saux != NULL){
            p = Saux->elem;
            export_trachea->val[p] = 1;
            Saux = Saux->next;
        }
        sprintf(filename,"%s/DEBUG/trachea/%s",img_parent_folder,file);
        iftWriteImageByExt(export_trachea,filename);
        for (p=0; p < dist->n; p++){
            if (dist->val[p] == IFT_INFINITY_INT)
                dist->val[p] = 0;
        }
        sprintf(filename,"%s/DEBUG/geodesic/%s",img_parent_folder,file);
        iftWriteImageByExt(dist,filename);
    #endif

    iftDestroyImage(&dist);
    return(St);
}

void ALTIS::iftLabelLungsMarkers(iftImage *bin, iftSet *Si, iftLabeledSet **seeds)
{

    iftImage *L,*SetSi;
    iftAdjRel *A;
    iftVoxel u,v;
    iftSet *Q, *Saux, *S1, *S2;
    int *hist;
    int p, q, r, s, max, c[2], l;
    int n1, x1, l1;
    int n2, x2, l2;

    Q = Saux = S1 = S2 = NULL;

    // Creating Image Containing Set Si
    SetSi = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    Saux = Si;
    while (Saux != NULL) {
        p = Saux->elem;
        SetSi->val[p] = 1;
        Saux = Saux->next;
    }

    // Labeling Connected Components
    L = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    A = iftSpheric(adjRelRadius); // TODO: it was ADJ_REL_RADIUS instead of adjRelRadius
    Saux = Si;
    l = 1;
    while (Saux != NULL) {
        s = Saux->elem;
        if (L->val[s] == 0){
            L->val[s] = l;
            iftInsertSet(&Q,s);
            while (Q != NULL){
                p = iftRemoveSet(&Q);
                u = iftGetVoxelCoord(L,p);
                for (r = 1; r < A->n; r++){
                    v = iftGetAdjacentVoxel(A,u,r);
                    if (iftValidVoxel(L,v)) {
                        q = iftGetVoxelIndex(L, v);
                        if ((L->val[q] == 0) && (SetSi->val[q] == 1)) {
                            L->val[q] = L->val[p];
                            iftInsertSet(&Q, q);
                        }
                    }
                }
            }
            l++;
        }
        Saux = Saux->next;
    }
    iftDestroyImage(&SetSi);
    iftDestroyAdjRel(&A);

    // Creating Histogram from Labeled Image
    hist = iftAllocIntArray(l);
    for (p = 0; p < L->n; p++){
        if (L->val[p] != 0){
            hist[L->val[p]]++;
        }
    }

    // Selecting Labels of the two Largest Components (Left and Right Lungs)
    for (s = 0; s < 2; s++){
        max = 0;
        c[s] = -1;
        for (r = 0; r < l; r++){
            if (max < hist[r]){
                max = hist[r];
                c[s] = r;
            }
        }
        hist[c[s]] = 0;
    }
    iftFree(hist);

    // Separating Left and Right Lungs from Labeled Image and
    // Computing Center of Mass from them
    Saux = Si;
    x1 = x2 = n1 = n2 = 0;
    while (Saux != NULL) {
        p = Saux->elem;
        if ((L->val[p] == c[0]) && (c[0] != -1)) {
            iftInsertSet(&S1, p);
            u = iftGetVoxelCoord(bin, p);
            x1 += u.x; n1++;
        }
        else if ((L->val[p] == c[1]) && (c[0] != -1)) {
            iftInsertSet(&S2, p);
            u = iftGetVoxelCoord(bin, p);
            x2 += u.x; n2++;

        }
        Saux = Saux->next;
    }
    if (n1 != 0)
        x1 /= n1;
    if (n2 != 0)
        x2 /= n2;
    iftDestroyImage(&L);

    // Inserting Left and Right Lung on LabeledSet
    if (x1 < x2) {
        l1 = 1;
        l2 = 2;
    } else {
        l1 = 2;
        l2 = 1;
    }
    while (S1 != NULL){
        p = iftRemoveSet(&S1);
        iftInsertLabeledSet(seeds,p,l1);
    }
    while (S2 != NULL){
        p = iftRemoveSet(&S2);
        iftInsertLabeledSet(seeds,p,l2);
    }
}

void ALTIS::iftLabelTracheaMarkers(iftImage *bin, iftSet *St, iftLabeledSet **seeds)
{
    iftImage *L, *SetSt;
    iftAdjRel *A;
    iftVoxel u,v;
    iftSet *Q, *Saux;
    int *mean_z;
    int p, q, r, s;
    int l, t;
    int size, z;

    Q = Saux = NULL;

    // Creating Image Containing Set St
    SetSt = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    Saux = St;
    size = 0;
    while (Saux != NULL) {
        p = Saux->elem;
        SetSt->val[p] = 1;
        Saux = Saux->next;
        size++;
    }

    // Labeling Connected Components and Computing their Center of Mass
    L = iftCreateImage(bin->xsize,bin->ysize,bin->zsize);
    A = iftSpheric(adjRelRadius); // TODO: it was ADJ_REL_RADIUS instead of adjRelRadius
    mean_z = iftAllocIntArray(size);
    Saux = St;
    l = 1;
    while (Saux != NULL) {
        s = Saux->elem;
        if (L->val[s] == 0){
            L->val[s] = l;
            iftInsertSet(&Q,s);
            size = 0;
            while (Q != NULL){
                p = iftRemoveSet(&Q);
                u = iftGetVoxelCoord(L,p);
                mean_z[L->val[p]] += u.z;
                size++;
                for (r = 1; r < A->n; r++){
                    v = iftGetAdjacentVoxel(A,u,r);
                    if (iftValidVoxel(L,v)) {
                        q = iftGetVoxelIndex(L, v);
                        if ((L->val[q] == 0) && (SetSt->val[q] == 1)) {
                            L->val[q] = L->val[p];
                            iftInsertSet(&Q, q);
                        }
                    }
                }
            }
            mean_z[l] /= size;
            l++;
        }
        Saux = Saux->next;
    }
    iftDestroyImage(&SetSt);
    iftDestroyAdjRel(&A);

    // Selecting the highest component
    z = 0;
    t = 0;
    for (p = 1; p <= l; p++){//l
        if (mean_z[p] > z){
            z = mean_z[p];
            t = p;
        }
    }
    iftFree(mean_z);

    // Inserting Trachea in LabeledSet
    int border;
    A = iftSpheric(1.0);
    for (p = 0; p < L->n; p++)
        if (L->val[p] == t){
            // Selecting seeds that do not belong to the border
            border = 0;
            u = iftGetVoxelCoord(L,p);
            for (int i = 1; i < A->n; i++) {
                v = iftGetAdjacentVoxel(A, u, i);
                if (iftValidVoxel(L, v)) {
                    q = iftGetVoxelIndex(L, v);
                    if (L->val[q] != t) {
                        border = 1;
                        break;
                    }
                }
            }
            if (!border)
                iftInsertLabeledSet(seeds,p,3);
        }
    iftDestroyImage(&L);
    iftDestroyAdjRel(&A);
}

iftLabeledSet *ALTIS::iftLabelMarkers(iftImage *bin, iftSet *Si, iftSet *St, iftSet *Se, iftSet **forbidden)
{
    int p;
    iftLabeledSet *seeds=NULL;
    iftSet *Saux = NULL;

    iftLabelLungsMarkers(bin,Si,&seeds);

    iftLabelTracheaMarkers(bin,St,&seeds);

    Saux = Se;
    while (Saux != NULL) {
        p = Saux->elem;
        iftInsertLabeledSet(&seeds,p,0);
        Saux = Saux->next;
    }

    *forbidden=NULL;
    for (int p=0; p < bin->n; p++)
        if (bin->val[p]==0)
            iftInsertSet(forbidden,p);

    return(seeds);
}

void ALTIS::iftGaussianFilter(bool apply, iftImage **img)
{
    if (apply) {
        #ifdef SHOW_STEP_BY_STEP
            puts("");
            puts("- Performing the Gaussian Filter");
            timer     *t1=iftTic();
        #endif
        iftImage *gauss, *aux, *scn;
        scn = *img;
        iftKernel *K = iftGaussianKernel(adjRelRadius, 1.5); // TODO: it was ADJ_REL_RADIUS instead of adjRelRadius
        gauss = iftLinearFilter(scn,K);
        iftDestroyKernel(&K);
        aux = iftAbs(gauss);
        iftDestroyImage(&gauss);
        *img = aux;
        iftDestroyImage(&scn);
        #ifdef SHOW_STEP_BY_STEP
            printf("--- Gaussian filter took ");
            char *formatted = iftFormattedTime(iftCompTime(t1, iftToc()));
            puts(formatted);
            iftFree(formatted);
        #endif
    }
}

void ALTIS::iftRemoveNoise(bool apply, iftImage **img, iftAdjRel *A)
{
    if (apply) {
        #ifdef SHOW_STEP_BY_STEP
            puts("");
            puts("- Removing noise from image");
            timer     *t1=iftTic();
        #endif
        iftImage *filt = iftMedianFilter(*img,A);
        iftDestroyImage(img);
        *img = filt;
        #ifdef SHOW_STEP_BY_STEP
            printf("--- Median filter took ");
            char *formatted = iftFormattedTime(iftCompTime(t1, iftToc()));
            puts(formatted);
            iftFree(formatted);
        #endif
    }
}


/************** ANOMALY DETECTION ***************/

#define LEFT_LUNG 1
#define RIGHT_LUNG 2

iftImage *iftPleuralAnomalyDetectionByGeodesicDistance(iftImage *segm, float radius, float geo_th)
{

    // Extracting LEFT LUNG
    puts("--- Left Lung");
    iftImage *LL                = iftExtractObject(segm,LEFT_LUNG);
    iftImage *LL_mediastinum    = iftGeodesicDistanceInPleura(segm, LL, geo_th);
    iftImage *LL_closed         = iftRestrictedCloseBin(LL,radius,LL_mediastinum);
    iftDestroyImage(&LL_mediastinum);
    iftDestroyImage(&LL);

    // Extracting RIGHT LUNG
    puts("--- Right Lung");
    iftImage *RL                = iftExtractObject(segm,RIGHT_LUNG);
    iftImage *RL_mediastinum    = iftGeodesicDistanceInPleura(segm, RL, geo_th);
    iftImage *RL_closed         = iftRestrictedCloseBin(RL,radius,RL_mediastinum);
    iftDestroyImage(&RL_mediastinum);
    iftDestroyImage(&RL);

    //merging objects
    iftImage *closed_segm = iftOr(LL_closed,RL_closed);
    iftDestroyImage(&LL_closed);
    iftDestroyImage(&RL_closed);

    //adding trachea
    for (int p = 0; p < segm->n; p++)
        if (segm->val[p] == 3)
            closed_segm->val[p] = 3;

    return closed_segm;

}

iftImage *iftGeodesicDistanceInPleura(iftImage *segm, iftImage *lung, float geo_th)
{
    iftImage *border = iftBorderImage(lung,FALSE);

    // Computing Seeds
    iftSet *seeds = NULL;
    iftAdjRel *A  = iftSpheric(sqrt(3.));
    for (int p = 0; p < border->n; p++)
    {
        if (border->val[p] == 0)
            continue;
        iftVoxel u = iftGetVoxelCoord(border,p);
        for (int a = 1; a < A->n; a++){
            iftVoxel v = iftGetAdjacentVoxel(A,u,a);
            if (iftValidVoxel(segm,v)){
                int q = iftGetVoxelIndex(border,v);
                if (segm->val[q] == 3){
                    iftInsertSet(&seeds,p);
                    break;
                }
            }
        }
    }

    iftFImage *geo_dist = iftGeodesicDistTrans(seeds,border,A);
    iftDestroyAdjRel(&A);
    float max = iftFMaximumValue(geo_dist);
    float min = max;
    for (int p = 0; p < geo_dist->n; p++){
        if ((geo_dist->val[p] < min) && (geo_dist->val[p] > 0))
            min = geo_dist->val[p];
    }
    iftImage *geo_dist_th = iftFThreshold(geo_dist,min,geo_th*max,1);
    iftDestroyFImage(&geo_dist);
    while (seeds != NULL){
        int p = iftRemoveSet(&seeds);
        geo_dist_th->val[p] = 1;
    }
    iftDestroyImage(&border);

    return geo_dist_th;
}

iftImage *iftRestrictedCloseBin(iftImage *bin, float radius, iftImage *forbidden)
{
    iftImage *bin_frame = iftAddFrame(bin,iftRound(radius)+5,0);
    iftImage *forbidden_frame = iftAddFrame(forbidden,iftRound(radius)+5,0);

    // IFT variables
    iftImage  *dist=NULL,*root=NULL,*dil=NULL;
    iftGQueue *Q=NULL;
    int        i,p,q,tmp;
    iftVoxel   u,v,r;
    iftAdjRel *A=iftSpheric(sqrt(3.)),*B=iftSpheric(1.);
    float      maxdist=radius*radius; // the EDT computes squared
    iftSet    *seed=NULL;

    // IFT Dilation limited by mediastinal pleura
    seed = iftObjectBorderSet(bin_frame,B);
    iftDestroyAdjRel(&B);

    // Initialization

    dil   = iftCopyImage(bin_frame);
    dist  = iftCreateImage(bin_frame->xsize,bin_frame->ysize,bin_frame->zsize);
    root  = iftCreateImage(bin_frame->xsize,bin_frame->ysize,bin_frame->zsize);
    Q     = iftCreateGQueue(IFT_QSIZE,bin_frame->n,dist->val);

    for (p=0; p < bin_frame->n; p++) {
        if (bin_frame->val[p] == 0)
            dist->val[p]= IFT_INFINITY_INT;
    }

    while (seed != NULL) {
        p = iftRemoveSet(&seed);
        dist->val[p]=0;
        root->val[p]=p;
        iftInsertGQueue(&Q,p);
    }

    // Image Foresting Transform

    while(!iftEmptyGQueue(Q)) {
        p=iftRemoveGQueue(Q);

        if (dist->val[p] <= maxdist){

            dil->val[p] = bin_frame->val[root->val[p]]; // dilation
            u = iftGetVoxelCoord(bin_frame,p);
            r = iftGetVoxelCoord(bin_frame,root->val[p]);

            for (i=1; i < A->n; i++){
                v = iftGetAdjacentVoxel(A,u,i);
                if (iftValidVoxel(bin_frame,v)){
                    q = iftGetVoxelIndex(bin_frame,v);
                    if (dist->val[q] > dist->val[p]){
                        tmp = (v.x-r.x)*(v.x-r.x) + (v.y-r.y)*(v.y-r.y) + (v.z-r.z)*(v.z-r.z);
                        if (tmp < dist->val[q]){
                            if (dist->val[q] != IFT_INFINITY_INT)
                                iftRemoveGQueueElem(Q, q);
                            dist->val[q]  = tmp;
                            root->val[q]  = root->val[p];
                            iftInsertGQueue(&Q, q);
                        }
                    }
                }
            }
        }else{ /* seeds for a possible subsequent erosion */
            iftInsertSet(&seed,p);
        }
    }

    iftDestroyImage(&bin_frame);
    iftDestroyGQueue(&Q);
    iftDestroyImage(&dist);
    iftDestroyAdjRel(&A);

    iftImage *filtered_dil = iftRemoveMediastinumForest(forbidden_frame,dil,root);
    iftDestroyImage(&forbidden_frame);
    iftDestroyImage(&root);
    iftDestroyImage(&dil);

    // Performing erosion
    iftImage *er   = iftErodeBin(filtered_dil,&seed,radius);
    iftImage *close = iftRemFrame(er,iftRound(radius)+5);
    iftDestroyImage(&er);
    iftDestroyImage(&filtered_dil);
    iftDestroySet(&seed);

    return close;
}

iftImage *iftSortComponentsByAreaVolume(iftImage *label_map) {
    iftIntArray *index = iftCreateIntArray(iftMaximumValue(label_map)+1);
    iftIntArray *areavolume = iftCreateIntArray(iftMaximumValue(label_map)+1);

    // initializing
    for (int i = 1; i <= iftMaximumValue(label_map); i++){
        index->val[i] = i;
    }
    for (int i = 0; i < label_map->n; i++){
        if (label_map->val[i] > 0)
            areavolume->val[label_map->val[i]]++;
    }

    for (int i = 1; i < areavolume->n; i++)
        for (int j = i-1; j > 0; j--)
            if (areavolume->val[i] > areavolume->val[j]){
                int aux = index->val[i];
                index->val[i] = index->val[j];
                index->val[j] = aux;
                aux = areavolume->val[i];
                areavolume->val[i] = areavolume->val[j];
                areavolume->val[j] = aux;
            }
    iftDestroyIntArray(&areavolume);

    iftImage *out = iftCopyImage(label_map);
    for (int i = 0; i < out->n; i++)
        out->val[i] = index->val[out->val[i]];
    iftDestroyIntArray(&index);


    return out;
}

iftImage *iftRemoveMediastinumForest(iftImage *mediastinum_bin, iftImage *closed, iftImage *closed_root)
{
    iftImage *out = iftCopyImage(closed);
    for (int i = 0; i < closed->n; i++){
        if (mediastinum_bin->val[closed_root->val[i]] > 0){
            out->val[i] = 0;
        }
    }

    return out;
}
