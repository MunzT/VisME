/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstracteyedataforcartesianplot.cpp
 **************************************************************************************************/
#include "abstracteyedataforcartesianplot.h"
#include <cmath>


AbstractEyeDataForCartesianPlot::AbstractEyeDataForCartesianPlot(Settings* settings) : AbstractEyeDataForPlot(settings)
{}


qreal AbstractEyeDataForCartesianPlot::getValue(Saccade* saccade, Trial* trial, PlotValues type,
                                                int fixationStartIndex)
{
    if (type == MSStart)
    {
        // microsaccade start positions
        return(trial->samplesToTime(int(saccade->getOnsetIndex()) - fixationStartIndex));
    }
    else if (type == PeakVelocity)
    {
        return saccade->getPeakVelocity();
    }
    else if (type == Amplitude)
    {
        return saccade->getAmplitude();
    }
    else if (type == Duration)
    {
        return trial->samplesToTime(int(saccade->getDuration()));
    }
    return 0.0;
}


QString AbstractEyeDataForCartesianPlot::getDescription(PlotValues type)
{
    if (type == MSStart)
    {
        return "start time in Fixation [ms]";
    }
    else if (type == PeakVelocity)
    {
        return "Peak Velocity [°/s]";
    }
    else if (type == Amplitude)
    {
        return "Amplitude [°]";
    }
    else if (type == Duration)
    {
        return "Duration [ms]";
    }
    return "";
}
