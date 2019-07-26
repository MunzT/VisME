/***********************************************************************************************//**
 * @author Tanja Munz
 * @file MicrosaccadeTimeItem.cpp
 **************************************************************************************************/
#include "microsaccadetimeitem.h"
#include <QCursor>


MicrosaccadeTimeItem::MicrosaccadeTimeItem(Settings* settings, Fixation* fixation, QRectF rect) :
    m_settings(settings), m_fixation(fixation)
{
    setRect(rect);

    setCursor(Qt::CrossCursor);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
}


void MicrosaccadeTimeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QColor pinkMoreTransparent = *(m_settings->m_colors.microsaccadesColor);
    pinkMoreTransparent.setAlpha(50);
    painter->setPen(QPen(pinkMoreTransparent, 0));
    painter->setBrush(pinkMoreTransparent);

    painter->drawRect(rect());
}


void MicrosaccadeTimeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->modifiers() != Qt::NoModifier)
    {
        event->ignore();
        return;
    }

    // also perform standard functions for the new selection
    QGraphicsRectItem::mousePressEvent(event);

    // necessary for qreal click
    event->ignore();
}


QRectF MicrosaccadeTimeItem::boundingRect() const
{
    return rect();
}


Fixation* MicrosaccadeTimeItem::fixation() const
{
    return m_fixation;
}
