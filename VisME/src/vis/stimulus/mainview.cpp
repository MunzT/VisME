/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainview.cpp
 **************************************************************************************************/
#include "mainview.h"


MainView::MainView(QWidget* parent) :
    AbstractView(parent)
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
}
