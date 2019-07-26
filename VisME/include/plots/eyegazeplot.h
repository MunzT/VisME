/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyegazeplot.h
 **************************************************************************************************/
#ifndef EYEGAZEPLOT_H
#define EYEGAZEPLOT_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianplot.h"
#include "settings.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a cartesian coordinate system.
 **************************************************************************************************/
class EyeGazePlot : public CartesianPlot, public AbstractEyeDataForCartesianPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    EyeGazePlot(Settings* settings);

    /*******************************************************************************************//**
     * Updates the plot settings for the given data.
     *
     * @param trials             all trials which shall be plotted
     * @param fixationsForTrials for each trial a list of its fixations
     **********************************************************************************************/
    void updatePlot(const std::vector<Trial*>& trials,
                    const std::vector<std::vector<Fixation*> >& fixationsForTrials);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);

    /*******************************************************************************************//**
     * Writes the marker value on the plot area.
     *
     * @param painter painter for drawing
     * @param color   color used for painting
     * @param max     value which is used as marker value
     * @param size    horizontal space used for drawing the text
     **********************************************************************************************/
    void drawMarker(QPainter& painter, const QColor color, qreal max, int size);
};

#endif // EYEGAZEPLOT_H
