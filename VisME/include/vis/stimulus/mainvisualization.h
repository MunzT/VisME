/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainvisualization.h
 **************************************************************************************************/
#ifndef MAINVISUALIZATION_H
#define MAINVISUALIZATION_H

#include "eyetrackingdata.h"
#include "fixation.h"
#include "fixationitem.h"
#include "mainwindow.h"
#include "settings.h"
#include <QGraphicsScene>

class FixationItem;

/***********************************************************************************************//**
 * This class holds all elements and information required for the main visualization.
 **************************************************************************************************/
class MainVisualization : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     *
     * @param settings settings used for the visualization
     **********************************************************************************************/
    MainVisualization(Settings* settings);

    /*******************************************************************************************//**
     * Updates the fixations data which will be visualised in the mainView.
     **********************************************************************************************/
    void updateTrial();

    /*******************************************************************************************//**
     * Updates the visibility for all fixation items.
     **********************************************************************************************/
    void updateVisibility();

    /*******************************************************************************************//**
     * Checks if there is a fixation item at the given position.
     *
     * @param pos position
     **********************************************************************************************/
    int itemAt(const QPointF& pos);

    /*******************************************************************************************//**
     * Sets the scene for the visualization.
     *
     * @param scene graphics scene for the visualization.
     **********************************************************************************************/
    void setScene(QGraphicsScene* scene);

    /*******************************************************************************************//**
     * Returns the fixation item for the given index.
     *
     * @param index index of the fixation
     * @return fixation item of the given index
     **********************************************************************************************/
    FixationItem* getFixationDataAt(int index) const;

signals:

    /*******************************************************************************************//**
     * Signal when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

private:

    /*******************************************************************************************//**
     * Creates a fixation item for the given fixation and adds it to the scene.
     *
     * @param fixation fixation for which an item should be created and added
     * @param index    index for the fixation
     **********************************************************************************************/
    void createAndAddItemToScene(Fixation* fixation, int index);

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// Scene for the presentation of the eye tracking data.
    QGraphicsScene* m_scene;

    /// Eye tracking data containing all participant data.
    EyeTrackingData* m_data;

    /// Fixation data: centroids and duration.
    std::vector<FixationItem*> m_fixationData;

    /// HAsh from fixation to item.
    QHash<Fixation*, FixationItem*> m_fixationToItemHash;

};

#endif // MAINVISUALIZATION_H
