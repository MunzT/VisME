/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianplot.h
 **************************************************************************************************/
#ifndef CARTESIANPLOT_H
#define CARTESIANPLOT_H

#include "abstractplot.h"
#include "enums.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a cartesian coordinate system.
 **************************************************************************************************/
class CartesianPlot : public AbstractPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    CartesianPlot();

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter) = 0;

    /*******************************************************************************************//**
     * Draws the axis of the plot in x and y direction with the origin on the bottom left; labels
     * and maerkers are added.
     *
     * @param plotSize      dimension of the plot in x/y direction
     * @param binSize       size of one bin wich is marked on the x axis
     * @param leftText      text for the y axis
     * @param rightText     text for the x axis
     * @param smallxVal     value of one bin
     * @param maxxVal       max x value at plotSize
     * @param maxyVal       max y value at plotSize
     * @param painter       painter for drawing
     * @param color         color for the axis
     * @param drawMaxMarker if a marker shall be drawn to indicate the max position of the axis
     * @param drawMinMarker if a marker shall be drawn to indicate the binSize in x direction and
     *                      one unit in y direction of the axis
     **********************************************************************************************/
    void drawAxis(const int plotSize, const int binSize, const QString leftText,
                  const QString rightText, qreal smallxVal, qreal maxxVal, qreal maxyVal,
                  QPainter& painter, const QColor color, bool drawMaxMarker = false,
                  bool drawMinMarker = false);

    /*******************************************************************************************//**
     * Draws horizontally centered coordinate axis.
     * @param painter         painter for drawing
     * @param plotSize        dimension of the plot in x/y direction
     * @param color           color for the axis
     * @param dataDescription type of data for the legend
     * @param maxyVal         max y value at plotSize
     **********************************************************************************************/
    void drawHorizontallyCenteredAxis(QPainter& painter, const int plotSize, const QColor color,
                                      const QString dataDescription, qreal maxyVal);

    /*******************************************************************************************//**
     * Draws centered coordinate axis.
     * @param holeRadius hole radius of the plot
     * @param plotRadius radius of the outer circle of the plot
     * @param painter    painter for drawing
     * @param color      color for the axis
     * @param drawMarker if a markers shall be drawn to indicate the min/max position of the axis
     **********************************************************************************************/
    void drawCenteredAxis(const qreal holeRadius, const qreal plotRadius,
                          QPainter& painter, const QColor color, bool drawMarker);

    /*******************************************************************************************//**
     * Sets some values used for plotting (plot size, spaces, ...)
     **********************************************************************************************/
    void setPlotSize();

protected:

    /*******************************************************************************************//**
     * Returns the coordinates in the visualization for the given x and y values.
     * Helpfunctions to get correct plot values of points (scaled and mirrored on the origin)
     *
     * @param x x value
     * @param y y value
     * @return point in visualization
     **********************************************************************************************/
    QPointF getPointInVis(qreal x, qreal y);

    /*******************************************************************************************//**
     * Returns the y position in the visualization for the y value.
     *
     * @param y y value
     * @return y position in the visualization
     **********************************************************************************************/
    qreal getYValueInVis(qreal y);

    /*******************************************************************************************//**
     * Returns the length in the visualization for the given value in y direction.
     *
     * @param y value
     * @return length in y direction in the visualization
     **********************************************************************************************/
    qreal getYLength(qreal y);

    /*******************************************************************************************//**
     * Returns the x position in the visualization for the y value.
     *
     * @param x x value
     * @return x position in the visualization
     **********************************************************************************************/
    qreal getXValueInVis(qreal x);

    // Scale and size values for drawing,
    qreal m_xScale;
    qreal m_yScale;

    int m_plotSize;
    int m_spacingV;
    int m_spacingH;
    int m_minSpace;

};

#endif // CARTESIANPLOT_H
