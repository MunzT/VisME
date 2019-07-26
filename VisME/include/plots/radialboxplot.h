/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialboxplot.h
 **************************************************************************************************/
#ifndef RADIALBOXPLOT_H
#define RADIALBOXPLOT_H

#include "radialhistogram.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a radial box plot.
 **************************************************************************************************/
class RadialBoxPlot : public RadialHistogram
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    RadialBoxPlot(Settings* settings, bool withHole);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter) = 0;

};

#endif // RADIALBOXPLOT_H
