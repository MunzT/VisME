/***********************************************************************************************//**
 * @author Tanja Munz
 * @file cartesianplot.cpp
 **************************************************************************************************/
#include "cartesianplot.h"

CartesianPlot::CartesianPlot() :
    AbstractPlot()
{
    m_horizontallyCentered = false;
    m_verticallyCentered = false;
    setStyleSheet("background-color:white;");
}


void CartesianPlot::drawAxis(const int plotSize, const int binSize, const QString leftText, const QString rightText,
                            qreal smallxVal, qreal maxxVal, qreal maxyVal,
                            QPainter& painter, const QColor color, bool drawMaxMarker, bool drawMinMarker)
{
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.save();

    painter.setPen(color);

    if (!m_horizontallyCentered && !m_verticallyCentered)
    {
        // x, y axis
        int additionalLength = 4;
        painter.drawLine(QPointF(-additionalLength, plotSize),
                         QPointF(plotSize + additionalLength, plotSize));
        painter.drawLine(QPointF(0, -additionalLength), QPointF(0, plotSize + additionalLength));

        // marker
        int markerSize = 2;

        if (drawMaxMarker)
        {
            painter.drawLine(QPointF(plotSize, -markerSize + plotSize),
                             QPointF(plotSize, markerSize + plotSize));
            painter.drawText(0, plotSize + plotSize/70, 2 * plotSize, plotSize/20,
                             Qt::AlignVCenter | Qt::AlignHCenter, QString::number(maxxVal, 'f', 2));
        }

        if (drawMinMarker && maxxVal != smallxVal && maxxVal > 0)
        {
            painter.drawLine(QPointF(binSize, -markerSize + plotSize),
                             QPointF(binSize, markerSize + plotSize));
            painter.drawText(-plotSize, plotSize + plotSize/70,
                             binSize * 2 + 2 * plotSize, plotSize/20,
                             Qt::AlignVCenter | Qt::AlignHCenter, QString::number(smallxVal, 'f', 2));
        }

        if (maxyVal > 0)
        {
            if (drawMaxMarker)
            {
                painter.drawLine(QPointF(-markerSize, 0), QPointF(markerSize, 0));
                painter.drawText(-plotSize, -plotSize, plotSize - plotSize/30, 2 * plotSize,
                                 Qt::AlignVCenter | Qt::AlignRight, QString::number(maxyVal, 'f', 2));
            }

            if (drawMinMarker && maxyVal != 1)
            {
                painter.drawLine(QPointF(-markerSize, plotSize - plotSize/maxyVal),
                                 QPointF(markerSize, plotSize - plotSize/maxyVal));
                painter.drawText(-plotSize, int(-2 * plotSize/maxyVal), plotSize - plotSize/30,
                                 int(2 * plotSize/maxyVal + 2 * plotSize),
                                 Qt::AlignVCenter | Qt::AlignRight, QString::number(1, 'f', 2));
            }
        }

        // x and y axis
        QFont font = painter.font();
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setPixelSize(font.pixelSize() * 1.1);
        painter.setFont(font);

        painter.drawText(-plotSize, plotSize + plotSize/12, 3 * plotSize, plotSize/11,
                         Qt::AlignVCenter | Qt::AlignHCenter, rightText);
        painter.save();
        painter.rotate(90);
        painter.translate(0, -plotSize);
        painter.drawText(-plotSize, plotSize + plotSize/12, 3 * plotSize, plotSize/11,
                         Qt::AlignVCenter | Qt::AlignHCenter, leftText);
        painter.restore();
    }

    painter.restore();
}


void CartesianPlot::drawHorizontallyCenteredAxis(QPainter& painter, const int plotSize,
                                                 const QColor color, const QString dataDescription,
                                                 qreal maxyVal)
{
    painter.save();

    painter.setPen(color);

    if (m_horizontallyCentered)
    {
        // x, y axis
        int additionalLength = 4;
        painter.drawLine(QPointF(- additionalLength, plotSize),
                         QPointF(plotSize + additionalLength, plotSize));
        painter.drawLine(QPointF(plotSize/2, -additionalLength),
                         QPointF(plotSize/2, plotSize + additionalLength));

        int markerSize = 2;
        painter.drawLine(QPointF(plotSize, plotSize - markerSize),
                         QPointF(plotSize, plotSize + markerSize));
        painter.drawLine(QPointF(0, plotSize - markerSize),
                         QPointF(0, plotSize + markerSize));
        painter.drawLine(QPointF(plotSize / 2.0 - markerSize, 0),
                         QPointF(plotSize / 2.0 + markerSize, 0));

        painter.drawText(0, plotSize + plotSize/20, plotSize, plotSize/20,
                         Qt::AlignVCenter | Qt::AlignHCenter, "0");
        painter.drawText(-plotSize, plotSize + plotSize/20, 2 * plotSize, plotSize/20,
                         Qt::AlignVCenter | Qt::AlignHCenter, "-180");
        painter.drawText(0, plotSize + plotSize/20, 2 * plotSize, plotSize/20,
                         Qt::AlignVCenter | Qt::AlignHCenter, "180");

        painter.drawText(0, -2 * plotSize/20, plotSize, plotSize/20,
                         Qt::AlignVCenter | Qt::AlignHCenter,
                         QString("#%1s (max: %2)").arg(dataDescription, QString::number(maxyVal)));
    }

    painter.restore();
}


void CartesianPlot::drawCenteredAxis(const qreal holeRadius, const qreal plotRadius,
                                     QPainter& painter, const QColor color, bool drawMarker)
{
    painter.save();

    painter.setPen(color);

    if (m_horizontallyCentered && m_verticallyCentered)
    {
        // x, y axis
        int additionalLength = 4;
        painter.drawLine(QPointF(0, -holeRadius), QPointF(0, -plotRadius - additionalLength));
        painter.drawLine(QPointF(0, holeRadius), QPointF(0, plotRadius + additionalLength));
        painter.drawLine(QPointF(-holeRadius, 0), QPointF(-plotRadius - additionalLength, 0));
        painter.drawLine(QPointF(holeRadius, 0), QPointF(plotRadius + additionalLength, 0));

        if (drawMarker)
        {
            int markerSize = 2;
            painter.drawLine(QPointF(plotRadius, -markerSize), QPointF(plotRadius, markerSize));
            painter.drawLine(QPointF(-plotRadius, -markerSize), QPointF(-plotRadius, markerSize));
            painter.drawLine(QPointF(-markerSize, plotRadius), QPointF(markerSize, plotRadius));
            painter.drawLine(QPointF(-markerSize, -plotRadius), QPointF(markerSize, -plotRadius));
        }
    }

    painter.restore();
}


void CartesianPlot::setPlotSize()
{
    m_minSpace = width() <= height() ? width() : height();

    m_spacingV = int(m_minSpace / 300.0 * 13.0);
    m_spacingH = int(m_minSpace / 300.0 * 80.0);
    m_plotSize = int(m_maxSize - m_spacingH);
}


QPointF CartesianPlot::getPointInVis(qreal x, qreal y)
{
    return(QPointF(getXValueInVis(x), getYValueInVis(y)));
}


qreal CartesianPlot::getYValueInVis(qreal y)
{
    return m_plotSize - y * m_yScale * m_plotSize;
}


qreal CartesianPlot::getYLength(qreal y)
{
    return y * m_yScale * m_plotSize;
}


qreal CartesianPlot::getXValueInVis(qreal x)
{
    return x * m_xScale * m_plotSize;
}
