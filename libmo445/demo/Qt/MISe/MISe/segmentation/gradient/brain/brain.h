#ifndef BRAIN_H
#define BRAIN_H

#include <QWidget>

#include <segmentation/gradient/arcweightfunction.h>

namespace Ui {
class Brain;
}

class Brain : public ArcWeightFunction
{
    Q_OBJECT

public:
    explicit Brain(QWidget *parent = nullptr);
    ~Brain();

public slots:
    void generate() override;

private:
    Ui::Brain *ui;
};

#endif // BRAIN_H
