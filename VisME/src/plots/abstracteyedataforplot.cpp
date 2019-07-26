/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstracteyedataforplot.cpp
 **************************************************************************************************/
#include "abstracteyedataforcartesianplot.h"
#include "directioncalculations.h"
#include <QSet>
#include <cmath>


AbstractEyeDataForPlot::AbstractEyeDataForPlot(Settings* settings) :
    m_settings(settings)
{}


void AbstractEyeDataForPlot::updatePlot(const std::vector<Trial*>& trials,
                              const std::vector<std::vector<Fixation*> >& fixationsForTrials)
{
    m_trials = trials;
    m_fixationsForTrials = fixationsForTrials;
}


qreal AbstractEyeDataForPlot::determineMaxValue(
        const std::vector<std::vector<Fixation*> >& fixationsForTrials) const
{
    qreal max = 0;

    if (m_settings->m_dataPlotData == PD_Microsaccades)
    {
        for (size_t j = 0; j < fixationsForTrials.size(); ++j)
        {
            qreal pixelsPerDegree = m_trials[j]->getParticipant()->getPixelsPerDegree();

            const QMap<int, GazeData>&  g = m_trials[j]->gazeData();
            for (size_t k = 0; k < fixationsForTrials[j].size(); ++k)
            {
                for (size_t l = 0; l < fixationsForTrials[j][k]->
                     getMicrosaccadesCount(m_trials[j]->msFromInputFile()); ++l)
                {

                    Saccade* m = fixationsForTrials[j][k]->
                                 getMicrosaccadeAt(l, m_trials[j]->msFromInputFile());

                    max = std::max(max, sqrt(pow(g[int(m->getOnsetIndex())].getXPosition() -
                                                 fixationsForTrials[j][k]->getXPosition(), 2) +
                                             pow(g[int(m->getOnsetIndex())].getYPosition() -
                                                 fixationsForTrials[j][k]->getYPosition(), 2)) / pixelsPerDegree);
                    max = std::max(max, sqrt(pow(g[int(m->getEndIndex())].getXPosition() -
                                                 fixationsForTrials[j][k]->getXPosition(), 2) +
                                             pow(g[int(m->getEndIndex())].getYPosition() -
                                                 fixationsForTrials[j][k]->getYPosition(), 2)) / pixelsPerDegree);
                }
            }
        }
    }
    else if (m_settings->m_dataPlotData == PD_Saccades)
    {
        for (size_t j = 0; j < fixationsForTrials.size(); ++j)
        {
            qreal pixelsPerDegree = m_trials[j]->getParticipant()->getPixelsPerDegree();
            if (fixationsForTrials[j].size() <= 1)
            {
                continue;
            }
            for (size_t k = 0; k < fixationsForTrials[j].size() - 1; ++k)
            {
                Fixation* startFixation = fixationsForTrials[j][k];
                Fixation* endFixation = fixationsForTrials[j][k + 1];

                // connection between centers of fixations
                max = std::max(max, std::sqrt(pow(endFixation->getXPosition() - startFixation->getXPosition(), 2) +
                                              pow(endFixation->getYPosition() - startFixation->getYPosition(), 2)) / pixelsPerDegree);

                // connection between start and end point of fixations
                //max = std::max(max, sqrt(pow(g[endFixation->getStartIndex()].getXPosition() -
                //                             g[startFixation->getEndIndex()].getXPosition(), 2) +
                //                         pow(g[endFixation->getStartIndex()].getYPosition() -
                //                             g[startFixation->getEndIndex()].getYPosition(), 2)) / pixelsPerDegree);
            }
        }
    }
    else if (m_settings->m_dataPlotData == PD_GazeRawData)
    {
        for (size_t j = 0; j < fixationsForTrials.size(); ++j)
        {
            qreal pixelsPerDegree = m_trials[j]->getParticipant()->getPixelsPerDegree();
            const QMap<int, GazeData>& g = m_trials[j]->gazeData();
            for (size_t k = 0; k < fixationsForTrials[j].size(); ++k)
            {
                Fixation* fixation = fixationsForTrials[j][k];

                 // -1 as there is no next point within this fixation for the last data point
                for (int l = fixation->getStartIndex(); l <= fixation->getEndIndex(); ++l)
                {
                    max = std::max(max, sqrt(pow(g[l].getXPosition() -
                                                 fixationsForTrials[j][k]->getXPosition(), 2) +
                                             pow(g[l].getYPosition() -
                                                 fixationsForTrials[j][k]->getYPosition(), 2)) / pixelsPerDegree);
                }
            }
        }
    }

    return max;
}


QColor AbstractEyeDataForPlot::getColor(const QString testCondition)
{
    if (testCondition.isNull() || testCondition.isEmpty())
    {
        return QColor(); // invalid color
    }

    return m_settings->m_testConditions.value(testCondition);
}


