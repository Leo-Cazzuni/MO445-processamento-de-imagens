#include "markers.h"

Markers::Markers()
{
    lastPoint.x = -1;
    lastPoint.y = -1;
    lastPoint.z = 0;

    this->current_ratio = 5.;
    this->brush = iftCircular(5.);
}

void Markers::setBrush(double ratio)
{
    if (this->brush != nullptr)
        iftDestroyAdjRel(&this->brush);

    if (ratio > this->max_ratio){
        ratio = max_ratio;
    } else if (ratio < this->min_ratio){
        ratio = min_ratio;
    }

    this->current_ratio = ratio;
    this->brush = iftCircular(float(ratio));
}

void Markers::draw(iftImage *img, QImage *qimg, iftVoxel u, int marker_label, QColor marker_color)
{
    if (this->brush == nullptr)
        return;

    iftVoxel v;

    for (int i = 0; i < brush->n; i++){
        v = iftGetAdjacentVoxel(brush,u,i);
        if (!((v.x >= 0) && (v.x <= qimg->width()-1) && (v.y >= 0) && (v.y <= qimg->height()-1)) || !iftValidVoxel(img,v))
            continue;
        int p = iftGetVoxelIndex(img,v);
        qimg->setPixel(v.x, v.y, marker_color.rgb());
        img->val[p] = marker_label;
    }
}

void Markers::draw(iftImage *img, QImage *qimg, iftVoxel u, int marker_label, QColor marker_color, Orientation *orient)
{
    if (this->brush == nullptr)
        return;

    iftVoxel v1,v2;

    for (int i = 0; i < brush->n; i++){
        v1 = iftGetAdjacentVoxel(brush,u,i);
        if (!((v1.x >= 0) && (v1.x <= qimg->width()-1) && (v1.y >= 0) && (v1.y <= qimg->height()-1)))
            continue;
        qimg->setPixel(v1.x, v1.y, marker_color.rgb());

        v2 = orient->mapPixelToVolume(v1);
        if (!iftValidVoxel(img,v2))
            continue;
        int p = iftGetVoxelIndex(img,v2);
        img->val[p] = marker_label;
    }
}

void Markers::erase(iftImage *img, QImage *marked_img, QImage clean_img, iftVoxel u)
{
    if (this->brush == nullptr)
        return;

    iftVoxel v;

    for (int i = 0; i < brush->n; i++){
        v = iftGetAdjacentVoxel(brush,u,i);
        if (!iftValidVoxel(img,v))
            continue;
        marked_img->setPixel(v.x,v.y,clean_img.pixel(v.x,v.y));
        int p = iftGetVoxelIndex(img,v);
        img->val[p] = 0;
    }

}

void Markers::erase(iftImage *img, QImage *marked_img, QImage clean_img, iftVoxel u, Orientation *orient)
{
    if (this->brush == nullptr)
        return;

    iftVoxel v1,v2;

    for (int i = 0; i < brush->n; i++){
        v1 = iftGetAdjacentVoxel(brush,u,i);
        if (!((v1.x >= 0) && (v1.x <= marked_img->width()-1) && (v1.y >= 0) && (v1.y <= marked_img->height()-1)))
            continue;
        marked_img->setPixel(v1.x, v1.y, clean_img.pixel(v1.x,v1.y));

        v2 = orient->mapPixelToVolume(v1);
        if (!iftValidVoxel(img,v2))
            continue;
        int p = iftGetVoxelIndex(img,v2);
        img->val[p] = 0;
    }
}
