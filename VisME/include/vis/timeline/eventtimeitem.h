/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eventtimeitem.h
 **************************************************************************************************/
#ifndef EVENTTIMEOVERLAY_H
#define EVENTTIMEOVERLAY_H

#include "event.h"
#include "settings.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

/***********************************************************************************************//**
 * This calss represents an event element containing a QPainterPath for its visualization.
 **************************************************************************************************/
class EventTimeItem : public QObject, public QGraphicsRectItem
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     *
     * @param settings global settings
     * @param event corresponding event
     * @param rect  rectangle used for visualization in timeline
     **********************************************************************************************/
    EventTimeItem(Settings* settings, Event* event, QRectF rect);

    /*******************************************************************************************//**
     * Returns the item's bounding rect.
     *
     * @return item's bounding rect
     **********************************************************************************************/
    QRectF boundingRect() const;

    /*******************************************************************************************//**
     * Returns the corresponding event.
     *
     * @return the corresponding event
     **********************************************************************************************/
    Event* event() const;

protected:

    /*******************************************************************************************//**
     * Paints the item.
     *
     * @param painter painter that paints the item
     **********************************************************************************************/
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

private:

    /// Current settings for the visualization.
    Settings* m_settings;

    /// Corresponding fixation.
    Event* m_event;
};

#endif // EVENTTIMEOVERLAY_H