QPair<QColor, QColor> AbstractEyeDataForPlot::changePainterForTestCondition(
        QPainter& painter, size_t index, std::vector<QString> testConditionsForAngles)
{
    QString testCondition;
    if (m_settings->m_overplot == PA_NoAggregation && m_settings->m_testConditionColors)
    {
        testCondition = m_trials[index]->getTestConditionType();
    }
    else if (m_settings->m_overplot == PA_ForTestConditions && m_settings->m_testConditionColors)
    {
        if (testConditionsForAngles.size() > int(index))
        {
            testCondition = testConditionsForAngles[index];
        }
    }

    // there exist values for each trial
    if (!testCondition.isEmpty())
    {
        QColor testConditionColor = m_settings->m_testConditions[testCondition];
        QColor testConditionColorTransparent = testConditionColor;
        testConditionColorTransparent.setAlpha(m_settings->m_colors.alphaForPlots);

        painter.setPen(testConditionColor);
        painter.setBrush(testConditionColorTransparent);

        return QPair<QColor, QColor>(testConditionColor, testConditionColorTransparent);
    }

    return QPair<QColor, QColor>(QColor(), QColor());
}


std::vector<std::vector<qreal> > AbstractEyeDataForPlot::getAngleData(PlotAggregation aggregation,
                                                                      std::vector<QString>& testConditionsForAngles)
{
    std::vector<qreal> anglesForTrial;
    QMap<QString, std::vector<qreal> > anglesForTrialForSelections;
    std::vector<std::vector<qreal>> allAngles;
    QMap<QString, int> testConditions; // just keys are used

    if (m_trials.size() != m_fixationsForTrials.size())
    {
        return allAngles;
    }

    auto dirCalc = new DirectionCalculations();
    for (size_t i = 0; i < m_trials.size(); ++i)
    {
        Trial* currentTrial = m_trials[i];

        switch(aggregation)
        {
            case PA_NoAggregation:
                anglesForTrial.clear();

                dirCalc->getAngularDataForTrial(currentTrial, m_settings->m_dataPlotDirection,
                                                m_settings->m_dataPlotData, anglesForTrial,
                                                m_fixationsForTrials[i]);

                allAngles.push_back(anglesForTrial);

                break;
            case PA_All:

                dirCalc->getAngularDataForTrial(currentTrial, m_settings->m_dataPlotDirection,
                                                m_settings->m_dataPlotData, anglesForTrial,
                                                m_fixationsForTrials[i]);

                break;
            case PA_ForTestConditions:

                dirCalc->getAngularDataForTrial(currentTrial, m_settings->m_dataPlotDirection,
                                                m_settings->m_dataPlotData,
                                                anglesForTrialForSelections[currentTrial->getTestConditionType()],
                                                m_fixationsForTrials[i]);
                testConditions[currentTrial->getTestConditionType()] = 0;
                break;
            case PA_ForTrials:
                dirCalc->getAngularDataForTrial(currentTrial, m_settings->m_dataPlotDirection,
                                                m_settings->m_dataPlotData,
                                                anglesForTrialForSelections[currentTrial->name()],
                                                m_fixationsForTrials[i]);

                break;
            case PA_ForParticipants:

                dirCalc->getAngularDataForTrial(currentTrial, m_settings->m_dataPlotDirection,
                                                m_settings->m_dataPlotData,
                                                anglesForTrialForSelections[currentTrial->getParticipant()->name()],
                                                m_fixationsForTrials[i]);
                break;
        }
    }

    if (aggregation == PA_All)
    {
        allAngles.push_back(anglesForTrial);
    }

    if (aggregation == PA_ForTestConditions || aggregation == PA_ForTrials ||
            aggregation == PA_ForParticipants)
    {
        for(const QString& type : anglesForTrialForSelections.keys())
        {
            allAngles.push_back(anglesForTrialForSelections[type]);
        }
    }

    for(const QString& type : testConditions.keys())
    {
        testConditionsForAngles.push_back(type);
    }

    return allAngles;
}


void AbstractEyeDataForPlot::reorderTrialsByTestCondition()
{
    bool testConditionFound = false;
    std::vector<Trial*> tempTrials;
    std::vector<std::vector<Fixation*> > tempFixationsForTrials;

    QMultiMap<QString, int> trialsForTestConditions;

    for (size_t i = 0; i < m_trials.size(); ++i)
    {
        trialsForTestConditions.insert(m_trials[i]->getTestConditionType(), int(i));
    }

    QMutableMapIterator<QString, int> iter(trialsForTestConditions);
    while(iter.hasNext())
    {
        int val = iter.next().value();
        tempTrials.push_back(m_trials[size_t(val)]);
        tempFixationsForTrials.push_back(m_fixationsForTrials[size_t(val)]);
        testConditionFound = testConditionFound || m_trials[size_t(val)]->getTestConditionType() != QString();
    }

    if (testConditionFound)
    {
        m_trials = tempTrials;
        m_fixationsForTrials = tempFixationsForTrials;
    }
}
