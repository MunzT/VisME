/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialplot.h
 **************************************************************************************************/
#ifndef RADIALPLOT_H
#define RADIALPLOT_H

#include "abstractplot.h"
#include <QPainter>
#include <QWidget>

/***********************************************************************************************//**
 * This class is responsible for visualizing a radial plot.
 **************************************************************************************************/
class RadialPlot : public AbstractPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    RadialPlot();

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter) = 0;

    /*******************************************************************************************//**
     * Resize event.
     *
     * @param event resize event
     **********************************************************************************************/
    void resizeEvent(QResizeEvent* event);

    /*******************************************************************************************//**
     * Draws the inner circle of the plot (with the hole radius); for polar plot like plots.
     *
     * @param holeRadius radius in th emiddle of the plot where no axis shall be visuble
     * @param painter    painter for drawing
     * @param color      color for the circle
     **********************************************************************************************/
    void drawInnerCircle(const qreal holeRadius, QPainter& painter, const QColor color);

    /*******************************************************************************************//**
     * Draws the outer circle of the plot (with the plot radius); for polar plot like plots.
     *
     * @param painter    painter for drawing
     * @param color      color for the circle
     **********************************************************************************************/
    void drawOuterCircle(QPainter& painter, const QColor color);

    /*******************************************************************************************//**
     * Adds the minimum (is always 0) and maximum values to the plot.
     *
     * @param painter painter for drawing
     * @param color   color for the text
     * @param max     maximum value for the text
     * @param degree if the unit for degree shall be added
     **********************************************************************************************/
    void drawMinMax(QPainter& painter, const QColor color, qreal max, bool degree = false);

    /*******************************************************************************************//**
     * Adds the degree values 0, 90, 180 and 360 to the plot.
     *
     * @param painter painter for drawing
     * @param color   color for the text
     **********************************************************************************************/
    void drawValues(QPainter& painter, const QColor color);

    /*******************************************************************************************//**
     * Rotates the given point p by the given angle clockwise around the given origin.
     *
     * @param origin origin for the rotation
     * @param angle  angle for the rotation
     * @param p      point which will be rotated
     **********************************************************************************************/
    QPointF rotatePoint(QPointF origin, qreal angle, QPointF p);

    /*******************************************************************************************//**
     * Draws the axis of the plot in x and y direction with the origin in the center.
     *
     * @param holeRadius radius in the middle of the plot where no axis shall be visuble
     * @param plotRadius radius of the plot where the xamimum values shall be plotted
     * @param painter    painter for drawing
     * @param color      color for the axis
     * @param drawMarker if a marker shall be drawn to indicate the min/max position of the axis
     **********************************************************************************************/
    void drawAxis(const qreal holeRadius, const qreal plotRadius, QPainter& painter,
                  const QColor color, bool drawMarker = false);

    /*******************************************************************************************//**
     * Draws the mean value with confidence interval.
     *
     * @param painter painter for drawing
     * @param holeRadius radius in the middle of the plot where no axis shall be visuble
     * @param plotRadius radius of the plot where the xamimum values shall be plotted
     * @param angles     angle values that are used for determining the mean value
     * @param color      color used in drawing
     **********************************************************************************************/
    void drawMean(QPainter& painter, int plotRadius, int holeRadius, std::vector<qreal> angles,
                  const QColor color);

    /*******************************************************************************************//**
     * Determines the position on a circle with the given radius rotated by the given radius.
     *
     * @param r     radius
     * @param angle angle
     * @return position on a circle with the given radius rotated by the given radius
     **********************************************************************************************/
    QPointF ellipseCoordinates(const QRectF& r, qreal angle);

    // Some values for geometric layout of labels, ...
    int m_minSpace;
    int m_spacingV;
    int m_spacingH;
    int m_plotRadius;
    int m_padding;
    int m_padding2;

};

#endif // RADIALPLOT_H
