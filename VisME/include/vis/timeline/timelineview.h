/***********************************************************************************************//**
 * @author Tanja Munz
 * @file timelineview.h
 **************************************************************************************************/
#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include "abstractview.h"
#include "fixationtimeitem.h"
#include "microsaccadetimeitem.h"
#include "trial.h"
#include <QRubberBand>
#include <QSet>
#include <QWidget>

/***********************************************************************************************//**
 * This class shows the visualization and offers interaction.
 **************************************************************************************************/
class TimelineView: public AbstractView
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     *
     * Initializes the canvas with a white background.
     *
     * @param parent
     **********************************************************************************************/
    TimelineView(QWidget* parent = 0);

    /*******************************************************************************************//**
     * Resets the view, so that all items are visible and there is an invisible border around the
     * items.
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

signals:

    /*******************************************************************************************//**
     * Signal that an area of the timeline was selected with its minimum and maximum pixel value in
     * x direction.
     **********************************************************************************************/
    void timelineSelection(int min, int max);

protected:

    /*******************************************************************************************//**
     * Mouse wheel event: zooms in/out.
     *
     * @param event
     **********************************************************************************************/
    virtual void wheelEvent(QWheelEvent* event) override;

    /*******************************************************************************************//**
     * Resize event.
     *
     * @param event
     **********************************************************************************************/
    virtual void resizeEvent(QResizeEvent* event);

    /*******************************************************************************************//**
     * Shows a rubber band to select graphs
     *
     * @param event mouse event
     **********************************************************************************************/
    void mousePressEvent(QMouseEvent *event);

    /*******************************************************************************************//**
     * Changes the geometry of the rubber band.
     *
     * @param event mouse event
     **********************************************************************************************/
    void mouseMoveEvent(QMouseEvent *event);

    /*******************************************************************************************//**
     * Emits a signal with the rectangle of selected nodes and hides the
     * rubber band.
     *
     * @param event mouse event
     **********************************************************************************************/
    void mouseReleaseEvent(QMouseEvent *event);

private:

    /// Rubber band
    QRubberBand* rubberBand;

    /// Origin of the rubber band
    QPoint origin;

};

#endif
