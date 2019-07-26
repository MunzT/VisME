/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainScene.cpp
 **************************************************************************************************/
#include "mainscene.h"
#include <QGraphicsView>
#include <QPainter>


MainScene::MainScene(MainVisualization* mainVis, Settings* settings, QObject* parent) :
    QGraphicsScene(parent),
    m_mainVis(mainVis),
    m_settings(settings)
{
    //general settings
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setBackgroundBrush(Qt::white);
}


void MainScene::updateSceneRectSize()
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }
    Trial* trial = m_settings->m_currentTrials[0];
    setSceneRect(trial->minX(),
                 trial->minY(),
                 trial->maxX() - trial->minX(),
                 trial->maxY() - trial->minY());
}


void MainScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QGraphicsScene::drawForeground(painter, rect);

    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    Trial* trial = m_settings->m_currentTrials[0];

    switch(m_settings->m_displayMode)
    {
        case Multi:

            for (size_t i = 0; i < m_settings->m_currentTrials.size(); ++i)
            {
                std::vector<std::vector<std::vector<QPointF> > > microsaccadeGazeData =
                        getMicrosaccadesGazeData(m_settings->m_currentTrials[i]);

                drawMicrosaccadeDirections(microsaccadeGazeData, painter);
            }

            break;

        case Individual:

            if (trial->gazeData().empty())
            {
                return;
            }

            // highlight fixation data samples
            if (m_settings->m_highlightFixations)
            {
                painter->setPen(QPen(*(m_settings->m_colors.fixationSamplesColor), 0));

                const QMap<int, GazeData>& g = trial->gazeData();
                int fixStart = 0;
                int fixEnd = 0;
                if (m_settings->m_highlightCurrentFixation && m_settings->m_currentFixationIndex != -1)
                {
                    fixStart = m_settings->m_currentFixationIndex;
                    fixEnd = m_settings->m_currentFixationIndex;
                }
                else
                {
                    fixEnd = trial->fixationsCount() - 1;
                }

                if (fixEnd >= 0 && fixStart >= 0 &&
                        trial->fixationsCount() > fixEnd && trial->fixationsCount() > fixStart)
                {
                    for (int i = fixStart; i <= fixEnd; ++i)
                    {
                        int start = trial->getFixationAt(i)->getStartIndex() + 1;
                        int end = trial->getFixationAt(i)->getEndIndex();

                        if (trial->outsideTimeLimitIfActivated(start, end))
                        {
                            continue;
                        }

                        for (int j = start; j <= end; ++j)
                        {
                            if (g.contains(j) && g.contains(j - 1))
                            {
                                secureDrawLine(QPointF(g[j - 1].getXPosition(), g[j - 1].getYPosition()),
                                               QPointF(g[j].getXPosition(), g[j].getYPosition()),
                                               painter);
                            }

                            if (j == start)
                            {
                                if (g.contains(j - 1))
                                {
                                    painter->setBrush(Qt::black);
                                    painter->drawEllipse(QPointF(g[j - 1].getXPosition(), g[j - 1].getYPosition()),
                                            scaleToView(2.0), scaleToView(2.0));
                                }

                            }
                            else if (j == end)
                            {
                                if (g.contains(j))
                                {
                                    painter->setBrush(Qt::white);
                                    painter->drawEllipse(QPointF(g[j].getXPosition(), g[j].getYPosition()),
                                                         scaleToView(2.0), scaleToView(2.0));
                                }
                            }
                        }
                    }
                }
            }

            std::vector<std::vector<std::vector<QPointF> > > microsaccadeGazeData =
                    getMicrosaccadesGazeData(trial);

            if (m_settings->m_showMicrosaccades)
            {
                QColor microsaccadesTransparentColor = *(m_settings->m_colors.microsaccadesColor);
                microsaccadesTransparentColor.setAlpha(200);

                for (size_t f = 0; f < microsaccadeGazeData.size(); ++f)
                {
                    for (size_t i = 0; i < microsaccadeGazeData[f].size(); ++i)
                    {
                        for (size_t j = 1; j < microsaccadeGazeData[f][i].size(); ++j)
                        {
                            painter->setPen(QPen(microsaccadesTransparentColor, scaleToView(2.0)));
                            if (m_settings->m_highlightCurrentFixation &&
                                    m_settings->m_currentFixationIndex != -1 &&
                                    size_t(m_settings->m_currentFixationIndex) != f)
                            {
                                QColor lighterColor = painter->pen().color();
                                lighterColor.setAlpha(int(lighterColor.alpha() / 10.0));
                                painter->setPen(QPen(lighterColor, scaleToView(2.0)));
                            }

                            // line with same start and end point produced strange rectangle which was too large
                            secureDrawLine(QPointF(microsaccadeGazeData[f][i][j - 1].x(),
                                                   microsaccadeGazeData[f][i][j - 1].y()),
                                           QPointF(microsaccadeGazeData[f][i][j].x(),
                                                   microsaccadeGazeData[f][i][j].y()),
                                           painter);
                        }
                    }
                }

                if (m_settings->m_showSamples)
                {
                    for (size_t f = 0; f < microsaccadeGazeData.size(); ++f)
                    {
                        for (size_t i = 0; i < microsaccadeGazeData[f].size(); ++i)
                        {
                            for (size_t j = 0; j < microsaccadeGazeData[f][i].size(); ++j)
                            {
                                bool draw = false;
                                painter->setPen(QPen(*(m_settings->m_colors.microsaccadesColor), scaleToView(2.0)));
                                draw = true;
                                if (m_settings->m_highlightCurrentFixation &&
                                        m_settings->m_currentFixationIndex != -1 &&
                                        size_t(m_settings->m_currentFixationIndex) != f)
                                {
                                    QColor lighterColor = painter->pen().color();
                                    lighterColor.setAlpha(int(lighterColor.alpha() / 4.0));
                                    painter->setPen(QPen(lighterColor, scaleToView(2.0)));
                                }

                                if (draw)
                                {
                                    painter->drawPoint(QPointF(microsaccadeGazeData[f][i][j].x(),
                                                               microsaccadeGazeData[f][i][j].y()));
                                }
                            }
                        }
                    }
                }
            }

            if (m_settings->m_showMicrosaccadeDirections)
            {
                drawMicrosaccadeDirections(microsaccadeGazeData, painter);
            }

            break;
    }
}


void MainScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    QGraphicsScene::drawBackground(painter, rect);

    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    Trial* trial = m_settings->m_currentTrials[0];

    if (trial->getStimulus() != QString())
    {
        QImage backgroundImage;
        backgroundImage.load(trial->getStimulus());
        painter->setOpacity(m_settings->m_backgroundOpacity / 100.0);
        painter->drawImage(int((trial->maxX() - trial->minX() -
                           backgroundImage.width()) / 2.0),
                           int((trial->maxY() - trial->minY() -
                           backgroundImage.height()) / 2.0),
                           backgroundImage);

        painter->setOpacity(1.0);
    }

    painter->setPen(QPen(*(m_settings->m_colors.sceneBorderColor), 0));
    painter->drawRect(QRectF(trial->minX(),
                             trial->minY(),
                             trial->maxX() - trial->minX(),
                             trial->maxY() - trial->minY()));

    switch(m_settings->m_displayMode)
    {
        case Multi:
            break;
        case Individual:

            if (trial->gazeData().empty())
            {
                return;
            }

            if (m_settings->m_showSamples)
            {
                QMapIterator<int, GazeData> it(trial->gazeData());
                while (it.hasNext())
                {
                    it.next();
                    if (trial->outsideTimeLimitIfActivated(it.key(), it.key()))
                    {
                        continue;
                    }
                    painter->setPen(QPen(*(m_settings->m_colors.sampleColor), scaleToView(2.0)));

                    painter->drawPoint(QPointF(it.value().getXPosition(), it.value().getYPosition()));
                }
            }

            if (m_settings->m_showSampleConnections)
            {
                // Raw data left eye
                QMapIterator<int, GazeData> it(trial->gazeData());
                it.next();
                QMapIterator<int, GazeData> prev(trial->gazeData());
                while (it.hasNext())
                {
                    prev = it;

                    if (trial->outsideTimeLimitIfActivated(it.key(), it.key()))
                    {
                        it.next();
                        continue;
                    }

                    it.next();

                    if (it.key() - prev.key() == 1)
                    {
                        painter->setPen(QPen(*(m_settings->m_colors.sampleConnectionColor), 0));
                    }
                    else
                    {
                        painter->setPen(QPen(*(m_settings->m_colors.missingDataColor),
                                             scaleToView(2.0), Qt::DotLine));
                    }
                    secureDrawLine(QPointF(it.value().getXPosition(), it.value().getYPosition()),
                                   QPointF(prev.value().getXPosition(), prev.value().getYPosition()),
                                   painter);
                }
            }

            if (m_settings->m_showScanpath || m_settings->m_showSaccadeDirections)
            {
                if (trial->fixationsCount() == 0)
                {
                    return;
                }

                for (int i = 0; i < trial->fixationsCount(); ++i)
                {

                    if (trial->outsideTimeLimitIfActivated(trial->getFixationAt(i)->getStartIndex(),
                                                           trial->getFixationAt(i)->getEndIndex()))
                    {
                        continue;
                    }

                    if (!m_settings->m_showNeighboringFixations ||
                            (m_settings->m_currentFixationIndex > -1 &&
                             i <= m_settings->m_currentFixationIndex + m_settings->m_followingFixationsCount &&
                             i >= m_settings->m_currentFixationIndex - m_settings->m_previousFixationsCount) ||
                           (m_settings->m_showSaccadeDirections && i > 0))
                    {
                        if (m_settings->m_showSaccadeDirections)
                        {
                            if (i == 0)
                            {
                                continue;
                            }

                            // line indicating next fixation
                            QLinearGradient linearGrad(m_mainVis->getFixationDataAt(i - 1)->center(),
                                                       m_mainVis->getFixationDataAt(i)->center());
                            linearGrad.setColorAt(0, * (m_settings->m_colors.saccadesDirectionStartColor));
                            linearGrad.setColorAt(1, * (m_settings->m_colors.saccadeColor));
                            painter->setPen(QPen(linearGrad, 0));

                            secureDrawLine(m_mainVis->getFixationDataAt(i - 1)->center(),
                                           m_mainVis->getFixationDataAt(i)->center(),
                                           painter);
                        }
                        else if (i < trial->fixationsCount())
                        {
                            if (m_settings->m_currentFixationIndex != -1 &&
                                    i == m_settings->m_currentFixationIndex + 1)
                            {
                                // line indicating next fixation
                                QLinearGradient linearGrad((m_mainVis->getFixationDataAt(i - 1)->center() +
                                                            m_mainVis->getFixationDataAt(i)->center()) / 2,
                                                            m_mainVis->getFixationDataAt(i)->center());
                                linearGrad.setColorAt(0, * (m_settings->m_colors.saccadesDirectionStartColor));
                                linearGrad.setColorAt(1, * (m_settings->m_colors.saccadeColor));
                                painter->setPen(QPen(linearGrad, 0));
                            }
                            else if (m_settings->m_currentFixationIndex != -1 &&
                                     i == m_settings->m_currentFixationIndex - 1)
                             {
                                 // line indicating previous fixation (just used when highlighting
                                 QLinearGradient linearGrad((m_mainVis->getFixationDataAt(i + 1)->center() +
                                                             m_mainVis->getFixationDataAt(i)->center()) / 2,
                                                             m_mainVis->getFixationDataAt(i + 1)->center());
                                 linearGrad.setColorAt(0, * (m_settings->m_colors.saccadeColor));
                                 linearGrad.setColorAt(1, * (m_settings->m_colors.saccadesDirectionStartColor));
                                 painter->setPen(QPen(linearGrad, 0));
                             }
                            else
                            {
                                painter->setPen(QPen(*(m_settings->m_colors.saccadeColor), 0));
                            }

                            if ((m_settings->m_currentFixationIndex == -1 ||
                                 i < m_settings->m_currentFixationIndex) &&
                                 i < trial->fixationsCount() - 1)
                            {
                                secureDrawLine(m_mainVis->getFixationDataAt(i + 1)->center(),
                                               m_mainVis->getFixationDataAt(i)->center(),
                                               painter);
                            }
                            else if (i > m_settings->m_currentFixationIndex && i > 0) // there have to be at least two fixations
                            {
                                secureDrawLine(m_mainVis->getFixationDataAt(i - 1)->center(),
                                               m_mainVis->getFixationDataAt(i)->center(),
                                               painter);
                            }
                        }
                    }
                }
            }
            break;
    }
}


