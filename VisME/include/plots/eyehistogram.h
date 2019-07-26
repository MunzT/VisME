/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyehistogram.h
 **************************************************************************************************/
#ifndef EYEHISTOGRAM_H
#define EYEHISTOGRAM_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianhistogram.h"
#include "settings.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a histogram for eye tracking data in the 
 * cartesian coordinate system.
 **************************************************************************************************/
class EyeHistogram : public CartesianHistogram
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    EyeHistogram(Settings* settings);

    /*******************************************************************************************//**
     * Updates the plot settings for the given data.
     *
     * @param trials             all trials which shall be plotted
     * @param fixationsForTrials for each trial a list of its fixations
     * @param saccadesForTrials  for each trial a list of its saccades
     **********************************************************************************************/
    void updatePlot(const std::vector<Trial*>& trials,
                    const std::vector<std::vector<Fixation*> >& fixationsForTrials,
                    const std::vector<std::vector<Saccade*> >& saccadesForTrials);

protected:

    /*******************************************************************************************//**
     * Draw function for the plot.
     *
     * @param painter painter for drawing
     **********************************************************************************************/
    void draw(QPainter& painter);

private:

    /// Contains the saccades that shall be considered for the plot.
    std::vector<std::vector<Saccade*> > m_saccadesForTrials;
};

#endif // EYEHISTOGRAM_H
