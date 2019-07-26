/***********************************************************************************************//**
 * @author Tanja Munz
 * @file timelinescene.cpp
 **************************************************************************************************/
#include "timelinescene.h"
#include <cmath>
#include <QGraphicsView>


TimelineScene::TimelineScene(Settings* settings, QObject* parent) :
    QGraphicsScene(parent),
    m_settings(settings),
    m_maxHeight(0)
{
    //general settings
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setBackgroundBrush(Qt::white);
}


void TimelineScene::updateSceneRectSize()
{
    if (m_settings->m_currentTrials.empty() ||
        m_settings->m_currentTrials[0]->gazeData().empty())
    {
        return;
    }

    setSceneRect(0, -m_maxHeight,
                 m_settings->m_currentTrials[0]->gazeData().lastKey() -
                 m_settings->m_currentTrials[0]->gazeData().firstKey(),
                 m_maxHeight);

}


void TimelineScene::setMaxHeight(const qreal& maxHeight)
{
    m_maxHeight = maxHeight;
}


void TimelineScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QGraphicsScene::drawBackground(painter, rect);

    if (m_settings->m_displayMode == DisplayMode::Multi)
    {
        return;
    }

    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    Trial* trial = m_settings->m_currentTrials[0];
    const QMap<int, GazeData>& gazeData = trial->gazeData();

    if (trial->gazeDataCount() >= 2)
    {
        painter->setPen(QPen(*(m_settings->m_colors.sceneBorderColor), 0));
        painter->drawLine(QPointF(0, -0),
                         QPointF(gazeData.lastKey() - gazeData.firstKey(), 0));
        painter->drawLine(QPointF(0, -m_maxHeight),
                         QPointF(gazeData.lastKey() - gazeData.firstKey(), -m_maxHeight));

        // draw gaze data
        const GazeData* previousGazeData = nullptr;
        int previousGazeDataIndex = -1;

        QMapIterator<int, GazeData> i(gazeData);
        qreal max = 0;
        qreal min = 1000;
        std::vector<qreal> values;

        std::vector<std::vector<QPointF>> velocities;
        std::vector<QPointF> velocitySection;

        // velocities
        while (i.hasNext())
        {
            i.next();

            const GazeData* currentGazeData = &i.value();
            int currentGazeDataIndex = i.key() - gazeData.firstKey();

            if (!trial->outsideTimeLimitIfActivated(i.key(), i.key()))
            {
                if (previousGazeData != nullptr && currentGazeData != nullptr && currentGazeDataIndex - previousGazeDataIndex == 1)
                {

                    if (m_settings->m_showVelocityValuesInTimeline)
                    {
                        // velocity
                        int diff = currentGazeDataIndex - previousGazeDataIndex;

                        qreal velX = (currentGazeData->getXPosition() - previousGazeData->getXPosition());
                        qreal velY = (currentGazeData->getYPosition() - previousGazeData->getYPosition());

                        // plot velocity value in the middle between two data points
                        qreal height = -sqrt(velX * velX + velY * velY) / diff;
                        velocitySection.push_back(QPointF(currentGazeDataIndex - 0.5, height));
                        max = std::max(max, fabs(height));
                        min = std::min(min, fabs(height));
                        values.push_back(fabs(height));
                    }
                }
                else
                {
                    velocities.push_back(velocitySection);
                    velocitySection.clear();
                }
                previousGazeData = currentGazeData;
                previousGazeDataIndex = currentGazeDataIndex;
            }
        }
        velocities.push_back(velocitySection);

        // plot velocities
        if (!velocities.empty() && m_settings->m_showVelocityValuesInTimeline)
        {
            painter->setPen(QPen(*(m_settings->m_colors.velocityColor), 0));
            for (size_t i = 0; i < velocities.size(); ++i)
            {
                if (!velocities[i].empty())
                {
                    for (size_t j = 0; j < velocities[i].size() - 1; ++j)
                    {
                        painter->drawLine(QPointF(velocities[i][j].x(), velocities[i][j].y() / max * m_maxHeight),
                                          QPointF(velocities[i][j + 1].x(), velocities[i][j + 1].y() / max * m_maxHeight));
                        painter->drawEllipse(QPointF(velocities[i][j].x(), velocities[i][j].y() / max * m_maxHeight),
                                             1.0 / views()[0]->transform().m11() * 0.5,
                                             1.0 / views()[0]->transform().m22() * 0.5);
                    }
                    painter->drawEllipse(QPointF(velocities[i].back().x(), velocities[i].back().y() / max * m_maxHeight),
                                         1.0 / views()[0]->transform().m11() * 0.5,
                                         1.0 / views()[0]->transform().m22() * 0.5);
                }
            }
        }

        // x and y
        i = QMapIterator<int, GazeData>(gazeData);
        previousGazeData = nullptr;
        previousGazeDataIndex = -1;
        while (i.hasNext())
        {
            i.next();
            if (!trial->outsideTimeLimitIfActivated(i.key(), i.key()))
            {
                const GazeData* currentGazeData = &i.value();
                int currentGazeDataIndex = i.key() - gazeData.firstKey();

                if (previousGazeData != nullptr)
                {
                    qreal yHeight = trial->maxY() - trial->minY();

                    if (m_settings->m_showXValuesInTimeline)
                    {
                        if (currentGazeDataIndex - previousGazeDataIndex == 1)
                        {
                            painter->setPen(QPen(*(m_settings->m_colors.xValueColor), 0));
                        }
                        else
                        {
                            painter->setPen(QPen(*(m_settings->m_colors.missingDataColor), 0, Qt::DotLine));
                        }

                        painter->drawLine(QPointF(previousGazeDataIndex, -previousGazeData->getXPosition()),
                                QPointF(currentGazeDataIndex, -currentGazeData->getXPosition()));


                        painter->setPen(QPen(*(m_settings->m_colors.xValueColor), 0));
                        painter->drawEllipse(QPointF(previousGazeDataIndex, -previousGazeData->getXPosition()),
                                             1.0 / views()[0]->transform().m11() * 0.5,
                                             1.0 / views()[0]->transform().m22() * 0.5);
                    }
                    if (m_settings->m_showYValuesInTimeline)
                    {
                        if (currentGazeDataIndex - previousGazeDataIndex == 1)
                        {
                            painter->setPen(QPen(*(m_settings->m_colors.yValueColor), 0));
                        }
                        else
                        {
                            painter->setPen(QPen(*(m_settings->m_colors.missingDataColor), 0, Qt::DotLine));
                        }
                        painter->drawLine(QPointF(previousGazeDataIndex, -(yHeight - previousGazeData->getYPosition())),
                            QPointF(currentGazeDataIndex, -(yHeight - currentGazeData->getYPosition())));

                        painter->setPen(QPen(*(m_settings->m_colors.yValueColor), 0));
                        painter->setBrush(*(m_settings->m_colors.yValueColor));
                        painter->drawEllipse(QPointF(previousGazeDataIndex, -(yHeight - previousGazeData->getYPosition())),
                                             1.0 / views()[0]->transform().m11() * 0.5,
                                             1.0 / views()[0]->transform().m22() * 0.5);
                    }
                }
                previousGazeData = currentGazeData;
                previousGazeDataIndex = currentGazeDataIndex;
            }
        }
    }
}


bool TimelineScene::isInsideCurrentFixation(int index)
{
    if (!m_settings->m_currentTrials.empty())
    {
        Trial* trial = m_settings->m_currentTrials[0];
        int id = index + trial->gazeData().firstKey();
        if (id >= trial->getFixationAt(m_settings->m_currentFixationIndex)->getStartIndex() &&
                id <= trial->getFixationAt(m_settings->m_currentFixationIndex)->getEndIndex())
        {
            return true;
        }
    }
    return false;
}



bool TimelineScene::isInsideFixation(int index)
{
    if (!m_settings->m_currentTrials.empty())
    {
        Trial* trial = m_settings->m_currentTrials[0];
        int id = index + trial->gazeData().firstKey();
        for (int i = 0; i < trial->fixationsCount(); ++i)
        {
            if (id >= trial->getFixationAt(i)->getStartIndex() &&
                    id <= trial->getFixationAt(i)->getStartIndex() +
                    trial->getFixationAt(i)->getDuration() - 1)
            {
                return true;
            }
        }
    }
    return false;
}
