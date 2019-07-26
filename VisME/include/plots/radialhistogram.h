/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialhistogram.h
 **************************************************************************************************/
#ifndef RADIALHISTOGRAM_H
#define RADIALHISTOGRAM_H

#include "abstracteyedataforplot.h"
#include "radialplot.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a radial histogram.
 **************************************************************************************************/
class RadialHistogram : public RadialPlot, public AbstractEyeDataForPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    RadialHistogram(Settings* settings, bool withHole);

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
    void draw(QPainter& painter) = 0;

    // An inner empty circle should be added to the graph.
    bool m_withHole;

};

#endif // RADIALHISTOGRAM_H
