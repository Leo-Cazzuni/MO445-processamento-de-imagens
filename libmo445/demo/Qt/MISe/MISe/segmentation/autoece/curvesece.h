#ifndef CURVESECE_H
#define CURVESECE_H

#include <QWidget>

namespace Ui {
class CurvesECE;
}

class CurvesECE : public QWidget
{
    Q_OBJECT

public:
    explicit CurvesECE(QWidget *parent = nullptr);
    ~CurvesECE();

private:
    Ui::CurvesECE *ui;
};

#endif // CURVESECE_H
