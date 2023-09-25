#ifndef EROSION_H
#define EROSION_H

#include <QWidget>

#include <postprocessing/method.h>

namespace Ui {
class Erosion;
}

class Erosion : public Method
{
    Q_OBJECT

public:
    explicit Erosion(QWidget *parent = nullptr);
    ~Erosion();

private:
    Ui::Erosion *ui;

    // Method interface
public:
    iftImage *process(iftImage *img);
};

#endif // EROSION_H
