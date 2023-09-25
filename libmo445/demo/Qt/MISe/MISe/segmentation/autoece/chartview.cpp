#include "chartview.h"

ChartView::ChartView(QWidget *&parent):QChartView(parent)
{

}

void ChartView::drawForeground(QPainter *painter, const QRectF &)
{
      painter->save();
      QColor color = QColor("indigo");

      color.setAlpha(50);

      QPen pen = QPen(color);
      pen.setWidth(3);
      painter->setPen(pen);

      QPointF p = this->chart()->mapToPosition(QPointF(270, 0));


      QRectF r = this->chart()->plotArea();

      QPointF p1 = QPointF(p.x(), r.top());
      QPointF p2 = QPointF(p.x(), r.bottom());
      painter->drawLine(p1, p2);

      painter->restore();


}
