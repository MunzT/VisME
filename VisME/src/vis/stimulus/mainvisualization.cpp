/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainvisualization.cpp
 **************************************************************************************************/
#include "mainvisualization.h"


MainVisualization::MainVisualization(Settings* settings) :
    m_settings(settings),
    m_scene(nullptr),
    m_data(nullptr)
{}


void MainVisualization::updateTrial()
{
    m_fixationData.clear();
    m_fixationToItemHash.clear();
    m_scene->clear();

    if (m_settings->m_currentTrials.empty() ||
            m_settings->m_currentTrials[0]->fixationsCount() <= 0)
    {
        return;
    }

    for (int i = 0; i < m_settings->m_currentTrials[0]->fixationsCount(); ++i)
    {
        Fixation* f = m_settings->m_currentTrials[0]->getFixationAt(i);
        createAndAddItemToScene(f, i);
    }

    updateVisibility();
}


void MainVisualization::updateVisibility()
{
    if (m_settings->m_displayMode != Individual)
    {
        m_scene->clear();
        return;
    }

    if (m_settings->m_currentTrials.size() == 0)
    {
        return;
    }

    Trial* trial = m_settings->m_currentTrials[0];

    if (m_settings->m_showNeighboringFixations)
    {
        for (size_t i = 0; i < m_fixationData.size(); ++i)
        {
        if ((m_settings->m_currentFixationIndex > -1 &&
             i <= size_t(m_settings->m_currentFixationIndex + m_settings->m_followingFixationsCount) &&
             i >= size_t(m_settings->m_currentFixationIndex - m_settings->m_previousFixationsCount)) &&
             (!trial->outsideTimeLimitIfActivated(trial->getFixationAt(int(i))->getStartIndex(),
                                                                 trial->getFixationAt(int(i))->getEndIndex())))
            {
                m_fixationData[i]->setVisible(m_settings->m_showScanpath);
            }
            else
            {
                m_fixationData[i]->setVisible(false);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_fixationData.size(); ++i)
        {
            if (!trial->outsideTimeLimitIfActivated(trial->getFixationAt(int(i))->getStartIndex(),
                                                    trial->getFixationAt(int(i))->getEndIndex()))
            {
                m_fixationData[i]->setVisible(m_settings->m_showScanpath);
            }
            else
            {
                m_fixationData[i]->setVisible(false);
            }
        }
    }
}


int MainVisualization::itemAt(const QPointF& pos)
{
    if (!m_settings->m_showScanpath)
    {
        return -1;
    }
    for (int i = int(m_fixationData.size()) - 1; i >= 0; --i)
    {
        const FixationItem* item = m_fixationData[size_t(i)];

        if (item->isVisible() && item->path().contains(pos - QPointF(item->center().x(),
                                                                     item->center().y())))
        {
            return i;
        }
    }
    return -1;
}


void MainVisualization::setScene(QGraphicsScene* scene)
{
    m_scene = scene;
}


FixationItem* MainVisualization::getFixationDataAt(int index) const
{
    if (index >= 0 && index < int(m_fixationData.size()))
    {
        return m_fixationData[size_t(index)];
    }
    return nullptr;
}


void MainVisualization::createAndAddItemToScene(Fixation* fixation, int index)
{
    if (m_fixationToItemHash.value(fixation) == nullptr && !m_settings->m_currentTrials.empty())
    {
        auto newFixationItem =
                new FixationItem(fixation, m_settings,
                                 fixation->getMicrosaccadesCount(m_settings->m_currentTrials[0]->msFromInputFile()));
        QObject::connect(newFixationItem, &FixationItem::fixationSelected,
                         this, &MainVisualization::fixationSelected);
        newFixationItem->setToolTip(QString::number(index));

        //m_fixationToItemHash.value(fixation)->show();

        m_fixationData.push_back(newFixationItem);
        m_fixationToItemHash.insert(fixation, newFixationItem);

        m_scene->addItem(newFixationItem);
    }
    else if (m_fixationToItemHash.value(fixation)->scene() != m_scene)
    {
        m_scene->addItem(m_fixationToItemHash.value(fixation));
    }
}
