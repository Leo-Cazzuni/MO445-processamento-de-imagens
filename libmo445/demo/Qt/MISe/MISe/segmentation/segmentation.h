#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <QWidget>
#include <segmentation/gradient/arcweightfunction.h>
#include <views/view.h>

class ArcWeightFunction;
class MainWindow;

#define PROGRESS_STEPS 100

class View;

class Segmentation : public QWidget
{
    Q_OBJECT
public:
    Segmentation(MainWindow *parent, View *view);
    Segmentation(const Segmentation &);
    Segmentation &operator=(const Segmentation &) = default;
    ~Segmentation();

    QString name();

    virtual void notifyGradientUpdate(ArcWeightFunction *function);

    virtual void renderGraphics(iftImage *orig, iftImage *image, QGraphicsPixmapItem *imagePixmap, int sliceType);
    virtual bool mouseClickGraphics(int x, int y, Qt::MouseButtons bts, Qt::KeyboardModifiers modifiers, int sliceType);
    virtual bool mouseMoveGraphics(int x, int y, int sliceType);
public slots:
    virtual void execute();

    virtual void notifyImageUpdate();

protected slots:
    void finishSegmentation();

signals:
    void segmentationFinished();
    void progress(int p);
    void textProgress(QString msg);

protected:
    void showEvent(QShowEvent *event) override;

    View *view;
    MainWindow *parent;

    bool annotationVisible;

    QString _name;

    MarkerInformationVector *labelInfo;

    virtual void updateLabel(QList<iftImage*> label);
    virtual QList<iftImage*> generateLabel() = 0;

    void updateProgress(float p, QString msg = "");

    friend class ExecuteWork;
};

Q_DECLARE_METATYPE(Segmentation*)



#endif // SEGMENTATION_H
