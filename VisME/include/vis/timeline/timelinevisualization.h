/***********************************************************************************************//**
 * @author Tanja Munz
 * @file timelinevisualization.h
 **************************************************************************************************/
#ifndef TIMELINEVISUALIZATION_H
#define TIMELINEVISUALIZATION_H

#include "eventtimeitem.h"
#include "eyetrackingdata.h"
#include "fixationtimeitem.h"
#include "microsaccadetimeitem.h"
#include "settings.h"
#include "trial.h"
#include <QGraphicsScene>

/***********************************************************************************************//**
 * This class holds all elements and information required for the timeline visualization.
 **************************************************************************************************/
class TimelineVisualization : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     *
     * @param settings settings used for the visualization
     **********************************************************************************************/
    TimelineVisualization(Settings* settings);

    /*******************************************************************************************//**
     * Updates the fixation and microsaccade data which will be visualised in the timeline view.
     **********************************************************************************************/
    void updateTrial();

    /*******************************************************************************************//**
     * Updates the visibility for all fixation items.
     **********************************************************************************************/
    void updateVisibility();

    /*******************************************************************************************//**
     * Updates the content size visible in the view.
     **********************************************************************************************/
    void updateSize();

    /*******************************************************************************************//**
     * Zooms to the given area defined by min and max.
     **********************************************************************************************/
    void zoomToRange(int min, int max);

    /*******************************************************************************************//**
     * Sets the scene for the visualization.
     *
     * @param scene graphics scene for the visualization.
     **********************************************************************************************/
    void setScene(QGraphicsScene* scene);

private:

    /*******************************************************************************************//**
     * Checks if there is an element item at the given position.
     *
     * @param pos
     **********************************************************************************************/
    int itemAt(const QPointF& pos);

signals:

    /*******************************************************************************************//**
     * Signal when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

private:

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// Scene for the presentation of the eye tracking data.
    QGraphicsScene* m_scene;

    /// Fixation time items for visualization.
    std::vector<FixationTimeItem*> m_fixationData;

    /// Microsaccade time items for visualization.
    std::vector<MicrosaccadeTimeItem*> m_microsaccadeData;
    
    /// Event time items for visualization.
    std::vector<EventTimeItem*> m_eventData;

};

#endif // TIMELINEVISUALIZATION_H
