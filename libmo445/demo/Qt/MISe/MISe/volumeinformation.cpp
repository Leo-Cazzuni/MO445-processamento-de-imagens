#include "volumeinformation.h"
#include "ui_volumeinformation.h"

VolumeInformation::VolumeInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeInformation)
{
    ui->setupUi(this);
}

VolumeInformation::~VolumeInformation()
{
    delete ui;
}

void VolumeInformation::showVolumeInformation(const iftImage *img,QString filename)
{
    char str[200];
    if (img != nullptr){
        ui->lblImagePath->setText(filename);

        float voxel_size;
        if (iftIs3DImage(img)) {
            voxel_size = img->dx*img->dy*img->dz;
            sprintf(str,"%f x %f x %f = %f units³",img->dx,img->dy,img->dz,voxel_size);
        } else {
            voxel_size = img->dx*img->dy;
            sprintf(str,"%f x %f = %f units²",img->dx,img->dy,voxel_size);
        }
        ui->lblVoxelSize->setText(str);

        if (iftIs3DImage(img)) {
            int image_size = img->xsize*img->ysize*img->zsize;
            sprintf(str,"%d x %d x %d = %d (%.2f units² )",img->xsize,img->ysize,img->zsize,image_size,voxel_size*image_size);
        } else {
            int image_size = img->xsize*img->ysize;
            sprintf(str,"%d x %d = %d (%.2f units³)",img->xsize,img->ysize,image_size,voxel_size*image_size);
        }
        ui->lblImageDomain->setText(str);

        int depth = iftImageDepth(img);
        sprintf(str,"%.0f (%d bits)",pow(2,depth),depth);
        ui->lblLabelDepth->setText(str);

        sprintf(str,"Maximum value: %d, Minimum value: %d",iftMaximumValue(img),iftMinimumValue(img));
        ui->lblMaxAndMinValues->setText(str);
    }

}
