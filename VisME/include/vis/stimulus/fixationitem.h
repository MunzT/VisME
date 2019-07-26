/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixationitem.h
 **************************************************************************************************/
#ifndef FIXATIONITEM_H
#define FIXATIONITEM_H

#include "fixation.h"
#include "mainvisualization.h"
#include "settings.h"
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPointF>

class MainVisualization;

/***********************************************************************************************//**
 * This calss represents a fixation element containing a QPainterPath for its visualization.
 **************************************************************************************************/
class FixationItem : public QObject, public QGraphicsPathItem
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    FixationItem(Fixation* fixation, Settings* settings, size_t microasaccadesCount);

    /*******************************************************************************************//**
     * Getter method for the center of the fixation.
     *
     * @return center of the fixation.
     **********************************************************************************************/
    QPointF center() const;

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

    /*******************************************************************************************//**
     * Selects this fixation.
     *
     * @param selected if fixation shall be selected
     **********************************************************************************************/
    void setSelected(bool selected);

signals:

    /*******************************************************************************************//**
     * Signal when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

private:

    /// Corresponding fixation.
    Fixation* m_fixation;

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// Number of microsaccades in fixation
    size_t m_microasaccadesCount;

};

#endif // FIXATIONITEM_H
