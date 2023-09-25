#ifndef VOLUMEINFORMATION_H
#define VOLUMEINFORMATION_H

#include <QDialog>
#include "global.h"

namespace Ui {
class VolumeInformation;
}

class VolumeInformation : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeInformation(QWidget *parent = nullptr);
    ~VolumeInformation();

    void showVolumeInformation(const iftImage *img, QString filename);

private:
    Ui::VolumeInformation *ui;
};

#endif // VOLUMEINFORMATION_H
