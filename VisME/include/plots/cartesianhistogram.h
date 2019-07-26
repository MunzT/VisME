/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianhistogram.h
 **************************************************************************************************/
#ifndef CARTESIANHISTOGRAM_H
#define CARTESIANHISTOGRAM_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianplot.h"
#include <QMap>
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a histogram in the cartesian coordinate system.
 **************************************************************************************************/
class CartesianHistogram : public CartesianPlot, public AbstractEyeDataForCartesianPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    CartesianHistogram(Settings* settings);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter) = 0;

    /// Categorical data for the histogram.
    std::vector<std::vector<qreal> > m_data;

    /// Maximum values in x and y direction.
    qreal m_maxXValue;
    qreal m_maxYValue;

    /// Horizontal shift value of the histogram bars.
    int m_diagramShift;

    /// Colors for the different categories.
    QMap<int, QColor> m_idToColor;

};

#endif // CARTESIANHISTOGRAM_H
