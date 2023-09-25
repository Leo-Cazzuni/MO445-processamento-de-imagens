#ifndef RENDERING_H
#define RENDERING_H

#include "global.h"

class Rendering
{
public:
    Rendering(iftImage *scene, iftImage *label);
    Rendering(iftImage *scene, iftImage *label, float ka, float kd, float ks);
    ~Rendering();

    void setGraphicalContextLabel(iftImage *lb);
    void setSceneOnGraphicalContext(iftImage *scene);
    void destroyLabel();

    iftVoxel getVoxelFromIndexBuffer(int x, int y);
    iftVoxel getVoxelFromIndexBuffer(iftVoxel u);

    void setObjectColor(int obj, iftColor YCbCr);
    void setObjectOpacity(int obj, float opac);
    void setObjectVisibility(int obj, bool visible);

    void setViewDirections(double tilt, double spin);
    void getViewDirections(double *tilt, double *spin);

    char getMode();
    void setMode(char mode);    

    iftImage *getIndexBuffer();

    bool *getObjectsVisibility();

    void setAmbientLightReflection(float ka);
    void setDiffuseLightReflection(float ka);
    void setSpecularLightReflection(float ka);

    void toogleProjectionMode();
    void toogleWireFrameVisibility();

    bool isGraphicalContextLabelEmpty();

    //int iftSliceRenderingAlongRay(iftImage *img, iftPoint P0, iftPoint P1, iftPoint Pn, int axialSlice, int coronalSlice, int sagittalSlice);
    iftImage *iftRender(iftImage *img, int axialSlice, int coronalSlice, int sagittalSlice);

private:

    float normalizationValue;
    float phongReflectionFactor;
    iftGraphicalContext *gc;
    iftImage *indexbuffer;
    char mode, projectionMode;
    double tilt = 0, spin = 0;
    bool wireframe;
};

#endif // RENDERING_H
