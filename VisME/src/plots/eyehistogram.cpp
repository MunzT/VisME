/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyehistogram.cpp
 **************************************************************************************************/
#include "eyehistogram.h"
#include "mathhelpfunctions.h"


EyeHistogram::EyeHistogram(Settings* settings) : CartesianHistogram(settings)
{
    m_mainPlotColor = *(m_settings->m_colors.dataPlotColor);
}


void EyeHistogram::updatePlot(const std::vector<Trial*>& trials,
                              const std::vector<std::vector<Fixation*> >& fixationsForTrials,
                              const std::vector<std::vector<Saccade*> >& saccadesForTrials)
{
    CartesianHistogram::updatePlot(trials, fixationsForTrials);

    m_saccadesForTrials = saccadesForTrials;

    show();
    update();
}


void EyeHistogram::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    if (m_fixationsForTrials.empty())
    {
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    qreal minSpace = width() <= height() ? width() : height();

    auto spacingV = int(minSpace / 300.0 * 13.0);
    auto spacingH = int(minSpace / 300.0 * 80.0);
    m_plotSize = m_maxSize - spacingH;

    QColor baseColor = *(m_settings->m_colors.dataPlotBaseColor);

    // center plot
    painter.translate((width() - m_plotSize) / 2, (height() - m_plotSize) / 2);

    painter.setPen(baseColor);
    painter.setBrush(Qt::transparent);

    painter.setPen(baseColor);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(spacingV - 2);
    painter.setFont(font);

    // determine max time of fixations and start positions of microsaccades
    std::vector<qreal> values;
    std::vector<int> testConditions;
    m_maxXValue = 0;

    QMap<QString, int> testConditionsToId;
    int count = 0;
    QMapIterator<QString, QColor> it(m_settings->m_testConditions);
    while (it.hasNext())
    {
        it.next();
        testConditionsToId.insert(it.key(), count);
        m_idToColor.insert(count, getColor(it.key()));
        count++;
    }

    if (m_settings->m_histogramAndScatterplotData == PD_Microsaccades)
    {
        for (size_t i = 0; i < m_fixationsForTrials.size(); ++i)
        {
            for (size_t j = 0; j < m_fixationsForTrials[i].size(); ++j)
            {
                for (size_t k = 0; k < m_fixationsForTrials[i][j]->
                     getMicrosaccadesCount(m_trials[i]->msFromInputFile()); ++k)
                {
                    Saccade* ms = m_fixationsForTrials[i][j]->
                                  getMicrosaccadeAt(k, m_trials[i]->msFromInputFile());
                    values.push_back(getValue(ms, m_trials[i], m_settings->m_histogramYAxis,
                                              m_fixationsForTrials[i][j]->getStartIndex()));
                    testConditions.push_back(testConditionsToId[m_trials[i]->getTestConditionType()]);
                }
            }
        }
    }
    else if (m_settings->m_histogramAndScatterplotData == PD_Saccades)
    {
        // Saccades can only be used when determined internally... (TODO)
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
                if (m_saccadesForTrials[i][j]->isValid()) // ignore invalid saccades (which might be influenced by missing data)
                {
                    values.push_back(getValue(m_saccadesForTrials[i][j], m_trials[i], m_settings->m_histogramYAxis));
                    testConditions.push_back(testConditionsToId[m_trials[i]->getTestConditionType()]);
                }
            }
        }
    }

    if (!values.empty())
    {
        m_maxXValue = *std::max_element(values.begin(), values.end());
    }

    // Size of bins
    qreal binSize = 0;

    if (m_settings->m_histogramYAxis == MSStart)
    {
        binSize = m_settings->m_histogramStartTimeBinSize;
    }
    else if (m_settings->m_histogramYAxis == PeakVelocity)
    {
        binSize = m_settings->m_histogramPeakVelocityBinSize;
    }
    else if (m_settings->m_histogramYAxis == Amplitude)
    {
        binSize = m_settings->m_histogramAmplitudeBinSize;
    }
    else if (m_settings->m_histogramYAxis == Duration)
    {
        binSize = m_settings->m_histogramDurationBinSize;
    }

    std::vector<std::vector<qreal> > testConditionBins(std::max(1, m_settings->m_testConditions.size()));

    for (size_t i = 0; i < testConditionBins.size(); ++i)
    {
        std::vector<qreal> bins(m_maxXValue / binSize + 1); // round up
        testConditionBins[i] = bins;
    }


    // fill bins with start positions of (micro)saccades
    for (size_t i = 0; i < values.size(); ++i)
    {
        testConditionBins[testConditions[i]][values[i] / binSize]++;
    }

    m_maxYValue = 0;
    std::vector<qreal> sums(testConditionBins[0].size());
    for (size_t i = 0; i < testConditionBins.size(); ++i)
    {
        for (size_t j = 0; j < testConditionBins[i].size(); ++j)
        {
            sums[j] += testConditionBins[i][j];
        }
    }

    m_maxYValue = *std::max_element(sums.begin(), sums.end());

    m_data = testConditionBins;

    m_xScale = 1;
    m_yScale = 1;
    if (m_settings->m_histogramMaxX != 0.0)
    {
        m_xScale = m_maxXValue / m_settings->m_histogramMaxX;
    }

    if (m_settings->m_histogramMaxY != 0.0)
    {
        m_yScale = m_maxYValue / m_settings->m_histogramMaxY;
    }

    // x, y axis
    QString DataDescription = m_settings->m_histogramAndScatterplotData == PD_Microsaccades ?
                                  "Microsaccade" : "Saccade";
    QString xDescription = getDescription(m_settings->m_histogramYAxis);

    drawAxis(m_plotSize, m_plotSize / qreal(m_data[0].size()) * m_xScale, tr("#%1s").arg(DataDescription),
             tr("%1 %2").arg(DataDescription, xDescription), binSize,
             m_maxXValue / m_xScale, m_maxYValue / m_yScale, painter, baseColor, true, true);

    CartesianHistogram::draw(painter);

    painter.setOpacity(1);
    painter.setPen(baseColor);
}
