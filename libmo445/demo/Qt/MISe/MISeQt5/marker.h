#ifndef MARKER_H
#define MARKER_H

#include "global.h"
#include <views/orientation.h>
#include <QVector>
#include <markerinformation.h>
#include <segmentation/forestwrapper.h>

class Marker
{
public:
    Marker(iftImageSequence *img);
    ~Marker();

    const iftImage *getMarkers() const;
    void setMarkers(const iftImage *img);

    void resetMarkers();

    void setBrushRadius(float new_radius);
    float getBrushRadius();
    iftAdjRel *getBrush();
    void setSphericity(bool sphere);

    void setAnnotationMode(char mode);
    char getAnnotationMode();

    void paintMarkersOnQImage(Orientation *orientation, QImage *img, QPoint point, int markerLabel, int sliceType, int sliceNum, bool erasing);
    void selectPointForDeletion(iftVoxel u, const iftImage *label, ForestWrapper *forest);

    void paintMarker(iftVoxel v, int label);
    void paintBox(iftVoxel v, int label);
    void eraseMarker(iftVoxel v);
    void setMarkerVisibility(int marker, int visibility);

    iftColor getMarkerColor(int marker);    
    //TODO make const
    MarkerInformationVector& getMarkerInfoArray();
    void setMarkerName(int index, QString name);
    int getIndexFromLabel(int label);
    void setMarkerLabel(int index, int label);
    iftColorTable *generateColorTable();//TODO getColorTable();
    iftIntArray *generateMarkersVisibility();

    void addItemInColorTable(int label = -1, iftColor *c = nullptr);
    void setObjectColorInColorTable(int obj, iftColor color);
    void destroyColorTable();

    void removeMarker(int markerLabel);

    void setLastPoint(QPoint *newPoint);
    const QPoint* getLastPoint() const;

    const iftImage *getRemovalMarkers() const;

    int getNextLabel();
    int getLabel(int index);
private:

    /******** IFT structures ********/
    iftImage *markers;
    iftAdjRel *brush;
    iftBoundingBox bb;

    MarkerInformationVector markerInfoArray;

    iftImage *removalMarkers;

    /******** Variables ********/
    float radius;
    bool sphere;
    char annotationMode;
    int bbIndex;
    int nmarkers;

    QPoint *lastPoint;
};

#endif // MARKER_H
