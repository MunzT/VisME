/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainscene.h
 **************************************************************************************************/
#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "mainvisualization.h"
#include "trial.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class MainVisualization;

/***********************************************************************************************//**
 * Graphics scene for the main visualization.
 **************************************************************************************************/
class MainScene : public QGraphicsScene
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    MainScene(MainVisualization* mainVis, Settings* settings, QObject* parent = 0);

    /*******************************************************************************************//**
     * Updates the size of the scene rect depening on the data of the current trial.
     **********************************************************************************************/
    void updateSceneRectSize();

protected:

    /*******************************************************************************************//**
     * Draws the foreground for the main visualization containing microsaccades and other
     * non-interactive elements. Settings from the user interface specifies which items will be
     * shown.
     **********************************************************************************************/
    void drawForeground(QPainter* painter, const QRectF& rect);

    /*******************************************************************************************//**
     * Draws the background for the main visualization containing gaze data and other
     * non-interactive elements. Settings from the user interface specifies which items will be
     * shown.
     **********************************************************************************************/
    void drawBackground(QPainter* painter, const QRectF& rect);

    /*******************************************************************************************//**
     * Mouse double click event: Removes selection of fixations.
     *
     * @param event mouse event
     **********************************************************************************************/
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

signals:

    /*******************************************************************************************//**
     * Signal when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

private:

    /*******************************************************************************************//**
     * Returns a list for each fixation containing the fixation's microsaccade coordinates.
     *
     * @return list for each fixation containing the fixation's microsaccade coordinates
     **********************************************************************************************/
    std::vector<std::vector<std::vector<QPointF> > > getMicrosaccadesGazeData(Trial* trial);

    /*******************************************************************************************//**
     * Returns a list for each fixation containing the fixation's microsaccade coordinates.
     *
     * @param microsaccadeGazeData microsaccade data for drawing
     * @param painter              painter for drawing
     **********************************************************************************************/
    void drawMicrosaccadeDirections(std::vector<std::vector<std::vector<QPointF> > >  microsaccadeGazeData,
                                    QPainter* painter);

    /*******************************************************************************************//**
     * DrawLine function which does not draw lines when p1 == p2 as this created artefacts.
     *
     * @param p1      first point
     * @param p2      second point
     * @param painter painter for drawing
     **********************************************************************************************/
    void secureDrawLine(QPointF p1, QPointF p2, QPainter* painter);

    /*******************************************************************************************//**
     * Scales a value for the view that it always hase a fixed visible size.
     *
     * @param value
     * @return scaled value
     **********************************************************************************************/
    qreal scaleToView(qreal value);


    /// Main visualization container.
    MainVisualization* m_mainVis;

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;
};

#endif // MAINSCENE_H
