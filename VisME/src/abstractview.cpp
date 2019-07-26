/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstractview.cpp
 **************************************************************************************************/
#include "abstractview.h"
#include <QApplication>
#include <cmath>


AbstractView::AbstractView(QWidget* parent) :
    QGraphicsView(parent)
{}


void AbstractView::resetView()
{
    resetTransform();

    updateAll();
}


void AbstractView::zoomIn()
{
    scaleView(qreal(1.2));
}


void AbstractView::zoomOut()
{
    scaleView(1 / qreal(1.2));
}


void AbstractView::updateAll()
{
    if (scene() != nullptr)
    {
        invalidateScene(mapToScene(rect()).boundingRect(), QGraphicsScene::BackgroundLayer);
    }
    update();
}


void AbstractView::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(qreal(2), event->delta() / 240.0));
}


void AbstractView::scaleView(qreal scaleFactor1, qreal scaleFactor2)
{
    if (scaleFactor2 < 0)
    {
        scaleFactor2 = scaleFactor1;
    }

    qreal widthFactor = transform().scale(scaleFactor1, scaleFactor2).mapRect(
                            QRectF(0, 0, 1, 1)).width();
    qreal heightFactor = transform().scale(scaleFactor1, scaleFactor2).mapRect(
                             QRectF(0, 0, 1, 1)).height();

    if (widthFactor < 1.0 / 1000.0 || widthFactor > 300 ||
            heightFactor < 1.0 / 1000.0 || heightFactor > 300)
    {
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    scale(scaleFactor1, scaleFactor2);

    updateAll();

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    qApp->restoreOverrideCursor();
}
