/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianhistogram.cpp
 **************************************************************************************************/
#include "cartesianhistogram.h"
#include "cartesianplot.h"


CartesianHistogram::CartesianHistogram(Settings* settings) :
    CartesianPlot(),
    AbstractEyeDataForCartesianPlot(settings),
    m_diagramShift(0)
{}


void CartesianHistogram::draw(QPainter& painter)
{
    // draw bins as histogram
    QPainterPath path;
    path.moveTo(0, getYValueInVis(0));

    qreal binCount = m_data[0].size();

    for (size_t i = 0; i < binCount; ++i)
    {
        // draw bars
        // if no data is given
        qreal xBinSize = qreal(i) / qreal(binCount);
        if (m_maxYValue > 0)
        {
            qreal yVal = 0;
            qreal prevYVal = 0;
            for (size_t j = 0; j < m_data.size(); ++j)
            {
                QColor testConditionColor = m_idToColor[int(j)];
                QColor testConditionColorTransparent = testConditionColor;
                if (!testConditionColor.isValid())
                {
                    testConditionColor = m_mainPlotColor;
                    testConditionColorTransparent = m_mainPlotColor;
                }

                testConditionColorTransparent.setAlpha(m_settings->m_colors.alphaForPlots);

                painter.setPen(QPen(testConditionColor));
                painter.setBrush(testConditionColorTransparent);

                yVal += m_data[j][i] / m_maxYValue;

                bool drawBars = true;

                if (yVal > 0)
                {
                    if (drawBars && yVal != prevYVal)
                    {
                        if (getXValueInVis(1.0 / binCount) - 2 > 0)
                        {
                            painter.drawRect(
                                QRectF(QPointF(m_diagramShift + getXValueInVis(xBinSize) + 1,
                                               getYValueInVis(yVal)),
                                       QPointF(m_diagramShift + getXValueInVis(qreal(i + 1) / qreal(binCount)) - 1,
                                               getYValueInVis(prevYVal))));
                        }
                        else
                        {
                            painter.drawRect(
                                QRectF(QPointF(m_diagramShift + getXValueInVis(xBinSize),
                                               getYValueInVis(yVal)),
                                       QPointF(m_diagramShift + getXValueInVis(qreal(i + 1) / qreal(binCount)),
                                               getYValueInVis(prevYVal))));
                        }
                    }
                }

                prevYVal = yVal;
            }
        }
    }

    painter.drawPath(path);
}
