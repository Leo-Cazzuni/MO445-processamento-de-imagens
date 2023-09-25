#ifndef CONTROLLED_H
#define CONTROLLED_H

#include <QWidget>

#include <segmentation/segmentation.h>

namespace Ui {
class Controlled;
}

class Controlled : public Segmentation
{
    Q_OBJECT

public:
    Controlled(MainWindow *parent, View *view);
    ~Controlled();

    QList<iftImage*> generateLabel() override;
private:
    Ui::Controlled *ui;
    iftImage* propagateMarkersThroughTime(iftLabeledSet* markers, int t);
};

#endif // CONTROLLED_H