std::vector<std::vector<std::vector<QPointF> > > MainScene::getMicrosaccadesGazeData(Trial* trial)
{
    std::vector<std::vector<std::vector<QPointF> > > microsaccadeGazeData;
    for (int i = 0; i < trial->fixationsCount(); ++i)
    {
        std::vector<std::vector<QPointF> > microsaccadeGazeDataInFixation;
        if (!trial->outsideTimeLimitIfActivated(trial->getFixationAt(i)->getStartIndex(),
                                                trial->getFixationAt(i)->getEndIndex()))
        {
            for (size_t k = 0; k < trial->getFixationAt(i)->getMicrosaccadesCount(trial->msFromInputFile()); ++k)
            {
                std::vector<QPointF> microsaccade;
                for (int j = int(trial->getMicrosaccadeAt(i, k)->getOnsetIndex());
                     j <= int(trial->getMicrosaccadeAt(i, k)->getEndIndex()); ++j)
                {
                    QPointF p = QPointF(trial->gazeData().value(j).getXPosition(),
                                        trial->gazeData().value(j).getYPosition());
                    microsaccade.push_back(p);
                }
                microsaccadeGazeDataInFixation.push_back(microsaccade);
            }
        }
        microsaccadeGazeData.push_back(microsaccadeGazeDataInFixation);
    }
    return microsaccadeGazeData;
}


