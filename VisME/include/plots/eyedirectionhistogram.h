/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedirectionhistogram.h
 **************************************************************************************************/
#ifndef EYEDIRECTIONHISTOGRAM_H
#define EYEDIRECTIONHISTOGRAM_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianhistogram.h"
#include "settings.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a histogram with eye tracking data in the
 * cartesian coordinate system.
 **************************************************************************************************/
class EyeDirectionHistogram : public CartesianHistogram
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    EyeDirectionHistogram(Settings* settings);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);

};

#endif // EYEDIRECTIONHISTOGRAM_H
