#ifndef MORPHCLOSING_H
#define MORPHCLOSING_H

#include <QWidget>

#include <postprocessing/method.h>

namespace Ui {
class MorphClosing;
}

class MorphClosing : public Method
{
    Q_OBJECT

public:
    explicit MorphClosing(QWidget *parent = nullptr);
    ~MorphClosing();

    iftImage *process(iftImage *img) override;
private:
    Ui::MorphClosing *ui;
};

#endif // MORPHCLOSING_H
