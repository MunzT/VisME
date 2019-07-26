/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstractview.h
 **************************************************************************************************/
#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

/***********************************************************************************************//**
 * This abstract class provides general functionality of a view.
 **************************************************************************************************/
class AbstractView: public QGraphicsView
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    AbstractView(QWidget* parent);

    /*******************************************************************************************//**
     * Resets the view, so that all items are visible and there is an invisible
     * border around the items
     **********************************************************************************************/
    void resetView();

    /*******************************************************************************************//**
     * Scales the view with a factor to zoom in.
     **********************************************************************************************/
    void zoomIn();

    /*******************************************************************************************//**
     * Scales the view with a factor to zoom out.
     **********************************************************************************************/
    void zoomOut();

    /*******************************************************************************************//**
     * Updates the entire vie (with background);
     **********************************************************************************************/
    void updateAll();

protected:

    /*******************************************************************************************//**
     * Mouse wheel event: zooms in/out.
     *
     * @param event
     **********************************************************************************************/
    virtual void wheelEvent(QWheelEvent* event) override;

    /*******************************************************************************************//**
     * Scales the view, there are different factors for the x and y
     * direction possible.
     *
     * @param scaleFactor1 the factor that is used to scale the view in
     *                     x direction
     * @param scaleFactor2 the factor that is used to scale the view in
     *                     y direction (if scaleFactor = -1 x and y are
     *                     scaled equally)
     **********************************************************************************************/
    void scaleView(qreal scaleFactor1, qreal scaleFactor2 = -1);

};

#endif // ABSTRACTVIEW_H
