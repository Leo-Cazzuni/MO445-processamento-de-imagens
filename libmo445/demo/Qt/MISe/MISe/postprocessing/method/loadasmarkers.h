#ifndef LOADASMARKERS_H
#define LOADASMARKERS_H

#include <QWidget>

#include <postprocessing/method.h>

namespace Ui {
class LoadAsMarkers;
}

class LoadAsMarkers : public Method
{
    Q_OBJECT

public:
    explicit LoadAsMarkers(QWidget *parent = nullptr);
    ~LoadAsMarkers();

private:
    Ui::LoadAsMarkers *ui;

    // Method interface
public:
    iftImage *process(iftImage *img);
};

#endif // LOADASMARKERS_H
