/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedirectionhistogram.cpp
 **************************************************************************************************/
#include "eyedirectionhistogram.h"
#include "mathhelpfunctions.h"


EyeDirectionHistogram::EyeDirectionHistogram(Settings* settings) : CartesianHistogram(settings)
{
    m_horizontallyCentered = true;
    m_mainPlotColor = *(m_settings->m_colors.dataPlotColor);
}


void EyeDirectionHistogram::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    if (m_fixationsForTrials.empty())
    {
        return;
    }

    reorderTrialsByTestCondition();

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    qreal minSpace = width() <= height() ? width() : height();

    auto spacingV = int(minSpace / 300.0 * 13.0);
    auto spacingH = int(minSpace / 300.0 * 80.0);
    m_plotSize = int(m_maxSize - spacingH);

    // center plot
    painter.translate((width() - m_plotSize) / 2, (height() - m_plotSize) / 2);

    painter.setPen(*(m_settings->m_colors.dataPlotBaseColor));
    painter.setBrush(Qt::transparent);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(spacingV - 2);
    painter.setFont(font);

    QColor baseColor = *(m_settings->m_colors.dataPlotBaseColor);

    painter.setBrush(Qt::transparent);

    QColor plotColor = *(m_settings->m_colors.dataPlotColor);
    QColor plotColorTransparent = *(m_settings->m_colors.dataPlotColor);
    plotColorTransparent.setAlpha(m_settings->m_colors.alphaForPlots);

    painter.setPen(plotColor);
    painter.setBrush(plotColorTransparent);

    // plots
    qreal max = 0;

    reorderTrialsByTestCondition();

    std::vector<QString> testConditionsForAngles;
    std::vector<std::vector<qreal> > angleValues = getAngleData(m_settings->m_overplot, testConditionsForAngles);

    for (size_t i = 0; i < angleValues.size(); ++i)
    {
        for (size_t j = 0; j < angleValues[i].size(); ++j)
        {
            angleValues[i][j] = fmod(angleValues[i][j] + 180, 360);
        }
        if (m_settings->m_overplot == PA_NoAggregation && m_settings->m_testConditionColors)
        {
            m_idToColor.insert(int(i), getColor(m_trials[i]->getTestConditionType()));
        }
        else if (m_settings->m_overplot == PA_ForTestConditions && m_settings->m_testConditionColors)
        {
            m_idToColor.insert(int(i), getColor(testConditionsForAngles[i]));
        }
        else
        {
            m_idToColor.insert(int(i), m_mainPlotColor);
        }
    }

    std::vector<std::vector<qreal> > orderedValues = dataToBins(angleValues, m_settings->m_dataPlotBins);

    if (orderedValues.empty())
    {
        return;
    }

    // draw histogram
    if (m_settings->m_dataPlotRadiusCount <= 0)
    {
        if (!angleValues.empty())
        {
            for (size_t j = 0; j < orderedValues[0].size(); ++j)
            {
                int tempMax = 0;
                for (size_t i = 0; i < orderedValues.size(); ++i)
                {
                    tempMax += orderedValues[i][j];

                }
                max = std::fmax(max, tempMax);
            }
        }
    }
    else
    {
        max = m_settings->m_dataPlotRadiusCount;
    }

    if (max <= 0) // no valid values available
    {
        return;
    }

    m_data = orderedValues;

    m_maxXValue = m_data.size();
    m_maxYValue = max;

    m_xScale = 1;
    m_yScale = 1;
    if (m_settings->m_dataPlotRadiusDegree != 0.0)
    {
        m_xScale = m_maxXValue / m_settings->m_histogramMaxX;
    }

    if (m_settings->m_dataPlotRadiusDegree != 0.0)
    {
        m_yScale = m_maxYValue / m_settings->m_histogramMaxY;
    }

    // x, y axis
    QString dataDescription = m_settings->m_dataPlotData == PD_Microsaccades ? "Microsaccade" :
                              m_settings->m_dataPlotData == PD_Saccades ? "Saccade" : "Gaze Point";
    drawHorizontallyCenteredAxis(painter, m_plotSize, baseColor, dataDescription, m_maxYValue / m_yScale);

    m_diagramShift = int(-m_plotSize / int(orderedValues[0].size()) / 2.0);

    CartesianHistogram::draw(painter);
}
