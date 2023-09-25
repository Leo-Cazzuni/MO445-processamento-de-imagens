#ifndef MANUALSEGMENTATION_H
#define MANUALSEGMENTATION_H

#include <QWidget>

#include <segmentation/segmentation.h>

namespace Ui {
class ManualSegmentation;
}

class ManualSegmentation : public Segmentation
{
    Q_OBJECT

public:
    ManualSegmentation(MainWindow *parent, View *view);
    ~ManualSegmentation();

private slots:
    void reset();

protected:
    iftImage *generateLabel() override;

private:
    Ui::ManualSegmentation *ui;
};

#endif // MANUALSEGMENTATION_H
