/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialscatterplot.h
 **************************************************************************************************/
#ifndef RADIALSCATTERPLOT_H
#define RADIALSCATTERPLOT_H

#include "abstracteyedataforplot.h"
#include "radialplot.h"
#include <QPainter>

/***********************************************************************************************//**
 * This class is responsible for visualizing a radial scatterplot.
 **************************************************************************************************/
class RadialScatterplot : public RadialPlot, public AbstractEyeDataForPlot
{

public:

    /// Element used for directional plots; it contains angle and length data as well weather the
    /// data belongs to a microsaccade or not.
    struct LengthElement
    {
        qreal angle;
        qreal length;
        bool fromMicrosaccade;
    };

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    RadialScatterplot(Settings* settings, bool connected);

    /*******************************************************************************************//**
     * Updates the plot settings for the given data.
     *
     * @param trials   all trials which shall be plotted
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

private:

    /*******************************************************************************************//**
     * Determines for each fixation a lists of elements without temporal break containing a list of
     * elements with a length, angle and a property if the corresponding sample belongs to a
     * microsaccade.
     *
     * @return for each fixation a lists of elements without temporal break containing a list of
     *         length elements
     **********************************************************************************************/
    std::vector<std::vector<std::vector<LengthElement> > > getDirectionalLengthData();

    /// Elements in the same inner vector shall be connected.
    bool m_connected;

};

#endif // RADIALSCATTERPLOT_H
