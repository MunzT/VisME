/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianscatterplot.h
 **************************************************************************************************/
#ifndef CARTESIANSCATTERPLOT_H
#define CARTESIANSCATTERPLOT_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianplot.h"
#include "settings.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a scatterplot in the cartesian coordinate system.
 **************************************************************************************************/
class CartesianScatterplot : public CartesianPlot, public AbstractEyeDataForCartesianPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    CartesianScatterplot(Settings* settings);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);

    /// x values for the scatterplot.
    std::vector<qreal> m_xValues;

    /// y values for the scatterplot.
    std::vector<qreal> m_yValues;

    /// Color used for the points (x,y) with the same index.
    std::vector<QColor> m_colorValues;

    /// Maximum values in x and y direction
    qreal m_maxValX = 0;
    qreal m_minValX = 0;
    qreal m_maxValY = 0;
    qreal m_minValY = 0;

};

#endif // CARTESIANSCATTERPLOT_H
