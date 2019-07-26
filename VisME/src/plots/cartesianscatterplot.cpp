/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianscatterplot.cpp
 **************************************************************************************************/
#include "cartesianscatterplot.h"
#include "mathhelpfunctions.h"


CartesianScatterplot::CartesianScatterplot(Settings* settings) :
    AbstractEyeDataForCartesianPlot(settings)
{}


void CartesianScatterplot::draw(QPainter& painter)
{
    // draw scatterplot
    for (size_t i = 0; i < m_xValues.size(); ++i)
    {
        QColor testConditionColor = m_colorValues[i];
        QColor testConditionColorTransparent = m_colorValues[i];
        if (!testConditionColor.isValid())
        {
            testConditionColor = m_mainPlotColor;
            testConditionColorTransparent = m_mainPlotColor;
        }
        testConditionColorTransparent.setAlpha(m_settings->m_colors.alphaForPlots);

        painter.setPen(QPen(testConditionColor, 3));
        painter.setBrush(testConditionColorTransparent);

        if (m_settings->m_logarithmicScale)
        {
            painter.drawPoint(QPointF(getXValueInVis((log10(m_xValues[i]) - log10(m_minValX)) / (log10(m_maxValX) - log10(m_minValX))),
                                      getYValueInVis((log10(m_yValues[i]) - log10(m_minValY)) / (log10(m_maxValY) - log10(m_minValY)))));
        }
        else
        {
            painter.drawPoint(QPointF(getXValueInVis(m_xValues[i] / m_maxValX), getYValueInVis(m_yValues[i] / m_maxValY)));
        }
    }

    if (!m_settings->m_logarithmicScale)
    {
        if (!m_xValues.empty())
        {
            painter.setPen(*(m_settings->m_colors.dataPlotBaseColor));

            qreal xMean = MathHelpFunctions::mean(m_xValues);
            qreal yMean = MathHelpFunctions::mean(m_yValues);

            painter.drawLine(QPointF(getXValueInVis(xMean / m_maxValX), 0),
                             QPointF(getXValueInVis(xMean / m_maxValX), m_plotSize));
            painter.drawLine(QPointF(0, getYValueInVis(yMean / m_maxValY)),
                             QPointF(m_plotSize, getYValueInVis(yMean / m_maxValY)));
        }
    }
    else
    {
        painter.setPen(*(m_settings->m_colors.dataPlotBaseColor));

        qreal xMean = MathHelpFunctions::mean(m_xValues);
        qreal yMean = MathHelpFunctions::mean(m_yValues);

        painter.drawLine(QPointF(getXValueInVis(((log10(xMean) - log10(m_minValX)) / (log10(m_maxValX) - log10(m_minValX)))), 0),
                         QPointF(getXValueInVis((log10(xMean) - log10(m_minValX)) / (log10(m_maxValX) - log10(m_minValX))), m_plotSize));
        painter.drawLine(QPointF(0, getYValueInVis((log10(yMean) - log10(m_minValY)) / (log10(m_maxValY) - log10(m_minValY)))),
                         QPointF(m_plotSize, getYValueInVis((log10(yMean) - log10(m_minValY)) / (log10(m_maxValY) - log10(m_minValY)))));
    }
}
