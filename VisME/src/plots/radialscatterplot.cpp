/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialscatterplot.cpp
 **************************************************************************************************/
#include "radialscatterplot.h"
#include "mathhelpfunctions.h"


RadialScatterplot::RadialScatterplot(Settings* settings, bool connected) :
    AbstractEyeDataForPlot(settings),
    m_connected(connected)
{}


void RadialScatterplot::updatePlot(const std::vector<Trial*>& trials,
                                   const std::vector<std::vector<Fixation*> >& fixationsForTrials)
{
    AbstractEyeDataForPlot::updatePlot(trials, fixationsForTrials);

    show();
    update();
}


void RadialScatterplot::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    QColor baseColor = *(m_settings->m_colors.dataPlotBaseColor);

    painter.setBrush(Qt::transparent);

    // outer circle
    drawOuterCircle(painter, baseColor);
    drawValues(painter, baseColor);

    QColor plotColor = *(m_settings->m_colors.dataPlotColor);
    QColor plotColorTransparent = *(m_settings->m_colors.dataPlotColor);
    plotColorTransparent.setAlpha(m_settings->m_colors.alphaForPlots);

    painter.setPen(plotColor);
    painter.setBrush(plotColorTransparent);

    // Plots
    qreal max = 0;

    drawAxis(0, m_plotRadius, painter, baseColor);

    std::vector<std::vector<std::vector<LengthElement> > > lengthElements = getDirectionalLengthData();

    QTransform t = painter.transform();

    if (lengthElements.empty())
    {
        return;
    }

    //draw lines to connect points

    if (m_settings->m_dataPlotRadiusCount <= 0)
    {
        for (size_t i = 0; i < lengthElements.size(); ++i)
        {
            for (size_t j = 0; j < lengthElements[i].size(); ++j)
            {
                for (size_t k = 0; k < lengthElements[i][j].size(); ++k)
                {
                    max = std::max({max, lengthElements[i][j][k].length});
                }
            }
        }
    }
    else
    {
        max = m_settings->m_dataPlotRadiusCount;
    }

    for (int o = 0; o < 2; ++o)
    {
        if (m_connected)
        {
            for(size_t i = 0; i < lengthElements.size(); ++i)
            {
                for(size_t j = 0; j < lengthElements[i].size(); ++j)
                {
                    for(size_t k = 0; k < lengthElements[i][j].size() - 1; ++k)
                    {
                        if (o == 1 && lengthElements[i][j][k].fromMicrosaccade &&
                            lengthElements[i][j][k + 1].fromMicrosaccade)
                        {
                            painter.setOpacity(0.4);
                            painter.setPen(QPen(*(m_settings->m_colors.microsaccadesColor), 0));
                        }
                        else if (o == 0 && (m_settings->m_dataPlotData == PD_GazeRawData))
                        {
                            painter.setOpacity(0.4);
                            painter.setPen(QPen(plotColor, 0));

                            changePainterForTestCondition(painter, i);
                        }
                        else
                        {
                            continue;
                        }

                        painter.setTransform(t);
                        //painter.rotate(m_dataPoints2[i].x());

                        qreal p1_y = lengthElements[i][j][k].length * m_plotRadius / max;
                        qreal p2_y = lengthElements[i][j][k + 1].length * m_plotRadius / max;

                        painter.drawLine(rotatePoint(QPointF(0, 0), lengthElements[i][j][k].angle, QPointF(0, p1_y)),
                                         rotatePoint(QPointF(0, 0), lengthElements[i][j][k + 1].angle, QPointF(0, p2_y)));
                    }
                }
            }
        }

        for(size_t i = 0; i < lengthElements.size(); ++i)
        {
            for(size_t j = 0; j < lengthElements[i].size(); ++j)
            {
                for(size_t k = 0; k < lengthElements[i][j].size(); ++k)
                {
                    painter.setOpacity(0.4);
                    painter.setPen(QPen(plotColor, 2));

                    if (o == 1 && lengthElements[i][j][k].fromMicrosaccade)
                    {
                        painter.setOpacity(0.4);
                        painter.setPen(QPen(*(m_settings->m_colors.microsaccadesColor), 2));
                    }
                    else if ((o == 0 && m_settings->m_dataPlotData == PD_GazeRawData) ||
                             m_settings->m_dataPlotData == PD_Saccades)
                    {
                        painter.setOpacity(0.4);
                        painter.setPen(QPen(plotColor, 2));

                        changePainterForTestCondition(painter, i);
                    }
                    else
                    {
                        continue;
                    }

                    painter.setTransform(t);
                    painter.rotate(lengthElements[i][j][k].angle);

                    painter.drawPoint(QPointF(0, -lengthElements[i][j][k].length * m_plotRadius / max));
                }
            }
        }
    }

    painter.setTransform(t);

    drawMinMax(painter, baseColor, max, true);
}


