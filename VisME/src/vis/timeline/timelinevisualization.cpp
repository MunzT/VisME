/***********************************************************************************************//**
 * @author Tanja Munz
 * @file timelinevisualization.cpp
 **************************************************************************************************/
#include "eventtimeitem.h"
#include "timelinescene.h"
#include "timelinevisualization.h"
#include <QGraphicsScene>
#include <QGraphicsView>


TimelineVisualization::TimelineVisualization(Settings* settings) :
    m_settings(settings),
    m_scene(nullptr)
{}


void TimelineVisualization::updateTrial()
{
    if (m_settings->m_currentTrials.empty())
    {
        m_fixationData.clear();
        m_microsaccadeData.clear();
        m_eventData.clear();
        m_scene->clear();
        return;
    }

    Trial* trial = m_settings->m_currentTrials[0];
    qreal maxHeight = std::max(trial->maxY() - trial->minY(),
                               trial->maxX() - trial->minX());

    m_fixationData.clear();
    m_microsaccadeData.clear();
    m_eventData.clear();
    m_scene->clear();
    for (int i = 0; i < trial->fixationsCount(); ++i)
    {
        Fixation* f = trial->getFixationAt(i);
        QRectF r1 = (QRectF(f->getStartIndex() - trial->gazeData().firstKey(),
                            -maxHeight,
                            f->getDuration() - 1,
                            maxHeight));

        auto fixation = new FixationTimeItem(m_settings, f, r1);
        QObject::connect(fixation, &FixationTimeItem::fixationSelected, this,
                         &TimelineVisualization::fixationSelected);
        fixation->setToolTip(QString::number(i));

        m_fixationData.push_back(fixation);
        m_scene->addItem(fixation);

        for (size_t j = 0; j < f->getMicrosaccadesCount(trial->msFromInputFile()); ++j)
        {
            Saccade* ms = trial->getMicrosaccadeAt(i, j);
            qreal start = ms->getOnsetIndex();
            qreal end = ms->getEndIndex();
            QRectF r2 = QRectF(start - trial->gazeData().firstKey(),
                               -maxHeight,
                               end - start,
                               maxHeight);

            auto microsaccade = new MicrosaccadeTimeItem(m_settings, f, r2);
            m_microsaccadeData.push_back(microsaccade);
            m_scene->addItem(microsaccade);
        }
    }

    for (size_t i = 0; i < trial->eventCount(); ++i)
    {
        Event* e = trial->getEventAt(i);
        qreal start = e->getStartIndex();
        qreal end = e->getEndIndex();
        QRectF r3 = QRectF(start - trial->gazeData().firstKey(),
                           -maxHeight,
                           end - start,
                           maxHeight);

        auto event = new EventTimeItem(m_settings, e, r3);
        QString tooltip = e->getName();
        event->setToolTip(tooltip);
        m_eventData.push_back(event);
        m_scene->addItem(event);
    }

    static_cast<TimelineScene*>(m_scene)->setMaxHeight(maxHeight);
}


void TimelineVisualization::updateVisibility()
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

    // fixations
    Trial* trial = m_settings->m_currentTrials[0];
    if (m_settings->m_showNeighboringFixations && m_settings->m_showFixationsInTimeline)
    {
        for (size_t i = 0; i < m_fixationData.size(); ++i)
        {
            if ((m_settings->m_currentFixationIndex > -1 &&
                 i <= size_t(m_settings->m_currentFixationIndex + m_settings->m_followingFixationsCount) &&
                 i >= size_t(m_settings->m_currentFixationIndex - m_settings->m_previousFixationsCount)))
            {
                m_fixationData[i]->setVisible(m_settings->m_showFixationsInTimeline);
            }
            else
            {
                m_fixationData[i]->setVisible(false);
            }

            if (trial->outsideTimeLimitIfActivated(trial->getFixationAt(int(i))->getStartIndex(),
                                                   trial->getFixationAt(int(i))->getEndIndex()))
            {
                m_fixationData[i]->setVisible(false);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < m_fixationData.size(); ++i)
        {
            m_fixationData[i]->setVisible(m_settings->m_showFixationsInTimeline);

            if (trial->outsideTimeLimitIfActivated(trial->getFixationAt(int(i))->getStartIndex(),
                                                   trial->getFixationAt(int(i))->getEndIndex()))
            {
                m_fixationData[i]->setVisible(false);
            }
        }
    }

    // microsaccades
    for (size_t i = 0; i < m_microsaccadeData.size(); ++i)
    {
        if (!trial->outsideTimeLimitIfActivated(m_microsaccadeData[i]->fixation()->getStartIndex(),
                                                m_microsaccadeData[i]->fixation()->getEndIndex()))
        {
            m_microsaccadeData[i]->setVisible(m_settings->m_showMicrosaccadesInTimeline);
        }
        else
        {
            m_microsaccadeData[i]->setVisible(false);
        }
    }

    // show all events
    for (size_t i = 0; i < m_eventData.size(); ++i)
    {
        if (!trial->outsideTimeLimitIfActivated(m_eventData[i]->event()->getStartIndex(),
                                                m_eventData[i]->event()->getEndIndex()))
        {
            m_eventData[i]->setVisible(m_settings->m_showEventsInTimeline);
        }
        else
        {
            m_eventData[i]->setVisible(false);
        }
    }
}


void TimelineVisualization::updateSize()
{
    if (!m_settings->m_activateZoomToFixationArea)
    {
        return;
    }
    if (m_settings->m_currentFixationIndex > -1 && m_settings->m_showNeighboringFixations)
    {
        int minIndex = std::max(0, m_settings->m_currentFixationIndex -
                                m_settings->m_previousFixationsCount);
        int maxIndex = std::min(int(m_fixationData.size()) - 1,
                                m_settings->m_currentFixationIndex +
                                m_settings->m_followingFixationsCount);
        int start = m_settings->m_currentTrials[0]->gazeData().firstKey();
        int min = m_fixationData[size_t(minIndex)]->fixation()->getStartIndex() - start;
        int max = m_fixationData[size_t(maxIndex)]->fixation()->getEndIndex() - start;
        zoomToRange(min, max);
    }
    else if (!m_settings->m_showNeighboringFixations)
    {
        m_scene->views()[0]->fitInView(m_scene->sceneRect(), Qt::IgnoreAspectRatio);
    }
}


void TimelineVisualization::zoomToRange(int min, int max)
{
    m_scene->views()[0]->fitInView(QRect(QPoint(min, 0),
                                         QPoint(max, int(m_scene->sceneRect().height()))),
                                   Qt::IgnoreAspectRatio);
}


void TimelineVisualization::setScene(QGraphicsScene* scene)
{
    m_scene = scene;
}


int TimelineVisualization::itemAt(const QPointF& pos)
{
    for (int i = int(m_fixationData.size()) - 1; i >= 0; --i)
    {
        const FixationTimeItem* item = m_fixationData[size_t(i)];

        if (item->isVisible() && item->rect().contains(pos))
        {
            return i;
        }
    }
    return -1;
}
