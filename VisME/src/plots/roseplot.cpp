/***********************************************************************************************//**
 * @author Tanja Munz
 * @file roseplot.cpp
 **************************************************************************************************/
#include "mathhelpfunctions.h"
#include "roseplot.h"

RosePlot::RosePlot(Settings* settings, bool withHole, bool withArcs) :
    RadialHistogram(settings, withHole),
    m_withArcs(withArcs)
{}


void RosePlot::draw(QPainter& painter)
{
    if (m_trials.empty())
    {
        return;
    }

    int holeRadius = 0;
    if (m_withHole)
    {
        holeRadius = m_plotRadius / 5;
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

    // method to see inner points better (seem to be smaller than they actually are as the
    // bars get larger on the outside of the area
    drawInnerCircle(holeRadius, painter, baseColor);

    reorderTrialsByTestCondition();

    std::vector<QString> testConditionsForAngles;
    std::vector<std::vector<qreal> > angleValues = getAngleData(m_settings->m_overplot, testConditionsForAngles);
    std::vector<qreal> allAngleValues = MathHelpFunctions::toVector(angleValues);
    std::vector<std::vector<qreal> > microsaccadeValuesOrdered =
            dataToBins(angleValues, m_settings->m_dataPlotBins);

    if (microsaccadeValuesOrdered.empty())
    {
        return;
    }

    QTransform t = painter.transform();

    if (m_settings->m_dataPlotRadiusCount <= 0)
    {
        if (!angleValues.empty())
        {
            for (size_t j = 0; j < microsaccadeValuesOrdered[0].size(); ++j)
            {
                int tempMax = 0;
                for (size_t i = 0; i < microsaccadeValuesOrdered.size(); ++i)
                {
                    tempMax += microsaccadeValuesOrdered[i][j];

                }
                max = std::fmax(max, tempMax);
            }
        }
    }
    else
    {
        max = m_settings->m_dataPlotRadiusCount;
    }

    if (max <= 0) // no valid values...
    {
        return;
    }

    // rose plot
    binSize = 360.0 / microsaccadeValuesOrdered[0].size();

    // RosePlot or RadialPlot
    bool plotArc = m_settings->m_dataPlotType == PT_RosePlot ||
                   m_settings->m_dataPlotType == PT_RosePlotWithHole;

    if (plotArc)
    {
        painter.rotate(-90 - binSize/2.0);
    }

    std::vector<qreal> previousHight;
    previousHight.assign(microsaccadeValuesOrdered[0].size(), holeRadius);
    for (size_t i = 0; i < microsaccadeValuesOrdered.size(); ++i) // trials
    {
        QPair<QColor, QColor> colors = changePainterForTestCondition(painter, i, testConditionsForAngles);

        for (size_t j = 0; j < microsaccadeValuesOrdered[i].size(); ++j)
        {
            // https://stackoverflow.com/questions/17118967/drawarc-are-not-drawen-correctly-qt-c
            QPainterPath path;
            qreal val  = (m_plotRadius - holeRadius) *
                         microsaccadeValuesOrdered[i][j] / max;

            if (plotArc)
            {
                QRectF outer_rect(-previousHight[j] - val, -previousHight[j] - val, 2 * (val + previousHight[j]), 2 * (val + previousHight[j]));
                QRectF inner_rect(-previousHight[j], -previousHight[j], 2 * previousHight[j], 2 * previousHight[j]);

                path.arcMoveTo(outer_rect, 0);
                path.arcTo(outer_rect, 0, -binSize);
                path.lineTo(ellipseCoordinates(inner_rect, -binSize));
                path.arcTo(inner_rect, -binSize, binSize);
                path.lineTo(ellipseCoordinates(outer_rect, 0));
                path.closeSubpath();
                painter.fillPath(path,
                                 colors.second.isValid() ? colors.second : plotColorTransparent);
                painter.strokePath(path,
                                   colors.first.isValid() ? colors.first : plotColor);

                previousHight[j] = previousHight[j] + val;
            }
            else
            {
                // Plot with rects
                QPointF pointLeftLow = rotatePoint(QPointF(0, 0), -binSize / 2.0, QPointF(0, holeRadius));
                pointLeftLow.setY(previousHight[j]);
                QRectF rect = QRectF(-pointLeftLow.x(), -pointLeftLow.y() - val,
                                     pointLeftLow.x() * 2, val);
                painter.drawRect(rect);
                previousHight[j] = pointLeftLow.y() + val;
            }

            painter.rotate(binSize);
        }
    }

    painter.setTransform(t);

    // mean
    drawMean(painter, m_plotRadius, holeRadius, allAngleValues, *m_settings->m_colors.meanValueColor);

    drawMinMax(painter, baseColor, max);
}
