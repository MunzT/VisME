/***********************************************************************************************//**
 * @author Tanja Munz
 * @file roseplot.h
 **************************************************************************************************/
#ifndef ROSEPLOT_H
#define ROSEPLOT_H

#include "radialhistogram.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a rose plot.
 **************************************************************************************************/
class RosePlot : public RadialHistogram
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    RosePlot(Settings* settings, bool withHole, bool withArcs);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);


private:

    // Use arcs or boxes.
    bool m_withArcs;

};

#endif // ROSEPLOT_H
