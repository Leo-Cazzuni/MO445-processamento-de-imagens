#include "qmywidget.h"

QMyWidget::QMyWidget(QWidget *parent) :
    QWidget(parent)
{

}

QMyWidget::~QMyWidget()
{

}

void QMyWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit(WidgetDoubleClicked());
}

/*
 * In order to set the stylesheet of a custom object inherited from
 * QWidget, one must reimplement the paint event of the object.
 * This implementation was extracted from StackOverflow.
 */
void QMyWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
