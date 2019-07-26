/***********************************************************************************************//**
 * @author Tanja Munz
 * @file timelinescene.h
 **************************************************************************************************/
#ifndef TIMELINESCENE_H
#define TIMELINESCENE_H

#include "timelinevisualization.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

/***********************************************************************************************//**
 * Graphics scene for the timeline visualization.
 **************************************************************************************************/
class TimelineScene : public QGraphicsScene
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    TimelineScene(Settings* settings, QObject* parent);

    /*******************************************************************************************//**
     * Updates the size of the scene rect depening on the data of the current trial.
     **********************************************************************************************/
    void updateSceneRectSize();

    /*******************************************************************************************//**
     * Updates the maximum height of the scene.
     *
     * @param maxHeight maximum height of the scene
     **********************************************************************************************/
    void setMaxHeight(const qreal& maxHeight);

protected:

    /*******************************************************************************************//**
     * Draws the background of the timeline: velocity, x and y plots.
     *
     * @param painter painter for drawing
     * @param rect    exposed rectangle
     **********************************************************************************************/
    void drawBackground(QPainter* painter, const QRectF& rect);

private:

    /*******************************************************************************************//**
     * Checks if the given index is inside the current fixation.
     *
     * @param index gaze position index
     **********************************************************************************************/
    bool isInsideCurrentFixation(int index);

    /*******************************************************************************************//**
     * Checks if the given index is inside any fixation.
     *
     * @param index gaze position index
     **********************************************************************************************/
    bool isInsideFixation(int index);

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// Maximum height of the scene.
    qreal m_maxHeight;
};

#endif // TIMELINESCENE_H
