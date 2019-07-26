/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyescatterplot.cpp
 **************************************************************************************************/
#include "eyescatterplot.h"
#include "mathhelpfunctions.h"


EyeScatterplot::EyeScatterplot(Settings* settings) : CartesianScatterplot(settings)
{
    m_mainPlotColor = *(m_settings->m_colors.dataPlotColor);
}


void EyeScatterplot::updatePlot(const std::vector<Trial*>& trials,
                                const std::vector<std::vector<Fixation*> >& fixationsForTrials,
                                const std::vector<std::vector<Saccade*> >& saccadesForTrials)
{
    AbstractEyeDataForCartesianPlot::updatePlot(trials, fixationsForTrials);
    m_saccadesForTrials = saccadesForTrials;

    show();
    update();
}


void EyeScatterplot::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    if (m_fixationsForTrials.empty())
    {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    setPlotSize();

    QColor baseColor = *(m_settings->m_colors.dataPlotBaseColor);

    // center plot
    painter.translate((width() - m_plotSize) / 2, (height() - m_plotSize) / 2);

    painter.setPen(baseColor);
    painter.setBrush(Qt::transparent);

    painter.setPen(baseColor);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(int(m_spacingV - 2));
    painter.setFont(font);

    m_xValues.clear();
    m_yValues.clear();
    m_colorValues.clear();

    // determine max time of fixations and start positions of microsaccades

    if (m_settings->m_histogramAndScatterplotData == PD_Microsaccades)
    {
        for (size_t i = 0; i < m_fixationsForTrials.size(); ++i)
        {
            for (size_t j = 0; j < m_fixationsForTrials[i].size(); ++j)
            {
                size_t msCount = m_fixationsForTrials[i][j]->getMicrosaccadesCount(m_trials[i]->msFromInputFile());
                for (size_t k = 0; k < msCount; ++k)
                {
                    Saccade* ms = m_fixationsForTrials[i][j]->getMicrosaccadeAt(k, m_trials[i]->msFromInputFile());
                    m_xValues.push_back(getValue(ms, m_trials[i], m_settings->m_scatterplotXAxis));
                    m_yValues.push_back(getValue(ms, m_trials[i], m_settings->m_scatterplotYAxis));
                    m_colorValues.push_back(getColor(m_trials[i]->getTestConditionType()));
                }
            }
        }
    }
    else if (m_settings->m_histogramAndScatterplotData == PD_Saccades)
    {
        // Right now, saccades can only be used when determined internally... (TODO)
        for (size_t i = 0; i < m_trials.size(); ++i)
        {
            if (m_trials[i]->fixationsFromInputFile())
            {
                drawMessage(m_plotSize, painter,
                            "Saccades have to be determined with this application for this option");
                return;
            }

            for (size_t j = 0; j < m_saccadesForTrials[i].size(); ++j)
            {
                Saccade* saccade = m_saccadesForTrials[i][j];
                if (saccade->isValid()) // ignore invalid saccades (which might be influenced by missing data)
                {
                    m_xValues.push_back(getValue(saccade, m_trials[i], m_settings->m_scatterplotXAxis));
                    m_yValues.push_back(getValue(saccade, m_trials[i], m_settings->m_scatterplotYAxis));
                    m_colorValues.push_back(getColor(m_trials[i]->getTestConditionType()));
                }
            }
        }
    }

    m_maxValX = 0;
    m_maxValY = 0;
    if (!m_xValues.empty() && !m_yValues.empty())
    {
        m_maxValX = *std::max_element(m_xValues.begin(), m_xValues.end());
        m_maxValY = *std::max_element(m_yValues.begin(), m_yValues.end());

        m_minValX = *std::min_element(m_xValues.begin(), m_xValues.end());
        m_minValY = *std::min_element(m_yValues.begin(), m_yValues.end());
    }

    m_xScale = 1;
    m_yScale = 1;
    if (m_settings->m_scatterplotMaxX > 0)
    {
        if (m_settings->m_logarithmicScale)
        {
            m_xScale = (log10(m_maxValX) - log10(m_minValX)) / qreal(m_settings->m_scatterplotMaxX);
        }
        else
        {
            m_xScale = m_maxValX / qreal(m_settings->m_scatterplotMaxX);
        }
    }

    if (m_settings->m_scatterplotMaxY > 0)
    {
        if (m_settings->m_logarithmicScale)
        {
            m_yScale = (log10(m_maxValY) - log10(m_minValY)) / qreal(m_settings->m_scatterplotMaxY);
        }
        else
        {
            m_yScale = m_maxValY / qreal(m_settings->m_scatterplotMaxY);
        }
    }

    // x, y axis
    QString DataDescription = m_settings->m_histogramAndScatterplotData == PD_Microsaccades ?
                                  "Microsaccade" : "Saccade";
    QString xDescription;
    QString yDescription;

    xDescription = getDescription(m_settings->m_scatterplotXAxis);
    yDescription = getDescription(m_settings->m_scatterplotYAxis);

    drawAxis(m_plotSize, 0, tr("%1 %2").arg(DataDescription, yDescription),
             tr("%1 %2").arg(DataDescription, xDescription), 0,
             m_maxValX / m_xScale, m_maxValY / m_yScale, painter, baseColor, true, false);

    CartesianScatterplot::draw(painter);

    painter.setOpacity(1);
    painter.setPen(baseColor);
}
