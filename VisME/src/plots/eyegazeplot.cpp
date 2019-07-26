/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyegazeplot.cpp
 **************************************************************************************************/
#include "enums.h"
#include "eyegazeplot.h"
#include "mathhelpfunctions.h"


EyeGazePlot::EyeGazePlot(Settings* settings) : AbstractEyeDataForCartesianPlot(settings)
{
    m_horizontallyCentered = true;
    m_verticallyCentered = true;
    m_mainPlotColor = *(m_settings->m_colors.dataPlotColor);
}


void EyeGazePlot::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    qreal minSpace = width() <= height() ? width() : height();

    m_spacingV = int(minSpace / 300.0 * 15.0);
    m_spacingH = int(minSpace / 300.0 * 20.0);
    auto plotRadius = int(m_maxSize / 2.0 - m_spacingH);
    auto padding = int(minSpace / 300.0 * 6.0);
    auto padding2 = int(minSpace / 300.0 * 20.0);

    QColor baseColor = *(m_settings->m_colors.dataPlotBaseColor);

    painter.setPen(baseColor);
    painter.setBrush(Qt::transparent);

    // x, y axis
    drawCenteredAxis(0, plotRadius, painter, baseColor, true);

    QString textTop = m_settings->m_dataPlotDirection == PDir_ToNextFixation ? "Next Fixation" :
                      m_settings->m_dataPlotDirection == PDir_ToPreviousFixation ? "Previous Fixation" : "y";
    QString textRight = m_settings->m_dataPlotDirection == PDir_ToNextFixation ||
                        m_settings->m_dataPlotDirection == PDir_ToPreviousFixation ? "" : "x";

    painter.setPen(baseColor);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_spacingV - 2);
    painter.setFont(font);

    painter.drawText(plotRadius + padding, - m_spacingV/2,
                     m_spacingH, m_spacingV,
                     Qt::AlignVCenter | Qt::AlignLeft, textRight);
    painter.drawText(-plotRadius/2, -plotRadius - m_spacingV - padding,
                     plotRadius, m_spacingV,
                     Qt::AlignBottom | Qt::AlignHCenter, textTop);

    QColor plotColor = *(m_settings->m_colors.dataPlotColor);

    painter.setPen(plotColor);

    // Plots
    qreal max = 0;

    if (m_fixationsForTrials.empty())
    {
        return;
    }

    if (m_settings->m_dataPlotRadiusDegree <= 0)
    {
        max = determineMaxValue(m_fixationsForTrials);
    }
    else
    {
        max = m_settings->m_dataPlotRadiusDegree;
    }

    qreal maxForPlot = max / plotRadius;

    painter.setPen(QPen(plotColor, 0));

    if (m_settings->m_dataPlotData == PD_Microsaccades)
    {
        for (size_t i = 0; i < m_trials.size(); ++i)
        {
            const QMap<int, GazeData>& g = m_trials[i]->gazeData();
            qreal pixelsPerDegree = m_trials[i]->getParticipant()->getPixelsPerDegree();

            for (size_t j = 0; j < m_fixationsForTrials[i].size(); ++j)
            {
                Fixation* fixation = m_fixationsForTrials[i][j];
                Fixation* nextFixation = m_trials[i]->getNextFixation(fixation);
                Fixation* prevFixation = m_trials[i]->getPrevFixation(fixation);
                qreal angle = 0;
                if (nextFixation != nullptr && m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                {
                    angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                        nextFixation->getXPosition(),
                                                        fixation->getYPosition(),
                                                        nextFixation->getYPosition());
                }
                else if (prevFixation != nullptr && m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                {
                    angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                        prevFixation->getXPosition(),
                                                        fixation->getYPosition(),
                                                        prevFixation->getYPosition());
                }

                QTransform t = painter.transform();

                painter.rotate(-angle);

                for (size_t k = 0; k < fixation->
                     getMicrosaccadesCount(m_trials[i]->msFromInputFile()); ++k)
                {
                    Saccade* ms = fixation->getMicrosaccadeAt(k, m_trials[i]->msFromInputFile());

                    qreal x1 = (g[int(ms->getOnsetIndex())].getXPosition() -
                                fixation->getXPosition()) / pixelsPerDegree;
                    qreal x2 = (g[int(ms->getEndIndex())].getXPosition() -
                                fixation->getXPosition()) / pixelsPerDegree;
                    qreal y1 = (g[int(ms->getOnsetIndex())].getYPosition() -
                                fixation->getYPosition()) / pixelsPerDegree;
                    qreal y2 = (g[int(ms->getEndIndex())].getYPosition() -
                                fixation->getYPosition()) / pixelsPerDegree;

                    QLinearGradient linearGrad(QPointF(x1, y1) / maxForPlot,
                                               QPointF(x2,y2) / maxForPlot);
                    linearGrad.setColorAt(0, *(m_settings->m_colors.microsaccadesColor));
                    linearGrad.setColorAt(1, *(m_settings->m_colors.microsaccadesDirectionEndColor));
                    painter.setPen(QPen(linearGrad, 1.0));

                    painter.drawLine(QPointF(x1, y1) / maxForPlot,
                                     QPointF(x2,y2) / maxForPlot);
                }

                painter.setTransform(t);
            }
        }
    }
    else if (m_settings->m_dataPlotData == PD_Saccades)
    {
        for (size_t i = 0; i < m_trials.size(); ++i)
        {
            if (m_fixationsForTrials[i].size() <= 1)
            {
                continue;
            }

            qreal pixelsPerDegree = m_trials[i]->getParticipant()->getPixelsPerDegree();

            for (size_t j = 0; j < m_fixationsForTrials[i].size() - 1; ++j)
            {
                Fixation* fixation = m_fixationsForTrials[i][j];
                Fixation* nextFixation = m_trials[i]->getNextFixation(fixation);
                qreal angle = 0;

                if (m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                {
                    angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                        nextFixation->getXPosition(),
                                                        fixation->getYPosition(),
                                                        nextFixation->getYPosition());
                }
                else if (m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                {
                    Fixation* prevFixation = m_trials[i]->getPrevFixation(fixation);
                    if (prevFixation != nullptr)
                    {
                        angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                             prevFixation->getXPosition(),
                                                             fixation->getYPosition(),
                                                             prevFixation->getYPosition());
                    }
                }

                QTransform t = painter.transform();

                painter.rotate(-angle);

                qreal x1 = 0;
                qreal x2 = (nextFixation->getXPosition() -
                           fixation->getXPosition()) / pixelsPerDegree;
                qreal y1 = 0;
                qreal y2 = (nextFixation->getYPosition() -
                           fixation->getYPosition()) / pixelsPerDegree;

                QLinearGradient linearGrad((QPointF(x2,y2) / 2) / maxForPlot,
                                           QPointF(x2,y2) / maxForPlot);
                linearGrad.setColorAt(0, *(m_settings->m_colors.saccadesDirectionStartColor));
                linearGrad.setColorAt(1, *(m_settings->m_colors.saccadeColor));
                painter.setPen(QPen(linearGrad, 1.0));

                painter.drawLine(QPointF(x1, y1) / maxForPlot,
                                 QPointF(x2,y2) / maxForPlot);

                painter.setTransform(t);
            }
        }
    }
    else if (m_settings->m_dataPlotData == PD_GazeRawData)
    {
        for (int o = 0; o < 2; ++o)
        {
            for (size_t i = 0; i < m_trials.size(); ++i)
            {
                qreal pixelsPerDegree = m_trials[i]->getParticipant()->getPixelsPerDegree();

                const QMap<int, GazeData>& g = m_trials[i]->gazeData();
                std::set<int> microsaccadeIndices = m_trials[i]->getMicrosaccadeIndices();

                for (size_t j = 0; j < m_fixationsForTrials[i].size(); ++j)
                {
                    Fixation* fixation = m_fixationsForTrials[i][j];
                    Fixation* nextFixation = m_trials[i]->getNextFixation(fixation);
                    Fixation* prevFixation = m_trials[i]->getPrevFixation(fixation);
                    qreal angle = 0;
                    if (nextFixation != nullptr && m_settings->m_dataPlotDirection == PDir_ToNextFixation)
                    {
                        angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                            nextFixation->getXPosition(),
                                                            fixation->getYPosition(),
                                                            nextFixation->getYPosition());
                    }
                    else if (prevFixation != nullptr && m_settings->m_dataPlotDirection == PDir_ToPreviousFixation)
                    {
                        angle = MathHelpFunctions::getAngle(fixation->getXPosition(),
                                                            prevFixation->getXPosition(),
                                                            fixation->getYPosition(),
                                                            prevFixation->getYPosition());
                    }

                    QTransform t2 = painter.transform();

                    painter.rotate(-angle);

                    for (int k = fixation->getStartIndex(); k <= fixation->getEndIndex() - 1; ++k)// additional -1 as there is no next point within this fixation for the last data point
                    {
                        qreal x1 = (g[k].getXPosition() -
                                    fixation->getXPosition()) / pixelsPerDegree;
                        qreal x2 = (g[k + 1].getXPosition() -
                                    fixation->getXPosition()) / pixelsPerDegree;
                        qreal y1 = (g[k].getYPosition() -
                                    fixation->getYPosition()) / pixelsPerDegree;
                        qreal y2 = (g[k + 1].getYPosition() -
                                    fixation->getYPosition()) / pixelsPerDegree;

                        if (o == 1 && microsaccadeIndices.count(k) > 0 && microsaccadeIndices.count(k + 1) > 0)
                        {
                            painter.setOpacity(0.4);
                            painter.setPen(*(m_settings->m_colors.microsaccadesColor));
                        }
                        else if (o == 0)
                        {
                            if (m_settings->m_dataPlotData == PD_GazeRawData)
                            {
                                painter.setOpacity(0.4);
                            }
                            painter.setPen(*(m_settings->m_colors.dataPlotColor));
                        }
                        else
                        {
                            continue;
                        }

                        painter.drawLine(QPointF(x1, y1) / maxForPlot,
                                         QPointF(x2,y2) / maxForPlot);
                    }

                    painter.setTransform(t2);
                }
            }
        }
    }

    drawMarker(painter, baseColor, max, plotRadius + padding2);
}


void EyeGazePlot::drawMarker(QPainter& painter, const QColor color, qreal max,
                             int size)
{
    painter.save();

    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setOpacity(1);
    painter.setPen(color);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_spacingV - 2);
    painter.setFont(font);

    painter.drawText(0, 0, size, size, Qt::AlignBottom | Qt::AlignRight,
                     QString(tr("marker: %1°")).arg(max));

    painter.restore();
}


void EyeGazePlot::updatePlot(const std::vector<Trial*>& trials,
                             const std::vector<std::vector<Fixation*> >& fixationsForTrials)
{
    AbstractEyeDataForCartesianPlot::updatePlot(trials, fixationsForTrials);

    show();
    update();
}
