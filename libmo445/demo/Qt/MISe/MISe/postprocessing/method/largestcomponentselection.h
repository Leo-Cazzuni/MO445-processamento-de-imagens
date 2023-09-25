#ifndef LARGESTCOMPONENTSELECTION_H
#define LARGESTCOMPONENTSELECTION_H

#include <QWidget>

#include <postprocessing/method.h>

namespace Ui {
class LargestComponentSelection;
}

class LargestComponentSelection : public Method
{
    Q_OBJECT

public:
    explicit LargestComponentSelection(QWidget *parent = nullptr);
    ~LargestComponentSelection();

private:
    Ui::LargestComponentSelection *ui;

    // Method interface
public:
    iftImage *process(iftImage *img);
};

#endif // LARGESTCOMPONENTSELECTION_H
