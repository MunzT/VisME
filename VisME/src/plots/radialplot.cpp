/***********************************************************************************************//**
 * @author Tanja Munz
 * @file radialplot.cpp
 **************************************************************************************************/
#define _USE_MATH_DEFINES
#include "mathhelpfunctions.h"
#include "radialplot.h"
#include "settings.h"
#include <cmath>
#include <QPainter>
#include <QStyleOption>


RadialPlot::RadialPlot() :
    AbstractPlot()
{
    m_horizontallyCentered = true;
    m_verticallyCentered = true;
    setStyleSheet("background-color:white;");
}


void RadialPlot::resizeEvent(QResizeEvent* event)
{
    AbstractPlot::resizeEvent(event);

    m_minSpace = width() <= height() ? width() : height();
    m_spacingV = int(m_minSpace / 300.0 * 15.0);
    m_spacingH = int(m_minSpace / 300.0 * 35.0);
    m_plotRadius = int(m_maxSize / 2.0 - m_spacingH);
    m_padding = int(m_minSpace / 300.0 * 6.0);
    m_padding2 = int(m_minSpace / 300.0 * 120.0);
}


void RadialPlot::drawInnerCircle(const qreal holeRadius, QPainter& painter, const QColor color)
{
    if (holeRadius > 0)
    {
        painter.save();
        painter.setPen(color);
        painter.setBrush(Qt::transparent);
        painter.drawEllipse(QPointF(0, 0), holeRadius, holeRadius);
        painter.restore();
    }
}


void RadialPlot::drawOuterCircle(QPainter& painter, const QColor color)
{
    painter.save();

    painter.setPen(color);
    painter.drawEllipse(QPointF(0, 0), m_plotRadius, m_plotRadius);

    painter.restore();
}


void RadialPlot::drawMinMax(QPainter& painter, const QColor color, qreal max, bool degree)
{
    painter.save();

    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setOpacity(1);
    painter.setPen(color);

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(int(m_spacingV - 2));
    painter.setFont(font);

    painter.drawText(m_plotRadius + m_spacingH - m_padding2, 0,
                     m_padding2, m_plotRadius + m_spacingV,
                     Qt::AlignBottom | Qt::AlignLeft, tr("min: 0"));
    painter.drawText(m_plotRadius + m_spacingH - m_padding2, 0,
                     m_padding2, m_plotRadius + m_spacingV * 2,
                     Qt::AlignBottom | Qt::AlignLeft, tr("max: %1%2").arg(QString::number(max),
                              degree ? "°" : ""));

    painter.restore();
}


void RadialPlot::drawValues(QPainter& painter, const QColor color)
{
    painter.save();

    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setOpacity(1);
    painter.setPen(color);

    QString textTop = "0°";
    QString textRight = "90°";
    QString textBottom = "180°";
    QString textLeft = "270°";

    QFont font = painter.font();
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_spacingV - 2);
    painter.setFont(font);

    painter.drawText(-m_spacingH/2, m_plotRadius + m_padding, m_spacingH, m_spacingV,
                     Qt::AlignBottom | Qt::AlignHCenter, textBottom);
    painter.drawText(m_plotRadius + m_padding, - m_spacingV/2, m_spacingH, m_spacingV,
                     Qt::AlignVCenter | Qt::AlignLeft, textRight);
    painter.drawText(-m_spacingH/2, -m_plotRadius - m_spacingV - m_padding, m_spacingH, m_spacingV,
                     Qt::AlignTop | Qt::AlignHCenter, textTop);
    painter.drawText(-m_plotRadius - m_spacingH - m_padding, - m_spacingV/2, m_spacingH, m_spacingV,
                     Qt::AlignVCenter | Qt::AlignRight, textLeft);

    painter.restore();
}


QPointF RadialPlot::rotatePoint(QPointF origin, qreal angle, QPointF p)
{
    qreal s = std::sin((angle + 180) / 180 * M_PI);
    qreal c = std::cos((angle + 180) / 180 * M_PI);

    // translate point to origin
    p.setX(p.x() - origin.x());
    p.setY(p.y() - origin.y());

    // rotate point
    qreal xnew = p.x() * c - p.y() * s;
    qreal ynew = p.x() * s + p.y() * c;

    // translate point back
    p.setX(xnew + origin.x());
    p.setY(ynew + origin.y());

    p.setX(xnew);
    p.setY(ynew);

    return p;
}


void RadialPlot::drawAxis(const qreal holeRadius, const qreal plotRadius,
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


void RadialPlot::drawMean(QPainter& painter, int plotRadius, int holeRadius,
                          std::vector<qreal> angles, const QColor color)
{
    std::sort(angles.data(), angles.data() + angles.size());
    qreal mean = MathHelpFunctions::angularMean(angles);
    painter.setPen(QPen(color, 2));

    painter.drawLine(rotatePoint(QPointF(0, 0), mean, QPointF(0, holeRadius)),
                     rotatePoint(QPointF(0, 0), mean, QPointF(0, plotRadius)));

    // variance
    std::vector<qreal> differences;
    for (size_t i = 0; i < angles.size(); ++i)
    {
        differences.push_back(MathHelpFunctions::angularDifference(angles[i], mean));
    }

    std::vector<qreal> temp = MathHelpFunctions::squareComponents(differences);
    qreal variance = std::accumulate(temp.begin(), temp.end(), 0.0) / temp.size();
    qreal standardDeviation = sqrt(variance);

    painter.drawLine(rotatePoint(QPointF(0, 0), mean + standardDeviation,
                                 QPointF(0, plotRadius + 10)),
                     rotatePoint(QPointF(0, 0), mean + standardDeviation,
                                 QPointF(0, plotRadius - 10)));

    painter.drawLine(rotatePoint(QPointF(0, 0), mean - standardDeviation,
                                 QPointF(0, plotRadius + 10)),
                     rotatePoint(QPointF(0, 0), mean - standardDeviation,
                                 QPointF(0, plotRadius - 10)));

    // The startAngle and spanAngle must be specified in 1/16th of a degree:
    qreal scale = 16;
    painter.drawArc(QRect(-plotRadius, -plotRadius, 2 * plotRadius, 2 * plotRadius),
                    int((90 + (- mean - standardDeviation)) * scale), int((2 * standardDeviation) * scale));
}


QPointF RadialPlot::ellipseCoordinates(const QRectF& r, qreal angle)
{
    // https://stackoverflow.com/questions/17118967/drawarc-are-not-drawen-correctly-qt-c
    QPainterPath path;
    path.arcMoveTo(r, angle);
    return path.currentPosition();
}
