/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyescatterplot.h
 **************************************************************************************************/
#ifndef EYESCATTERPLOT_H
#define EYESCATTERPLOT_H

#include "abstracteyedataforcartesianplot.h"
#include "cartesianscatterplot.h"
#include "settings.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a scatterplot in the cartesian coordinate system.
 **************************************************************************************************/
class EyeScatterplot : public CartesianScatterplot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    EyeScatterplot(Settings* settings);

    /*******************************************************************************************//**
     * Updates the plot settings for the given data.
     *
     * @param trials   all trials which shall be plotted
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

#endif // EYESCATTERPLOT_H
