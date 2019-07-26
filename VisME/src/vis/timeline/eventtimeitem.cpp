/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eventtimeitem.cpp
 **************************************************************************************************/
#include "eventtimeitem.h"
#include <QCursor>


EventTimeItem::EventTimeItem(Settings* settings, Event* event, QRectF rect) :
    m_settings(settings), m_event(event)
{
    setCursor(Qt::CrossCursor);
    setAcceptHoverEvents(true);

    setRect(rect);
}


QRectF EventTimeItem::boundingRect() const
{
    return rect();
}


void EventTimeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(QPen(*(m_settings->m_colors.eventColor), 0));
    QColor fixationHighlightColorTransparent = *(m_settings->m_colors.eventColor);
    fixationHighlightColorTransparent.setAlpha(fixationHighlightColorTransparent.alpha() / 2);
    painter->setBrush(fixationHighlightColorTransparent);

    painter->drawRect(rect());
}


Event* EventTimeItem::event() const
{
    return m_event;
}

