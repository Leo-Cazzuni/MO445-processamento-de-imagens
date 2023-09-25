#include "flim.h"
#include <QDir>

FLIM::FLIM(iftFLIMArch *arch, QTemporaryDir *origDir, QDir *network) :
    arch(arch), origDir(origDir), network(network)
{

}

FLIM::~FLIM()
{
    delete arch;
    delete origDir;
    delete network;
}


#define Imax 255
iftFImage *MImageGradient(iftMImage *img, iftAdjRel *A)
{
  iftFImage *gradI = iftCreateFImage(img->xsize,img->ysize,img->zsize);
  float Gmin=IFT_INFINITY_FLT, Gmax=IFT_INFINITY_FLT_NEG;

  for (ulong p=0; p < img->n; p++){
    iftVoxel u  = iftMGetVoxelCoord(img,p);
    float value = 0.0;
    for (int i=1; i < A->n; i++) {
      iftVoxel v = iftGetAdjacentVoxel(A,u,i);
      if (iftMValidVoxel(img,v)){
    int q = iftMGetVoxelIndex(img,v);
    for (ulong b=0; b < img->m; b++)
      value += (img->val[q][b]-img->val[p][b])*(img->val[q][b]-img->val[p][b]);
      }
    }
    value = sqrtf(value)/A->n;
    if (value > Gmax) Gmax = value;
    if (value < Gmin) Gmin = value;
    gradI->val[p] = value;
  }

  for (ulong p=0; p < img->n; p++){
    gradI->val[p] = (Imax*(gradI->val[p]-Gmin)/(Gmax-Gmin));
  }

/*
  iftImage *temp = iftCreateImage(gradI->xsize,gradI->ysize,gradI->zsize);
  for (int p=0; p < img->n; p++){
     temp->val[p] = iftRound(gradI->val[p]);
  }
  iftWriteImageByExt(temp,"grad.png");
  iftDestroyImage(&temp); */

  return(gradI);
}

#include <QDebug>
iftImage *FLIM::generateImageGradient()
{
    QString orig_dir = origDir->path();
    QString param_dir = network->path();



    QFile *image_list_file = new QFile("/tmp/list.csv");//todo

    if(image_list_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream out(image_list_file);
         QString filename = QDir(origDir->path()).entryList()[2]; // TODO
         out << filename << Qt::endl;
    }
    image_list_file->close();
    QString image_list = QFileInfo(*image_list_file).absoluteFilePath();//QFileInfo(*image_list_file).absoluteFilePath();

   // QTemporaryDir *featDir = new QTemporaryDir();
    QDir *featDir = new QDir("/home/ilan/features/");
    QString feat_dir = featDir->path();

    qDebug() << orig_dir;
    qDebug() << image_list;
    qDebug() << param_dir;
    qDebug() << feat_dir;

    iftFLIMExtractFeatures(orig_dir.toUtf8().data(), image_list.toUtf8().data(), arch,
                           param_dir.toUtf8().data(), feat_dir.toUtf8().data(), nullptr, 0);

    qDebug() << QDir(featDir->path()).entryList();
    QFileInfo act_mimg(featDir->path()+"/"+QDir(featDir->path()).entryList()[2]);
    iftMImage *mimg = iftReadMImage(act_mimg.absoluteFilePath().toUtf8().data());

    iftAdjRel *A = iftSpheric(1.733);

    iftFImage *grad = MImageGradient(mimg, A);

    iftImage *out  = iftFImageToImage(grad, Imax);

    iftDestroyAdjRel(&A);
    iftDestroyMImage(&mimg);
    iftDestroyFImage(&grad);
    delete featDir;

    iftWriteImageByExt(out, "grad.nii");

    return out;
}

iftMImage *FLIM::extractFeatures()
{
    QString orig_dir = origDir->path();
    QString param_dir = network->path();



    QFile *image_list_file = new QFile("/tmp/list.csv");//todo

    if(image_list_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream out(image_list_file);
         QString filename = QDir(origDir->path()).entryList()[2]; // TODO
         out << filename << Qt::endl;
    }
    image_list_file->close();
    QString image_list = QFileInfo(*image_list_file).absoluteFilePath();//QFileInfo(*image_list_file).absoluteFilePath();

    QTemporaryDir *featDir = new QTemporaryDir();
    //QDir *featDir = new QDir("/home/ilan/features/");
    QString feat_dir = featDir->path();

    qDebug() << orig_dir;
    qDebug() << image_list;
    qDebug() << param_dir;
    qDebug() << feat_dir;

    iftFLIMExtractFeatures(orig_dir.toUtf8().data(), image_list.toUtf8().data(), arch,
                           param_dir.toUtf8().data(), feat_dir.toUtf8().data(), nullptr, 0);

    qDebug() << QDir(featDir->path()).entryList();
    QFileInfo act_mimg(featDir->path()+"/"+QDir(featDir->path()).entryList()[2]);
    iftMImage *mimg = iftReadMImage(act_mimg.absoluteFilePath().toUtf8().data());

    //delete featDir;

    return mimg;
}
