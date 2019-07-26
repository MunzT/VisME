/***********************************************************************************************//**
 * @author Tanja Munz
 * @file polarplot.cpp
 **************************************************************************************************/
#include "mathhelpfunctions.h"
#include "polarplot.h"


PolarPlot::PolarPlot(Settings* settings, bool withHole) :
    RadialHistogram(settings, withHole)
{
}


void PolarPlot::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    int holeRadius = 0;
    if (m_withHole)
    {
        holeRadius = int(m_plotRadius / 5);
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
    qreal binSize = 0;
    qreal max = 0;

    drawAxis(holeRadius, m_plotRadius, painter, baseColor);

    std::vector<QString> testConditionsForAngles;
    std::vector<std::vector<qreal> > angleValues = getAngleData(m_settings->m_overplot, testConditionsForAngles);
    std::vector<qreal> allAngleValues = MathHelpFunctions::toVector(angleValues);
    std::vector<std::vector<qreal> > orderedValues = dataToBins(angleValues, m_settings->m_dataPlotBins);

    drawInnerCircle(holeRadius, painter, baseColor);

    if (orderedValues.empty())
    {
        return;
    }

    // draw polar plot
    binSize = 360.0 / orderedValues[0].size();

    if (m_settings->m_dataPlotRadiusCount <= 0)
    {
        max = MathHelpFunctions::maxValue(orderedValues);
    }
    else
    {
        max = m_settings->m_dataPlotRadiusCount;
    }

    if (max <= 0) // no valid values available
    {
        return;
    }

    for (size_t j = 0; j < orderedValues.size(); ++j)
    {
        changePainterForTestCondition(painter, j, testConditionsForAngles);

        QPainterPath path = QPainterPath();
        QPolygonF polygon;

        for (size_t i = 0; i < orderedValues[j].size(); ++i)
        {
            qreal val = holeRadius + (m_plotRadius - holeRadius) * orderedValues[j][i] / max;
            polygon << rotatePoint(QPointF(0, 0), i * binSize, QPointF(0, val));
        }
        polygon << polygon.first(); // same as first point

        path.addPolygon(polygon);

        QPainterPath circlePath = QPainterPath();
        circlePath.moveTo(holeRadius,0);
        circlePath.arcTo(-holeRadius, -holeRadius, 2 * holeRadius, 2 * holeRadius, 0, 360);
        path.addPath(circlePath);

        painter.drawPath(path);
    }

    // mean
    drawMean(painter, m_plotRadius, holeRadius, allAngleValues, *m_settings->m_colors.meanValueColor);

    drawMinMax(painter, baseColor, max);
}
