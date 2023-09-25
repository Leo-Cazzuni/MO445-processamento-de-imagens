#ifndef GLOBAL_H
#define GLOBAL_H

#include <QGraphicsView>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QMouseEvent>
#include <QLocale>
#include <QImage>
#include <QIcon>
#include <QDateTime>
#include <QFontMetrics>
#include <QColorDialog>
#include <QRadialGradient>
#include <QTableWidgetItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QTextStream>
#include <QFile>

extern "C" {
#include "ift.h"
}

// For Volumetric image
#define AXIAL 1
#define CORONAL 2
#define SAGITTAL 3

// For Rendering
#define FAST_MODE 5
#define QUALITY_MODE 1

// For QImage Structure
#define QIMAGE_FORMAT QImage::Format_RGB888

// For Annotation
#define MAX_BRUSH 20
#define FREE_FORM_ANNOTATION 1
#define BOX_ANNOTATION 2
#define ERASING_ANNOTATION 3
#define HALT_ANNOTATION 4

const QString DEFAULT_FLIM_MODEL_DIR_KEY   = "default_flim_model_dir";
const QString DEFAULT_FLIM_MODEL_LIST_KEY  = "default_flim_model_list";
const QString DEFAULT_IMAGE_DIR_KEY        = "default_image_dir";
const QString DEFAULT_LABEL_DIR_KEY        = "default_label_dir";
const QString DEFAULT_MARKERS_DIR_KEY      = "default_markers_dir";
const QString DEFAULT_EXPORT_DIR_KEY       = "default_export_dir";
const QString DEFAULT_FLIMBUILDER_LOC_KEY  = "default_flimbuilder_location";
const QString DEFAULT_MIMG_LOC_KEY         = "default_mimg_location";
const QString DEFAULT_SALIENCY_LOC_KEY     = "default_saliency_location";

/**
 * @brief Returns a QImage generated from a iftImage
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms an iftImage into a QImage
 *
 * @param <img> Pointer to an image stored in a iftImage data structure
 * @return Same image stored in a QImage data structure
 */
QImage *iftImageToQImage(iftImage *img);

QImage *iftImageToQImage(iftImage *img, iftImage *label, iftColorTable *t, bool *obj_visibility, iftImage *markers, iftColorTable *tm);

QImage *iftImageToQImage(iftImage *img, iftImage *label, iftImage *border, iftColorTable *t, bool *obj_visibility, iftImage *markers, iftImage *toggle_markers, iftColorTable *tm, iftIntArray *markers_visibility);

/**
 * @brief Returns a colored QImage generated from a iftImage with its respective label map
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms an iftImage and its label map into a colored QImage
 *
 * @param <img> Pointer to an image stored in a iftImage data structure
 * @param <label> Pointer to a label map stored in a iftImage data structure
 * @param <t> Pointer to color table stores in a iftColorTable data structure
 * @param <obj_visibility> Pointer to a bool vector indicating if the object is visible or not
 * @return Same image stored in a QImage data structure
 */
QImage *iftLabeledImageToColoredQImage(iftImage *img, iftImage *label, iftColorTable *t, bool *obj_visibility);

/**
 * @brief Returns a colored QImage generated from a iftImage with its respective label map
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms an iftImage and its label map into a colored QImage
 *
 * @param <img> Pointer to an image stored in a iftImage data structure
 * @param <label> Pointer to a label map stored in a iftImage data structure
 * @param <t> Pointer to color table stores in a iftColorTable data structure
 * @return Same image stored in a QImage data structure
 */
QImage *iftLabeledImageToColoredQImage(iftImage *img, iftImage *label, iftColorTable *t);

/**
 * @brief Returns a colored QImage generated from a iftImage with its Cb and Cr channels
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms an iftImage and its label map into a colored QImage
 *
 * @param <img> Pointer to an image stored in a iftImage data structure
 * @param <label> Pointer to a label map stored in a iftImage data structure
 * @param <t> Pointer to color table stores in a iftColorTable data structure
 * @return Same image stored in a QImage data structure
 */
QImage *iftColoredImageToColoredQImage(iftImage *img);

/**
 * @brief Returns a iftImage generated from a QImage
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms a QImage into a iftImage
 *
 * @param <img> Image stored in a QImage data structure
 * @return Pointer to a copy of the QImage in a iftImage
 */
iftImage *iftQImageToImage(QImage *img);

/**
 * @brief Returns a QColor generated from an iftImage
 *
 * @author Azael de Melo e Sousa
 *
 * Transforms anm iftColor into a QImage
 *
 * @param <img> iftColor data structure
 * @return QColor data structure
 */
QColor iftColorToQColor(iftColor YCbCr);

/**
 * @brief Writes message on log
 *
 * @author Azael de Melo e Sousa
 *
 * Writes specif message on log.
 *
 * @param <opeartion_detail> QString, the operation performed
 * @param <elapsed> qint64, elapsed time
 * @return void
 */
void iftWriteOnLog(QString operation_detail, qint64 elapsed);

#define UNMARKED -1

void iftInitMarkersImage(iftImage *markers);

bool iftAreImagesEqual(const iftImage *img1, const iftImage *img2);

void iftSetObjectNormalParallelized(iftGraphicalContext *gc, char normal_type);

/**
@brief Computes the watershed transform in a forest structure.

Complexity: O(|A| * |V|).

@param  fst  iftImageForest.    Forest structure created with a gradient image.
@param  seed iftLabeledSet.     List of spels with image index and seed label.
@param  removal_markers iftSet. List of spels marked for removal. NULL if empty

@return void. All forest maps are updated by reference.
*/
void iftFastDiffWatershed(iftImageForest *fst, iftLabeledSet *seed, iftSet * removal_markers);

iftMImage *iftMGetZXSlice(const iftMImage *mimg, int ycoord);
iftMImage *iftMGetXYSlice(const iftMImage *mimg, int zcoord);
iftMImage *iftMGetYZSlice(const iftMImage *mimg, int xcoord);

iftImage *MImageGradient(const iftMImage *img, iftAdjRel *A);


#define CMAX     65535

typedef struct iftDynTrees_ {
  iftMImage *img;          /* original image in some color space */
  iftAdjRel *A;            /* adjacency relation */
  iftImage  *label;        /* label map */
  iftImage  *root;         /* root map */
  iftMImage *cumfeat;      /* cumulative feature vector map */
  int       *treesize;     /* tree size map */
  iftImage  *cost;         /* path cost map */
  iftImage  *pred;         /* predecessor map */
  iftGQueue *Q;            /* priority queue with integer costs */
  float      maxfeatdist;  /* maximum feature distance */
} iftDynTrees;

iftDynTrees *iftCreateDynTrees(iftMImage *img, iftAdjRel *A);
void         iftDestroyDynTrees(iftDynTrees **dt);
void         iftExecDiffDynTrees(iftDynTrees *dt, iftLabeledSet **S, iftSet **M);
iftSet      *iftDiffDynTreeRemoval(iftDynTrees *dt, iftSet **M);
void         iftDiffDynSubtreeUpdate(iftDynTrees *dt, int q);

#endif // GLOBAL_H
