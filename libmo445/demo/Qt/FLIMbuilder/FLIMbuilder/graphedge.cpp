#include "graphedge.h"
#include "graphnode.h"
#include "math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GraphEdge::GraphEdge(GraphNode* _sourceNode, GraphNode* _destNode)
{
    setAcceptedMouseButtons(Qt::NoButton);
    sourceNode = _sourceNode;
    destinationNode = _destNode;
    sourceNode->sourceEdges.push_back(this);
    destinationNode->destEdges.push_back(this);
    arrowSize = 1;
    lineWidth = 4;
    fillColor = QColor(200,200,200,255); //light gray
    setVisible(false);
    //edgeType = undirectedEdge;
    adjust();
}

GraphEdge::~GraphEdge()
{
    return;
}

void GraphEdge::adjust()
{
    if (!sourceNode || !destinationNode)
        return;

    QLineF line(mapFromItem(sourceNode, sourceNode->scale()/2, sourceNode->scale()/2), mapFromItem(destinationNode, destinationNode->scale()/2, destinationNode->scale()/2));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(6.)) {
        QPointF edgeOffset((line.dx() * qreal(sourceNode->radiusX)) / length, (line.dy() * qreal(sourceNode->radiusY)) / length);
        QPointF edgeOffsetDest((line.dx() * qreal(destinationNode->radiusX)) / length, (line.dy() * qreal(destinationNode->radiusY)) / length);
        sourcePoint = line.p1() + edgeOffset;
        destinationPoint = line.p2() - edgeOffsetDest;
    } else {
        sourcePoint = destinationPoint = line.p1();
    }
}

QRectF GraphEdge::boundingRect() const
{
    if (!sourceNode || !destinationNode)
        return QRectF();

    qreal penWidth = 0.5;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destinationPoint.x() - sourcePoint.x(),
                                      destinationPoint.y() - sourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (sourceNode == nullptr || destinationNode == nullptr)
        return;

    QLineF line(sourcePoint, destinationPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the border
    //painter->setPen(Qt::black);
    //painter->setPen(QPen(borderColor, lineWidth+0.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //painter->setRenderHint( QPainter::Antialiasing );
    //painter->drawLine(line);
    // Draw the line itself
    QPen pen(fillColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setRenderHint( QPainter::Antialiasing );
    painter->drawLine(line);

    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = (2*M_PI) - angle;

    if(arrowSize > 0){
        QPointF destArrowP1 = destinationPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                                         cos(angle - M_PI / 3) * arrowSize);
        QPointF destArrowP2 = destinationPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                                         cos(angle - M_PI + M_PI / 3) * arrowSize);
        painter->setBrush(fillColor);
        //    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
        painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
    }
}
