/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialhistogram.cpp
 **************************************************************************************************/
#include "radialhistogram.h"


RadialHistogram::RadialHistogram(Settings* settings, bool withHole) :
    AbstractEyeDataForPlot(settings), m_withHole(withHole)
{}


void RadialHistogram::updatePlot(const std::vector<Trial*>& trials,
                                   const std::vector<std::vector<Fixation*> >& fixationsForTrials)
{
    AbstractEyeDataForPlot::updatePlot(trials, fixationsForTrials);

    show();
    update();
}
