#include "common.h"

float pointDefaultRx = 6;
float pointDefaultRy = 6;
float textDefaultFontSize = pointDefaultRx;

float boundBoxDefaultX = -textDefaultFontSize;
float boundBoxDefaultY = -textDefaultFontSize;
float boundBoxDefaultWidth = 2*textDefaultFontSize;
float boundBoxDefaultHeight = 2*textDefaultFontSize;


//Transforms iftImage into QImage (in GrayScale)
QImage *iftImageToQImage(iftImage *img, iftImage *markers, QHash<QString,QColor> hashMarkerId2MarkerColor, iftImage *label, iftImage *label_border, QHash<int,QColor> hashLabelId2LabelColor)
{
    QImage *dst = new QImage(img->xsize,img->ysize,QIMAGE_FORMAT);
    QColor color;

    if (iftIsColorImage(img)){
        //#pragma omp parallel for private(color)
        for (int i=0;i<img->n;i++){
            iftColor YCbCr, RGB;
            YCbCr.val[0] = img->val[i];
            YCbCr.val[1] = img->Cb[i];
            YCbCr.val[2] = img->Cr[i];

            if (label != nullptr){
                if (label->val[i] > 0){
                    if (hashLabelId2LabelColor.find(label->val[i] % MAX_NUMBER_LABEL_COLORS) != hashLabelId2LabelColor.end()){
                        // The YCbCr colors from IFT are stored in the QColor strutcture at the RGB attributes respectively
                        YCbCr.val[1] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].green();
                        YCbCr.val[2] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].blue();
                        if (label_border->val[i] > 0){
                            YCbCr.val[0] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].red();
                        }
                    }
                }
            }

            if (markers != nullptr){
                if (markers->val[i] > 0){
                    QString marker_name;
                    if (markers->val[i] == 1)
                        marker_name = "Background";
                    else
                        marker_name = QString("Marker %1").arg(markers->val[i]-1);
                    if (hashMarkerId2MarkerColor.find(marker_name) != hashMarkerId2MarkerColor.end()){
                        iftVoxel u = iftGetVoxelCoord(img,i);
                        dst->setPixel(u.x,u.y,hashMarkerId2MarkerColor[marker_name].rgb());
                        continue;
                    }
                }
            }
            RGB = iftYCbCrtoRGB(YCbCr,255);
            color.setRed(RGB.val[0]);
            color.setGreen(RGB.val[1]);
            color.setBlue(RGB.val[2]);
            iftVoxel u = iftGetVoxelCoord(img,i);
            dst->setPixel(u.x,u.y,color.rgb());
        }
    } else {
        //#pragma omp parallel for private(color)        
        for (int i=0;i<img->n;i++){
            color.setRed(img->val[i]);
            color.setGreen(img->val[i]);
            color.setBlue(img->val[i]);

            if (label != nullptr){
                if (label->val[i] > 0){
                    if (hashLabelId2LabelColor.find(label->val[i] % MAX_NUMBER_LABEL_COLORS) != hashLabelId2LabelColor.end()){
                        iftColor YCbCr, RGB;
                        // The YCbCr colors from IFT are stored in the QColor strutcture at the RGB attributes respectively
                        YCbCr.val[0] = img->val[i];
                        if (label_border->val[i] > 0){
                            YCbCr.val[0] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].red();
                        }
                        YCbCr.val[1] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].green();
                        YCbCr.val[2] = hashLabelId2LabelColor[label->val[i] % MAX_NUMBER_LABEL_COLORS].blue();
                        RGB = iftYCbCrtoRGB(YCbCr,255);
                        color.setRed(RGB.val[0]);
                        color.setGreen(RGB.val[1]);
                        color.setBlue(RGB.val[2]);
                    }
                }
            }
            if (markers != nullptr){
                if (markers->val[i] > 0){
                    QString marker_name;
                    if (markers->val[i] == 1)
                        marker_name = "Background";
                    else
                        marker_name = QString("Marker %1").arg(markers->val[i]-1);
                    if (hashMarkerId2MarkerColor.find(marker_name) != hashMarkerId2MarkerColor.end()){
                        iftVoxel u = iftGetVoxelCoord(img,i);
                        dst->setPixel(u.x,u.y,hashMarkerId2MarkerColor[marker_name].rgb());
                        continue;
                    }
                }
            }
            iftVoxel u = iftGetVoxelCoord(img,i);
            dst->setPixel(u.x,u.y,color.rgb());
        }
    }

    return dst;
}

iftImage *iftColorGrayImageByActivation(iftImage *img, iftImage *activation, iftColorTable *ctb)
{
  iftImage *colored_image;

  colored_image = iftCreateColorImage(img->xsize,img->ysize,img->zsize,8);

  float Imax = iftMax(iftMaximumValue(activation),1); 

  for(int p = 0; p < img->n; p++){
      
    int index = int(((float(activation->val[p])/Imax)*(ctb->ncolors-1)));
      
    iftColor ycbcr_color = ctb->color[index];

    colored_image->val[p] = img->val[p];
    colored_image->Cb[p]  = ushort(ycbcr_color.val[1]);
    colored_image->Cr[p]  = ushort(ycbcr_color.val[2]);
  }
    
  return(colored_image);
}

iftLabeledSet* iftFirstElemOfComponents(iftImage *label_image)
{
    iftLabeledSet *firsts = nullptr;
    int n_objs = iftMaximumValue(label_image)+1;
    iftIntArray *objs = iftCreateIntArray(n_objs);

    for(int c = 1; c < n_objs; c++){
        objs->val[c] = -1;
    }
    for(int p = 0; p < label_image->n; p++){
        if ((label_image->val[p] > 0) && (objs->val[label_image->val[p]] == -1)){
            objs->val[label_image->val[p]] = p;
            iftInsertLabeledSet(&firsts,p,label_image->val[p]);
        }
    }
    iftDestroyIntArray(&objs);

    return firsts;
}
