#ifndef QMYWIDGET_H
#define QMYWIDGET_H

#include "global.h"

class QMyWidget: public QWidget
{    
    Q_OBJECT

public:
    explicit QMyWidget(QWidget *parent = 0);
    ~QMyWidget();

signals:
    void WidgetDoubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

};

#endif // QMYWIDGET_H
