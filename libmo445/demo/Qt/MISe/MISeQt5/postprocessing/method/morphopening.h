#ifndef MORPHOPENING_H
#define MORPHOPENING_H

#include <QWidget>

#include <postprocessing/method.h>

namespace Ui {
class MorphOpening;
}

class MorphOpening : public Method
{
    Q_OBJECT

public:
    explicit MorphOpening(QWidget *parent = nullptr);
    ~MorphOpening();

private:
    Ui::MorphOpening *ui;

    // Method interface
public:
    iftImage *process(iftImage *img) override;
};


#endif // MORPHOPENING_H
