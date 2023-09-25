#ifndef DILATION_H
#define DILATION_H

#include <QWidget>
#include <ift.h>

#include <postprocessing/method.h>

namespace Ui {
class Dilation;
}

class Dilation : public Method
{
    Q_OBJECT

public:
    explicit Dilation(QWidget *parent = nullptr);
    ~Dilation();

    iftImage *process(iftImage *img) override;
private:
    Ui::Dilation *ui;
};

#endif // DILATION_H
