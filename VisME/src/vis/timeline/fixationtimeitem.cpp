/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixationtimeitem.cpp
 **************************************************************************************************/
#include "fixationtimeitem.h"
#include <QCursor>


FixationTimeItem::FixationTimeItem(Settings* settings, Fixation* fixation, QRectF rect) :
    m_settings(settings), m_fixation(fixation)
{
    setCursor(Qt::CrossCursor);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);

    setRect(rect);
}


Fixation* FixationTimeItem::fixation() const
{
    return m_fixation;
}


void FixationTimeItem::setSelected(bool selected)
{
    //QGraphicsRectItem::setSelected(selected);

    if (selected)
    {
        emit fixationSelected(m_fixation);
    }
}


QRectF FixationTimeItem::boundingRect() const
{
    return rect();
}


void FixationTimeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if (m_fixation->isMarked())
    {
        painter->setPen(QPen(*(m_settings->m_colors.fixationHighlightColor), 0));
        QColor fixationHighlightColorTransparent = *(m_settings->m_colors.fixationHighlightColor);
        fixationHighlightColorTransparent.setAlpha(100);
        painter->setBrush(fixationHighlightColorTransparent);
    }
    else
    {
        QColor fixColor1 = *(m_settings->m_colors.fixationColor);
        QColor fixColor2 = fixColor1;
        fixColor1.setAlpha(int(fixColor1.alpha() / 13.0));
        fixColor2.setAlpha(int(fixColor2.alpha() / 3.0));
        painter->setPen(QPen(fixColor2, 0));
        painter->setBrush(fixColor1);
    }

    painter->drawRect(rect());
}


void FixationTimeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() != Qt::NoModifier)
    {
        event->ignore();
        return;
    }
    // also perform standard functions for the new selection
    QGraphicsRectItem::mousePressEvent(event);

    setSelected(true);
}

