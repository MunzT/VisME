/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstracteyedataforcartesianplot.h
 **************************************************************************************************/
#ifndef ABSTRACTEYEDATAFORCARTESIANPLOT_H
#define ABSTRACTEYEDATAFORCARTESIANPLOT_H

#include "abstracteyedataforplot.h"
#include "settings.h"
#include "trial.h"
#include <QPainter>
#include <QWidget>

/***********************************************************************************************//**
 * Abstract class for eye data used in plots.
 **************************************************************************************************/
class AbstractEyeDataForCartesianPlot : public AbstractEyeDataForPlot
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    AbstractEyeDataForCartesianPlot(Settings* settings);

    /*******************************************************************************************//**
     * Returns a value for the (micro)saccade according the the scatterplot settings
     *
     * @param saccade (micro)saccade
     * @param trial   current trial
     * @param type    type of data visible in the scatterplot
     * @param fixationStartIndex required for microsaccades to get start index of corresponding
     *                           fixation
     * @return value of the (micro)saccade
     **********************************************************************************************/
    qreal getValue(Saccade* saccade, Trial* trial, PlotValues type, int fixationStartIndex = 0);

    /*******************************************************************************************//**
     * Returns a description for the given histogram/scatterplot type.
     *
     * @param type type of data visible in the scatterplot
     * @return description for the given histogram/scatterplot type
     **********************************************************************************************/
    QString getDescription(PlotValues type);

};

#endif // ABSTRACTEYEDATAFORCARTESIANPLOT_H
