#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "global.h"
#include <views/view.h>
#include "markersettings.h"
#include "volumeinformation.h"
#include "segmentation/segmentation.h"
#include "segmentation/altis/altis.h"
#include "segmentation/semiautomatic/semiautomatic.h"
#include "segmentation/threshold/threshold.h"
#include <QVariant>
#include <postprocessing/postprocessing.h>
#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadImageFromCommandLine(QStringList img_path);
    void loadLabelFromCommandLine(QString label_path);

    void loadListOfObjects(bool useMarkerInfo = false);
    void destroyObjectTableActions();
    void setSystemMode(QString mode);

    void showSegmentationModule(Segmentation *module);

    void setMarkers(iftImage *marker);
    void setMarkers(iftLabeledSet *marker);

    void setAnnotationVisibility(bool visible);

    int currentTime();

    QString getFileName();

    ArcWeightFunction *currentArcWeightFunction;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    //TODO remove it
    void updateAllGraphicalViews();

    void slotCalculateGradient();
    void slotResetGradient();

private slots:

    void zoomIn();
    void zoomOut();
    void normalSize();

    void slotChangeGradientMethod();

    /*=========== FILE MANAGEMENT ============*/
    void slotOpenImageSet();
    void slotOpenFile();
    void slotImportLabel();
    void slotExportLabel();
    void slotImportMarkers();
    void slotExportMarkers();
    void slotExportObjectsCSV();
    void slotExportImageSequence();
    /*========================================*/

    void loadNextImageFromImageSet();
    void loadPreviousImageFromImageSet();
    void saveLabel();

    void slotOpenProcessingOptions();

    /*======== IMAGE LINEAR TRANSFORM ========*/
    void slotAbout();
    void slotChangeBrightness();
    void slotChangeContrast();
    /*========================================*/

    /*========== VOLUME INFORMATION ==========*/
    void slotShowVolumeInformation();
    /*========================================*/

    /*============== RENDERING ===============*/
    void slotChangeObjectVisibility();
    void slotChangeObjectColor(int row, int col);
    void slotChangeObjectOpacity(QTableWidgetItem* item);
    void slotMarkAll();
    /*========================================*/

    /*============= ANNOTATION ===============*/
    void slotMarkerDoubleClick(int row, int col);
    void slotMarkerChanged(int row, int col);
    void slotChangeMarkerColor(int row);
    void slotChangeMarkerVisibility();
    void slotChangeMarkerVisibility(int row, bool visible);
    void slotAddNewMarker();
    void slotRemoveMarker();
    void slotUndoMarker();
    void slotDestroyMarkerSettingsWindow();
    void slotUpdateBrushRadius(float r);
    void slotUpdateMarkerName(int row,QString s);
    void slotUpdateMarkerColor(int row, QColor color);
    void slotStartAnnotation(int mode);
    void slotEraseAnnotation();
    void slotHaltAnnotation();
    void slotTableOpenMarkerClicked();
    void slotTableDeleteObjectClicked();
    /*========================================*/

    /*============ SEGMENTATION ==============*/
    void slotChangeSegmentationMethod(int index);
    /*========================================*/

    void slotShowVolumeTimeThumbnail(int time);
    void slotChangeVolume();
    void slotClickOnRenderingOpenCurve(int label);


private:
    Ui::MainWindow *ui;

    /*
     * Functions
     */    
    void scaleImage(double factor);
    void setIcons();
    void setActions();
    void createHotkeys();    
    void createActions();
    void createObjectTableActions();
    void createMarkersTableActions();
    void createMarkerSettingsConnections();
    void destroyActions();    
    void destroyMarkersTableActions();
    void destroyMarkerSettingsConnections();
//    void showAxialSlice(iftImage *s);
//    void showCoronalSlice(iftImage *s);
//    void showSagittalSlice(iftImage *s);
    void showProjection(iftImage *proj);
//    void placeOrientationIndicators();
    void initializeAllGraphicalViews();
//    void updateAllLines();
    void updateRendition();
    void loadListOfMarkers();
    void addItemInListOfMarkers();
    void addItemInListOfMarkers(int row, QString desc);
    void addBackgroundInListOfMarkers();

    void set2DVisualizationMode();
    void set3DVisualizationMode();

    void setMultiTemporalVisualizationMode();
    void setUniTemporalVisualizationMode();

    View *view;
    MarkerSettings *ms;
    QVector<Segmentation *> segmentationViews; // TODO remove

    int imageSetCurrentIndex, imageSetSize;
    QString     imageSetPath;
    QStringList imageSet;

    iftCSV *objectsCSV;

    /*
     * Variables
     */
    int angleAxial, angleCoronal, angleSagittal;
    double scaleFactor, abs_scaleFactor = 1.0;
    bool applyScale;
    // For slice visualization
    int viewMode = 1;

    // TODO refactor
    friend class RenderingView;

    iftImageForest *forest;

    /*
     * Windows
     */
    VolumeInformation *vol_info = nullptr;
    PostProcessing *postProcessingWindow = nullptr;

    /*
     * Declaration of QT structures
     */
    QImage *axial = nullptr;
    QImage *coronal = nullptr;
    QImage *sagittal = nullptr;
    QImage *projection = nullptr;
    QGraphicsLineItem *L[6] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    QGraphicsPixmapItem *P[4] = {nullptr,nullptr,nullptr,nullptr};
    QGraphicsTextItem *T[12] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    QPen *pen = nullptr;

    /*
     * Declaration of QT actions
     */
    QAction *openFileAct;
    QAction *openImageSetAct;
    QAction *importLabelAct;
    QAction *exportLabelAct;
    QAction *importMarkersAct;
    QAction *exportMarkersAct;
    QAction *exportImageSequenceAct;
    QAction *openVolumeInformationAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *aboutAct;
    QAction *lineVisibilityAct;
    QAction *processingAct;

    QAction *undoAction = nullptr;

    /*
     * Declaration of User Interface structures
     */
    QMenu *fileMenu;
    QMenu *importMenu;
    QMenu *exportMenu;
    QMenu *viewMenu;
    QMenu *processingMenu;
    QMenu *helpMenu;
    QMenu *methodsMenu;
    QMessageBox messageBox;
};

#endif // MAINWINDOW_H