void MainScene::drawMicrosaccadeDirections(std::vector<std::vector<std::vector<QPointF> > > microsaccadeGazeData,
                                           QPainter* painter)
{
    for (size_t f = 0; f < microsaccadeGazeData.size(); ++f)
    {
        if (m_settings->m_displayMode == Multi ||
                !m_settings->m_highlightCurrentFixation || m_settings->m_currentFixationIndex == -1 ||
                (m_settings->m_highlightCurrentFixation &&
                size_t(m_settings->m_currentFixationIndex) == f))
        {
            for (size_t i = 0; i < microsaccadeGazeData[f].size(); ++i)
            {
                size_t startIndex = 0;
                size_t endIndex = microsaccadeGazeData[f][i].size() - 1;

                QLinearGradient linearGrad(QPointF(microsaccadeGazeData[f][i][startIndex].x(),
                                                   microsaccadeGazeData[f][i][startIndex].y()),
                                           QPointF(microsaccadeGazeData[f][i][endIndex].x(),
                                                   microsaccadeGazeData[f][i][endIndex].y()));
                linearGrad.setColorAt(0, *(m_settings->m_colors.microsaccadesColor));
                linearGrad.setColorAt(1, *(m_settings->m_colors.microsaccadesDirectionEndColor));
                painter->setPen(QPen(linearGrad, scaleToView(3.0)));

                secureDrawLine(QPointF(microsaccadeGazeData[f][i][startIndex].x(),
                                       microsaccadeGazeData[f][i][startIndex].y()),
                               QPointF(microsaccadeGazeData[f][i][endIndex].x(),
                                       microsaccadeGazeData[f][i][endIndex].y()),
                               painter);
            }
        }
    }
}


void MainScene::secureDrawLine(QPointF p1, QPointF p2, QPainter* painter)
{
    if (p1 != p2)
    {
        painter->drawLine(p1, p2);
    }
}


qreal MainScene::scaleToView(qreal value)
{
    return 1.0 / views()[0]->transform().m11() * value;
}


void MainScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
    if(event->isAccepted())
    {
        return;
    }

    emit fixationSelected(nullptr);

}
