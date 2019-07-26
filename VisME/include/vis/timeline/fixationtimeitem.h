/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixationtimeitem.h
 **************************************************************************************************/
#ifndef FIXATIONTIMEITEM_H
#define FIXATIONTIMEITEM_H

#include "fixation.h"
#include "settings.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

/***********************************************************************************************//**
 * This calss represents a fixation element containing a QPainterPath for its visualization.
 **************************************************************************************************/
class FixationTimeItem : public QObject, public QGraphicsRectItem
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     *
     * @param settings global settings
     * @param fixation corresponding fixation
     * @param rect     rectangle used for visualization in timeline
     **********************************************************************************************/
    FixationTimeItem(Settings* settings, Fixation* fixation, QRectF rect);

    /*******************************************************************************************//**
     * Returns the corrsponding fixation.
     *
     * @return corrsponding fixation
     **********************************************************************************************/
    Fixation* fixation() const;

    /*******************************************************************************************//**
     * Sets item to be selected.
     *
     * @param selected if fixation shall be selected
     **********************************************************************************************/
    void setSelected(bool selected);

    /*******************************************************************************************//**
     * Returns the item's bounding rect.
     *
     * @return item's bounding rect
     **********************************************************************************************/
    QRectF boundingRect() const;

signals:

    /*******************************************************************************************//**
     * Signal when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

protected:

    /*******************************************************************************************//**
     * Paints the item.
     *
     * @param painter painter that paints the item
     **********************************************************************************************/
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);

    /*******************************************************************************************//**
     * When a mouse button is pressed, the item should be selected (no other
     * items are selected) and information about the item should be shown in
     * the dock.
     **********************************************************************************************/
    void mousePressEvent(QGraphicsSceneMouseEvent*);

private:

    /// Current settings for the visualization.
    Settings* m_settings;

    /// Corresponding fixation.
    Fixation* m_fixation;
};

#endif // FIXATIONTIMEOVERLAY_H
