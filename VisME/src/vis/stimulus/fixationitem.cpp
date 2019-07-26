/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixationitem.cpp
 **************************************************************************************************/
#include "fixationitem.h"
#include <QCursor>


FixationItem::FixationItem(Fixation* fixation,
                           Settings* settings,
                           size_t microasaccadesCount) :
    m_fixation(fixation),
    m_settings(settings),
    m_microasaccadesCount(microasaccadesCount)
{
    setCursor(Qt::CrossCursor);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    setPos(fixation->getXPosition(), fixation->getYPosition());
}


QPointF FixationItem::center() const
{
    return QPointF(m_fixation->getXPosition(), m_fixation->getYPosition());
}


void FixationItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!m_settings->m_showScanpath)
    {
        return;
    }

    QPainterPath fixPath;

    qreal scaleValue = m_settings->m_fixationScale / 100.0;
    if (m_settings->m_fixationSize == FromDuration)
    {
        qreal radius = m_fixation->getDuration() / 50.0 * scaleValue;
        fixPath.addEllipse(QRectF(-radius, -radius, 2 * radius, 2 * radius));
    }
    else if (m_settings->m_fixationSize == Equal)
    {
        qreal radius = 15 * scaleValue;
        fixPath.addEllipse(QRectF(-radius, -radius, 2 * radius, 2 * radius));
    }
    else if (m_settings->m_fixationSize == FromMicrosaccadesCount)
    {
        qreal radius = m_microasaccadesCount * 6 * scaleValue;
        fixPath.addEllipse(QRectF(-radius, -radius, 2 * radius, 2 * radius));
    }

    setPath(fixPath);

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(QPen(*(m_settings->m_colors.fixationColor), 0));
    QColor fixationColorTransparent = *(m_settings->m_colors.fixationColor);
    fixationColorTransparent.setAlpha(std::max(50, m_settings->m_colors.fixationColor->alpha() - 20));
    painter->setBrush(fixationColorTransparent);

    if (m_fixation->isMarked())
    {
        painter->setPen(QPen(*(m_settings->m_colors.fixationHighlightColor), 0));
        painter->setBrush(*(m_settings->m_colors.fixationHighlightColor));
    }
    painter->drawPath(path());
}


void FixationItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() != Qt::NoModifier)
    {
        event->ignore();
        return;
    }

    // also perform standard functions for the new selection
    QGraphicsPathItem::mousePressEvent(event);

    setSelected(true);
}


void FixationItem::setSelected(bool selected)
{
    //QGraphicsPathItem::setSelected(selected);

    if (selected)
    {
        emit fixationSelected(m_fixation);
    }
}
