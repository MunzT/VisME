/***********************************************************************************************//**
 * @author Tanja Munz
 * @file microsaccadetimeitem.h
 **************************************************************************************************/
#ifndef MICROSACCADETIMEITEM_H
#define MICROSACCADETIMEITEM_H

#include "fixation.h"
#include "settings.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

/***********************************************************************************************//**
 * This calss represents a microsaccade element containing a QPainterPath for its visualization.
 **************************************************************************************************/
class MicrosaccadeTimeItem : public QObject, public QGraphicsRectItem
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    MicrosaccadeTimeItem(Settings* settings, Fixation* fixation, QRectF rect);

    /*******************************************************************************************//**
     * Returns the corresponding microsaccade.
     *
     * @return corresponding microsaccade
     **********************************************************************************************/
    Fixation* fixation() const;

protected:

    /*******************************************************************************************//**
     * Paints the item.
     *
     * @param painter painter that paints the item
     * @param option  style option of the item
     **********************************************************************************************/
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);

    /*******************************************************************************************//**
     * When a mouse button is pressed, the item should be selected (no other
     * items are selected) and information about the item should be shown in
     * the dock.
     **********************************************************************************************/
    void mousePressEvent(QGraphicsSceneMouseEvent*);

    /*******************************************************************************************//**
     * Returns the item's bounding rect.
     *
     * @return item's bounding rect
     **********************************************************************************************/
    QRectF boundingRect() const;

private:

    /// Current settings for visualizations.
    Settings* m_settings;

    /// Related fixation
    Fixation* m_fixation;

};

#endif // MICROSACCADETIMEITEM_H