std::vector<std::vector<std::vector<RadialScatterplot::LengthElement> > > RadialScatterplot::getDirectionalLengthData()
{
    std::vector<std::vector<std::vector<LengthElement> > > outValues;

    if (m_trials.size() != m_fixationsForTrials.size())
    {
        return outValues;
    }

    for (size_t i = 0; i < m_fixationsForTrials.size(); ++i) // for each trial
    {
        // list of gaze indices which belong to microsaccades
        std::set<int> microsaccadeIndices = m_trials[i]->getMicrosaccadeIndices();

        std::vector<std::vector<LengthElement > > fixationData;

        size_t fixCount = 0;
        foreach (Fixation* fixation, m_fixationsForTrials[i]) // for each fixation of trial
        {
            if (fixation == nullptr)
            {
                continue;
            }

            Fixation* nextFixation = m_trials[i]->getNextFixation(fixation);
            Fixation* prevFixation = m_trials[i]->getPrevFixation(fixation);

            qreal angleInDegreesToNextFixations = 0;
            qreal angleInDegreesToPrevFixations = 0;
            if (nextFixation != nullptr)
            {
                angleInDegreesToNextFixations = MathHelpFunctions::getAngle(
                                                         fixation->getXPosition(),
                                                         nextFixation->getXPosition(),
                                                         fixation->getYPosition(),
                                                         nextFixation->getYPosition());
            }
            if (prevFixation != nullptr)
            {
                angleInDegreesToPrevFixations = MathHelpFunctions::getAngle(
                                                         fixation->getXPosition(),
                                                         prevFixation->getXPosition(),
                                                         fixation->getYPosition(),
                                                         prevFixation->getYPosition());
            }

            std::vector<LengthElement > dataValues;

            const QMap<int, GazeData>& g = m_trials[i]->gazeData();

            if (m_settings->m_dataPlotData == PD_Microsaccades)
            {
                if (fixation->getMicrosaccadesCount(m_trials[i]->msFromInputFile()) > 0)
                {
                    for (size_t j = 0; j < fixation->getMicrosaccadesCount(m_trials[i]->msFromInputFile()); ++j)
                    {
                        Saccade* ms = fixation->getMicrosaccadeAt(j, m_trials[i]->msFromInputFile());

                        qreal microsaccadeAngle = MathHelpFunctions::getAngle(
                                g[int(ms->getOnsetIndex())].getXPosition(),
                                g[int(ms->getEndIndex())].getXPosition(),
                                g[int(ms->getOnsetIndex())].getYPosition(),
                                g[int(ms->getEndIndex())].getYPosition());

                        qreal length = std::sqrt(pow(g[int(ms->getEndIndex())].getXPosition() - g[int(ms->getOnsetIndex())].getXPosition(), 2) +
                                pow(g[int(ms->getEndIndex())].getYPosition() - g[int(ms->getOnsetIndex())].getYPosition(), 2));

                        LengthElement el;
                        el.length = length / m_trials[i]->getParticipant()->getPixelsPerDegree();
                        el.fromMicrosaccade = true;

                        if (m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                        {
                            el.angle = fmod(360 - angleInDegreesToNextFixations + microsaccadeAngle, 360);
                        }
                        else if (m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                        {
                            el.angle = fmod(360 - angleInDegreesToPrevFixations + microsaccadeAngle, 360);
                        }
                        else
                        {
                            el.angle = fmod(microsaccadeAngle, 360);
                        }
                        dataValues.push_back(el);
                    }
                }
                if (!dataValues.empty())
                {
                    fixationData.push_back(dataValues);
                }
            }
            else if (m_settings->m_dataPlotData == PD_Saccades)
            {
                qreal angle = angleInDegreesToNextFixations;

                if (nextFixation != nullptr && fixCount < m_fixationsForTrials[i].size() - 1)
                {
                    qreal length = std::sqrt(pow(nextFixation->getXPosition() - fixation->getXPosition(), 2) +
                            pow(nextFixation->getYPosition() - fixation->getYPosition(), 2));

                    LengthElement el;
                    el.length = length / m_trials[i]->getParticipant()->getPixelsPerDegree();

                    el.fromMicrosaccade = false;

                    if (m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                    {
                        el.angle = fmod(360 - angleInDegreesToNextFixations + angle, 360);
                    }
                    else if (m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                    {
                        Fixation* prevFixation = m_trials[i]->getPrevFixation(fixation);
                        if (prevFixation != nullptr)
                        {
                            qreal angle2 = MathHelpFunctions::getAngle(
                                                    fixation->getXPosition(),
                                                    prevFixation->getXPosition(),
                                                    fixation->getYPosition(),
                                                    prevFixation->getYPosition());

                            el.angle = fmod(360 - angle2 + angle, 360);
                        }
                    }
                    else
                    {
                        el.angle = fmod(angle, 360);
                    }
                    dataValues.push_back(el);
                }

                if (!dataValues.empty())
                {
                    fixationData.push_back(dataValues);
                }
            }
            else if (m_settings->m_dataPlotData == PD_GazeRawData)
            {
                for (int j = fixation->getStartIndex(); j <= fixation->getEndIndex() - 1; ++j) // additional -1 as there is no next point within this fixation for the last data point
                {
                    if (g.contains(j))
                    {
                        qreal gazePointAngle = MathHelpFunctions::getAngle(
                                                        g[j].getXPosition(),
                                                        g[j + 1].getXPosition(),
                                                        g[j].getYPosition(),
                                                        g[j + 1].getYPosition());

                        qreal length = std::sqrt(pow(g[j + 1].getXPosition() - g[j].getXPosition(), 2) +
                                pow(g[j + 1].getYPosition() - g[j].getYPosition(), 2));

                        LengthElement el;
                        el.length = length / m_trials[i]->getParticipant()->getPixelsPerDegree();
                        el.fromMicrosaccade = microsaccadeIndices.count(j);
                        if (m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                        {
                            el.angle = fmod(360 - angleInDegreesToNextFixations +
                                        gazePointAngle, 360);
                        }
                        else if (m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                        {
                            el.angle = fmod(360 - angleInDegreesToPrevFixations +
                                        gazePointAngle, 360);
                        }
                        else
                        {
                            el.angle = fmod(gazePointAngle, 360);

                        }
                        dataValues.push_back(el);
                    }
                    else // in case data is missing save as separate set of gaze data, otherwise they would be connected in connected scatterplot
                    {
                        if (!dataValues.empty())
                        {
                            fixationData.push_back(dataValues);
                            dataValues = std::vector<LengthElement>();
                        }
                    }
                }
                if (!dataValues.empty())
                {
                    fixationData.push_back(dataValues);
                }
            }
            fixCount++;
        }

        outValues.push_back(fixationData);
    }

    return outValues;
}

