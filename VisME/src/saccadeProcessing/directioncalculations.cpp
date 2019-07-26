#include "directioncalculations.h"
#include "mathhelpfunctions.h"
#include "trial.h"


DirectionCalculations::DirectionCalculations()
{}


void DirectionCalculations::getAngularDataForTrial(Trial* trial,
                                                   PlotDirection dataPlotDirection,
                                                   PlotData dataPlotData,
                                                   std::vector<qreal>& anglesForTrial,
                                                   std::vector<Fixation*> fixations)
{
    const QMap<int, GazeData>& g = trial->gazeData();
    size_t fixationCount = 0;

    foreach (Fixation* fixation, fixations)
    {
        if (fixation == nullptr)
        {
            continue;
        }
        if (dataPlotData == PD_Microsaccades)
        {
            if (fixation->getMicrosaccadesCount(trial->msFromInputFile()) > 0)
            {
                for (size_t i = 0; i < fixation->getMicrosaccadesCount(trial->msFromInputFile()); ++i)
                {
                    Saccade* ms = fixation->getMicrosaccadeAt(i, trial->msFromInputFile());

                    Fixation* nextFixation = trial->getNextFixation(fixation);
                    Fixation* prevFixation = trial->getPrevFixation(fixation);

                    qreal microsaccadeAngle = MathHelpFunctions::getAngle(
                            g[int(ms->getOnsetIndex())].getXPosition(),
                            g[int(ms->getEndIndex())].getXPosition(),
                            g[int(ms->getOnsetIndex())].getYPosition(),
                            g[int(ms->getEndIndex())].getYPosition());

                    qreal angleInDegreesBetweenFixations = 0;
                    if (nextFixation != nullptr && dataPlotDirection == PDir_ToNextFixation)
                    {
                        angleInDegreesBetweenFixations = MathHelpFunctions::getAngle(
                                                                  fixation->getXPosition(),
                                                                  nextFixation->getXPosition(),
                                                                  fixation->getYPosition(),
                                                                  nextFixation->getYPosition());
                    }
                    else if (prevFixation != nullptr && dataPlotDirection == PDir_ToPreviousFixation)
                    {
                        angleInDegreesBetweenFixations = MathHelpFunctions::getAngle(
                                                                  fixation->getXPosition(),
                                                                  prevFixation->getXPosition(),
                                                                  fixation->getYPosition(),
                                                                  prevFixation->getYPosition());
                    }


                    if (dataPlotDirection == PDir_ToNextFixation ||
                            dataPlotDirection == PDir_ToPreviousFixation)
                    {
                        qreal angle = fmod(360 - angleInDegreesBetweenFixations +
                                           microsaccadeAngle, 360);
                        anglesForTrial.push_back(angle);
                    }
                    else
                    {
                        anglesForTrial.push_back(microsaccadeAngle);
                    }
                }
            }
        }
        else if (dataPlotData == PD_Saccades)
        {
            Fixation* nextFixation = trial->getNextFixation(fixation);
            qreal angleInDegreesBetweenFixations = 0;
            if (nextFixation != nullptr && fixationCount < fixations.size() - 1)
            {
                angleInDegreesBetweenFixations = MathHelpFunctions::getAngle(
                                                          fixation->getXPosition(),
                                                          nextFixation->getXPosition(),
                                                          fixation->getYPosition(),
                                                          nextFixation->getYPosition());

                if (dataPlotDirection == PDir_ToNextFixation)
                {
                    anglesForTrial.push_back(fmod(0, 360));
                }
                else if (dataPlotDirection == PDir_ToPreviousFixation)
                {
                    Fixation* prevFixation = trial->getPrevFixation(fixation);
                    if (prevFixation != nullptr)
                    {
                        qreal angleInDegreestoPrevFixation = MathHelpFunctions::getAngle(
                                                                      fixation->getXPosition(),
                                                                      prevFixation->getXPosition(),
                                                                      fixation->getYPosition(),
                                                                      prevFixation->getYPosition());

                        anglesForTrial.push_back(fmod(360 - angleInDegreestoPrevFixation
                                                      + angleInDegreesBetweenFixations, 360));
                    }
                }
                else
                {
                    anglesForTrial.push_back(angleInDegreesBetweenFixations);
                }
            }
        }
        else if (dataPlotData == PD_GazeRawData)
        {
            Fixation* nextFixation = trial->getNextFixation(fixation);
            Fixation* prevFixation = trial->getPrevFixation(fixation);
            qreal angleInDegreesBetweenFixations = 0;
            if (nextFixation != nullptr && dataPlotDirection == PDir_ToNextFixation)
            {
                angleInDegreesBetweenFixations = MathHelpFunctions::getAngle(
                                                          fixation->getXPosition(),
                                                          nextFixation->getXPosition(),
                                                          fixation->getYPosition(),
                                                          nextFixation->getYPosition());
            }
            else if (prevFixation != nullptr && dataPlotDirection == PDir_ToPreviousFixation)
            {
                angleInDegreesBetweenFixations = MathHelpFunctions::getAngle(
                                                          fixation->getXPosition(),
                                                          prevFixation->getXPosition(),
                                                          fixation->getYPosition(),
                                                          prevFixation->getYPosition());
            }

            const QMap<int, GazeData>& g = trial->gazeData();
            for (int i = fixation->getStartIndex(); i <= fixation->getEndIndex(); ++i) // -1 as there is no next point within this fixation for the last data point
            {
                if (g.contains(i))
                {
                    qreal gazePointAngle = MathHelpFunctions::getAngle(
                                                    g[i].getXPosition(),
                                                    g[i + 1].getXPosition(),
                                                    g[i].getYPosition(),
                                                    g[i + 1].getYPosition());

                    if (dataPlotDirection == PDir_ToNextFixation ||
                            dataPlotDirection == PDir_ToPreviousFixation)
                    {
                        anglesForTrial.push_back(fmod(360 - angleInDegreesBetweenFixations +
                                                 + gazePointAngle, 360));
                    }
                    else
                    {
                        anglesForTrial.push_back(gazePointAngle);
                    }
                }
            }
        }
        fixationCount++;
    }
}
