/***********************************************************************************************//**
 * @author Tanja Munz
 * @file polarplot.h
 **************************************************************************************************/
#ifndef POLARPLOT_H
#define POLARPLOT_H

#include "radialhistogram.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a cartesian coordinate system.
 **************************************************************************************************/
class PolarPlot : public RadialHistogram
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    PolarPlot(Settings* settings, bool withHole);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);

};

#endif // POLARPLOT_H
