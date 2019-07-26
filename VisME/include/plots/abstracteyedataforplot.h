/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstracteyedataforplot.h
 **************************************************************************************************/
#ifndef ABSTRACTEYEDATAFORPLOT_H
#define ABSTRACTEYEDATAFORPLOT_H

#include "settings.h"
#include "trial.h"
#include <QPainter>
#include <QWidget>

/***********************************************************************************************//**
 * Abstract class for eye data used in plots.
 **************************************************************************************************/
class AbstractEyeDataForPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    AbstractEyeDataForPlot(Settings* settings);

    /*******************************************************************************************//**
     * Updates the plot settings for the given data.
     *
     * @param trials             all trials which shall be plotted
     * @param fixationsForTrials for each trial a list of its fixations
     **********************************************************************************************/
    void updatePlot(const std::vector<Trial*>& trials,
                    const std::vector<std::vector<Fixation*> >& fixationsForTrials);

    /*******************************************************************************************//**
     * Returns the maximum value for distances between fixation center and the distance to each of
     * the gaze data points/microsaccade start and end position of the corresponding fixation.
     *
     * @param fixationsForTrials for each trial a list of its fixations
     **********************************************************************************************/
    qreal determineMaxValue(const std::vector<std::vector<Fixation*> >& fixationsForTrials) const;

    /*******************************************************************************************//**
     * Returns a color for the given trial test condition if available, else an invalid color.
     *
     * @param testCondition test condition of trial
     * @return test condition color
     **********************************************************************************************/
    QColor getColor(const QString testCondition);

    /*******************************************************************************************//**
     * Changes the color of the painter according to the test condition of the trial and returns
     * the values for the pen and brush.
     *
     * @param painter                 painter for drawing
     * @param index                   index for test conditions from trials if PA_NoAggregation;
     *                                from given list if PA_ForTestConditions
     * @param testConditionsForAngles list of test conditions
     * @return color  values for the pen and brush
     **********************************************************************************************/
    QPair<QColor, QColor> changePainterForTestCondition(QPainter& painter, size_t index,
            std::vector<QString> testConditionsForAngles = std::vector<QString>());

    /*******************************************************************************************//**
     * Determines for each trial a lists which contains the number of microsaccades/gaze movements
     * (depending on the settings) in fixations towards each direction (aggregation bins are used).
     *
     * @param aggregation             if trial counts shall be added
     * @param testConditionsForAngles list of test conditions
     * @return number of microsaccades/gaze movements towards each direction for each trial
     **********************************************************************************************/
    std::vector<std::vector<qreal> > getAngleData(PlotAggregation aggregation,
                                                  std::vector<QString>& testConditionsForAngles);

    /*******************************************************************************************//**
     * Reorder m_trials and m_fixationsForTrials that trials with same test condition are grouped.
     **********************************************************************************************/
    void reorderTrialsByTestCondition();

protected:

    /// Contains settings for the plot.
    Settings* m_settings;

    /// Trials which contain data for the visualization.
    std::vector<Trial*> m_trials;

    /// Contains the fixations that shall be considered for the plot.
    std::vector<std::vector<Fixation*> > m_fixationsForTrials;
};

#endif // ABSTRACTEYEDATAFORPLOT_H
