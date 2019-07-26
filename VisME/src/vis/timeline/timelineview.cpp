/***********************************************************************************************//**
 * @author Tanja Munz
 * @file Timelineview.cpp
 **************************************************************************************************/
#include "timelineview.h"
#include <QGraphicsView>
#include <QtWidgets>


TimelineView::TimelineView(QWidget* parent) :
    AbstractView(parent),
    rubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
    //general settings
    setCacheMode(CacheBackground);

    // indicates that edges of primitives should be anti aliased
    setRenderHint(QPainter::Antialiasing);

    // holds the alignment of the scene in the center of the view when the whole
    // scene is visible.
    setAlignment(Qt::AlignCenter);

    // the scene point at the center of the view is used as the anchor;
    // to zoom to the mouse position
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // the cursor changes into a pointing hand; dragging the mouse around will
    // scroll the scrollbars
    setDragMode(QGraphicsView::ScrollHandDrag);

    setOptimizationFlags(QGraphicsView::DontSavePainterState);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    setMouseTracking(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


void TimelineView::resetView()
{
    fitInView(sceneRect(), Qt::IgnoreAspectRatio);

    updateAll();
}


void TimelineView::zoomIn()
{
    scaleView(qreal(1.2), 1);
}


void TimelineView::zoomOut()
{
    scaleView(1 / qreal(1.2), 1);
}


void TimelineView::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(qreal(2.0), event->delta() / 240.0), 1);
}


void TimelineView::resizeEvent(QResizeEvent* event)
{
    QRectF r = sceneRect();
    QGraphicsView::resizeEvent(event);
    fitInView(r, Qt::IgnoreAspectRatio);
}


void TimelineView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::ALT)
    {
        if (event->button() == Qt::LeftButton) {
             origin = event->pos();
             if (!rubberBand)
                 rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
             rubberBand->setGeometry(QRect(QPoint(origin.x(), 0), QPoint(origin.x(), height())));
             rubberBand->show();
        }
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}


void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::ALT)
    {
        rubberBand->setGeometry(QRect(QPoint(origin.x(), 0),
                                      QPoint(event->pos().x(), height())).normalized());
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}


void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->modifiers() == Qt::ALT)
    {
        rubberBand->hide();
        QRectF rect = mapToScene(rubberBand->geometry()).boundingRect();
        int val1 = int(rect.left());
        int val2 = int(rect.right());
        emit timelineSelection(std::min(val1, val2), std::max(val1, val2));
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}
