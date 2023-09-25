#ifndef ORIENTATION_H
#define ORIENTATION_H

#include <global.h>

class Orientation
{
public:
    Orientation();
    ~Orientation();

    iftMatrix *getAxialOrientationMatrix();
    iftMatrix *getCoronalOrientationMatrix();
    iftMatrix *getSagittalOrientationMatrix();

    void setAxialOrientationMatrix(iftMatrix *m);
    void setCoronalOrientationMatrix(iftMatrix *m);
    void setSagittalOrientationMatrix(iftMatrix *m);

    iftMatrix *createRotationMatrix();
    void rotateAxialSlice();
    void rotateCoronalSlice();
    void rotateSagittalSlice();

    iftMatrix *createHorizontalFlipMatrix();
    void flipAxialSlice();
    void flipCoronalSlice();
    void flipSagittalSlice();

    void setOrientationToRadiologists();
    void setOrientationToNeurologists();

    iftVoxel mapPixelToVolume(iftVoxel u, int slice, char plane);

    iftImage* applyOrientationOnSlice(iftImage *img, char plane);

private:


    iftMatrix *A, *C, *S;
    iftMatrix *Ainv, *Cinv, *Sinv;
    iftVoxel offset[3];

    char dimension;
};

#endif // ORIENTATION_H
