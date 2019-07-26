/***********************************************************************************************//**
 * @author Tanja Munz
 * @file trial.cpp
 **************************************************************************************************/
#include "settings.h"
#include "trial.h"
#include <algorithm>
#include <iostream>
#include <cmath>


Trial::Trial(Participant* p, Settings* settings) :
    m_name(""),
    m_participant(p),
    m_settings(settings),
    m_useFixationsFromInput(true),
    m_microsaccadeSettings(nullptr),
    m_fixationSettings(nullptr)
{}


Trial::~Trial()
{
    if (m_microsaccadeSettings != nullptr)
    {
        delete m_microsaccadeSettings;
    }
    if (m_fixationSettings != nullptr)
    {
        delete m_fixationSettings;
    }
}


qreal Trial::frequencyRate() const
{
    return m_frequencyRate;
}


void Trial::setFrequencyRate(qreal frequencyRate)
{
    m_frequencyRate = frequencyRate;
}


QString Trial::name() const
{
    return m_name;
}


QString Trial::modifiedName() const
{
    QString newName = m_name;
    newName.replace(".", "_");
    return newName;
}


void Trial::setName(const QString& name)
{
    m_name = name;
}


Participant* Trial::getParticipant() const
{
    return m_participant;
}


QString Trial::getStimulus() const
{
    return m_stimulus;
}


void Trial::setStimulus(const QString& stimulus)
{
    m_stimulus = stimulus;
}


void Trial::setMinMax(qreal minX, qreal maxX, qreal minY, qreal maxY)
{
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
}


qreal Trial::minX() const
{
    return m_minX;
}


qreal Trial::minY() const
{
    return m_minY;
}


qreal Trial::maxX() const
{
    return m_maxX;
}


qreal Trial::maxY() const
{
    return m_maxY;
}


void Trial::setGazeData(const QMap<int, GazeData> gazeData, GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        m_gazeDataRight = gazeData;
    }
    else if (gazeType == GazeType::GT_Left)
    {
        m_gazeDataLeft = gazeData;
    }
    else if (gazeType == GazeType::GT_Average)
    {
        m_gazeDataAverage = gazeData;
    }
}


size_t Trial::gazeDataCount(GazeType gazeType) const
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        return size_t(m_gazeDataRight.size());
    }
    else if (gazeType == GazeType::GT_Left)
    {
        return size_t(m_gazeDataLeft.size());
    }
    else if (gazeType == GazeType::GT_Average)
    {
        return size_t(m_gazeDataAverage.size());
    }
    return 0;
}


QMap<int, GazeData> Trial::gazeData(GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        return m_gazeDataRight;
    }
    else if (gazeType == GazeType::GT_Left)
    {
        return m_gazeDataLeft;
    }
    else if (gazeType == GazeType::GT_Average)
    {
        return m_gazeDataAverage;
    }
    return QMap<int, GazeData>();
}


std::vector<Fixation>* Trial::fixations(GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        if (m_useFixationsFromInput)
        {
            return &m_inputFixationsRight;
        }
        else
        {
            return &m_calculatedFixationsRight;
        }
    }
    else if (gazeType == GazeType::GT_Left)
    {
        if (m_useFixationsFromInput)
        {
            return &m_inputFixationsLeft;
        }
        else
        {
            return &m_calculatedFixationsLeft;
        }
    }
    else if (gazeType == GazeType::GT_Average)
    {
        if (m_useFixationsFromInput)
        {
            return &m_inputFixationsAverage;
        }
        else
        {
            return &m_calculatedFixationsAverage;
        }
    }
    return nullptr;
}


std::vector<Fixation*> Trial::fixationPointers(GazeType gazeType)
{
    std::vector<Fixation>* fix = fixations(gazeType);
    std::vector<Fixation*> out;

    for (size_t i = 0; i < fix->size(); ++i)
    {
        out.push_back(&(*fix)[i]);
    }

    return out;
}


void Trial::setFixations(const std::vector<Fixation>& fixations, GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        if (m_useFixationsFromInput)
        {
            m_inputFixationsRight = fixations;
        }
        else
        {
            m_calculatedFixationsRight = fixations;
        }
    }
    else if (gazeType == GazeType::GT_Left)
    {
        if (m_useFixationsFromInput)
        {
            m_inputFixationsLeft = fixations;
        }
        else
        {
            m_calculatedFixationsLeft = fixations;
        }
    }
    else if (gazeType == GazeType::GT_Average)
    {
        if (m_useFixationsFromInput)
        {
            m_inputFixationsAverage = fixations;
        }
        else
        {
            m_calculatedFixationsAverage = fixations;
        }
    }
}


void Trial::setSaccades(const std::vector<Saccade>& saccades, GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        m_calculatedSaccadesRight = saccades;
    }
    else if (gazeType == GazeType::GT_Left)
    {
        m_calculatedSaccadesLeft = saccades;
    }
    else if (gazeType == GazeType::GT_Average)
    {
        m_calculatedSaccadesAverage = saccades;
    }
}


int Trial::fixationsCount(GazeType gazeType)
{
    if (fixations(gazeType) != nullptr)
    {
        return int(fixations(gazeType)->size());
    }
    return 0;
}


Fixation* Trial::getFixationAt(int index, GazeType gazeType)
{
    if (index >= 0 && fixationsCount(gazeType) > index)
    {
        return &((*fixations(gazeType))[size_t(index)]);
    }
    return nullptr;
}


Fixation* Trial::getNextFixation(Fixation* fixation, GazeType gazeType)
{
    int i = getFixationIndex(fixation, gazeType);
    if (i >= 0 && fixationsCount(gazeType) > i + 1)
    {
        return getFixationAt(i + 1, gazeType);
    }
    else
    {
        return nullptr;
    }
}


Fixation* Trial::getPrevFixation(Fixation* fixation, GazeType gazeType)
{
    int i = getFixationIndex(fixation, gazeType);
    if (i >= 1)
    {
        return getFixationAt(i - 1, gazeType);
    }
    else
    {
        return nullptr;
    }
}


int Trial::getFixationIndex(Fixation* f, GazeType gazeType)
{
    for (int i = 0; i < fixationsCount(gazeType); ++i)
    {
        if (getFixationAt(i, gazeType) == f)
        {
            return i;
        }
    }
    return -1;
}


size_t Trial::saccadesCount(GazeType gazeType)
{
    return saccades(gazeType)->size();
}


Saccade* Trial::getSaccadeAt(size_t index, GazeType gazeType)
{
    return &((*saccades(gazeType))[index]);
}


std::vector<Saccade>* Trial::saccades(GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    if (gazeType == GazeType::GT_Right)
    {
        return &m_calculatedSaccadesRight;
    }
    else if (gazeType == GazeType::GT_Left)
    {
        return &m_calculatedSaccadesLeft;
    }
    else if (gazeType == GazeType::GT_Average)
    {
        return &m_calculatedSaccadesAverage;
    }
    return nullptr;
}


bool Trial::useFixationsFromInput() const
{
    return m_useFixationsFromInput;
}


void Trial::setUseFixationsFromInput(bool useFixationsFromInput)
{
    m_useFixationsFromInput = useFixationsFromInput;
}


void Trial::setMicrosaccadesForFixations(std::vector<Saccade>* microsaccades, GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }

    std::vector<Fixation>* currentFixations = fixations(gazeType);

    for (int i = 0; i < fixationsCount(gazeType); ++i)
    {
        (*currentFixations)[size_t(i)].clearMicrosaccades(msFromInputFile());
    }

    // remove microsaccades outside fixation areas or within the beginning/end of fixations
    int ignoreAtStart = m_settings->m_microsaccadeSettings->m_useIgnoreAtStart ?
                            timeToSamples(m_settings->m_microsaccadeSettings->m_ignoreAtStart) : 0;
    int ignoreAtEnd = m_settings->m_microsaccadeSettings->m_useIgnoreAtEnd ?
                            timeToSamples(m_settings->m_microsaccadeSettings->m_ignoreAtEnd) : 0;

    int fixationIndex = 0;
    // assumes that microsaccades are temporally ordered
    for (size_t i = 0; i < microsaccades->size(); ++i)
    {
        bool fixationFound = false;
        while(!fixationFound)
        {
            if (fixationsCount(gazeType) <= fixationIndex ||
                    getFixationAt(fixationIndex, gazeType)->getStartIndex() > int((*microsaccades)[i].getOnsetIndex()))
            {
                break;
            }

            if (getFixationAt(fixationIndex, gazeType)->getStartIndex() <=
                    int((*microsaccades)[i].getOnsetIndex()) &&
                getFixationAt(fixationIndex, gazeType)->getEndIndex() >=
                    int((*microsaccades)[i].getEndIndex()))
            {

                // just in boundaries
                if (getFixationAt(fixationIndex, gazeType)->getStartIndex() + ignoreAtStart <=
                        int((*microsaccades)[i].getOnsetIndex()) &&
                    getFixationAt(fixationIndex, gazeType)->getEndIndex() - ignoreAtEnd >=
                        int((*microsaccades)[i].getEndIndex()))
                {
                    (*currentFixations)[size_t(fixationIndex)].addMicrosaccade((*microsaccades)[i], msFromInputFile());
                }
                fixationFound = true;
            }
            else
            {
                fixationIndex++;
            }
        }
    }
}


Saccade* Trial::getMicrosaccadeAt(int fixationIndex, size_t microsaccadeIndex, GazeType gazeType)
{
    return getFixationAt(fixationIndex, gazeType)->
            getMicrosaccadeAt(microsaccadeIndex, msFromInputFile());
}


size_t Trial::microsaccadesCount(GazeType gazeType)
{
    size_t count = 0;
    for (int i = 0; i < fixationsCount(gazeType); ++i)
    {
        count += getFixationAt(i, gazeType)->getMicrosaccadesCount(msFromInputFile());
    }

    return count;
}


void Trial::updateMicrosaccadesSettings()
{
    if (m_microsaccadeSettings != nullptr)
    {
        delete m_microsaccadeSettings;
    }

    m_microsaccadeSettings = new SaccadeFilterSettings(*m_settings->m_microsaccadeSettings);
}


void Trial::updateFixationSettings()
{
    if (m_fixationSettings != nullptr)
    {
        delete m_fixationSettings;
    }

    m_fixationSettings = new SaccadeFilterSettings(*m_settings->m_fixationSettings);
}


bool Trial::msFromInputFile()
{
    return m_microsaccadeSettings == nullptr || m_microsaccadeSettings->m_fromInputFile;
}


bool Trial::fixationsFromInputFile()
{
    return m_fixationSettings == nullptr || m_fixationSettings->m_fromInputFile;
}


SaccadeFilterSettings* Trial::getMsFilterSettings() const
{
    return m_microsaccadeSettings;
}


SaccadeFilterSettings* Trial::getFixationFilterSettings() const
{
    return m_fixationSettings;
}


std::set<int> Trial::getMicrosaccadeIndices(GazeType gazeType)
{
    std::set<int> indices;
    for (int i = 0; i < fixationsCount(); ++i)
    {
        for (size_t k = 0; k < getFixationAt(i, gazeType)->getMicrosaccadesCount(msFromInputFile()); ++k)
        {
            for (auto j = getMicrosaccadeAt(i, k, gazeType)->getOnsetIndex();
                 j <= getMicrosaccadeAt(i, k, gazeType)->getEndIndex(); ++j)
            {
                indices.insert(int(j));
            }
        }
    }
    return indices;
}


void Trial::printAllMicrosaccades(GazeType gazeType)
{
    for (int i = 0; i < fixationsCount(gazeType); ++i)
    {
        Fixation* f = getFixationAt(i, gazeType);

        for (size_t j = 0; j < f->getMicrosaccadesCount(msFromInputFile()); ++j)
        {
            f->getMicrosaccadeAt(j, msFromInputFile())->print();
        }
    }
}


QString Trial::getTestConditionType() const
{
    return m_testConditionType;
}


void Trial::setTestConditionType(const QString& testConditionType)
{
    m_testConditionType = testConditionType;
}


void Trial::setEvents(std::vector<Event> events)
{
    m_events = events;
}


Event* Trial::getEventAt(size_t index)
{
    return &(m_events[index]);
}


size_t Trial::eventCount()
{
    return m_events.size();
}


int Trial::duration(GazeType gazeType)
{
    int maximum = 0;
    if (gazeDataCount(gazeType) > 0)
    {
        maximum = std::max(maximum, lastIndex(gazeType));
    }
    int minimum = maximum;
    if (gazeDataCount(gazeType) > 0)
    {
        minimum = std::min(minimum, firstIndex(gazeType));
    }

    return maximum - minimum + 1;
}


int Trial::firstIndex(GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }
    const QMap<int, GazeData>& data = gazeData(gazeType);
    if (!data.isEmpty())
    {
        return gazeData(gazeType).firstKey();
    }
    return 0;
}


int Trial::lastIndex(GazeType gazeType)
{
    if (gazeType == GazeType::GT_Unset)
    {
        gazeType = m_settings->m_gazeType;
    }
    const QMap<int, GazeData>& data = gazeData(gazeType);
    if (!data.isEmpty())
    {
        return gazeData(gazeType).lastKey();
    }
    return 0;
}


qreal Trial::samplesToTime(int samples)
{
    return samples * 1000.0 / frequencyRate();
}


size_t Trial::timeToSamples(qreal time)
{
    return size_t(ceil(time / 1000.0 * frequencyRate())); //round to upper
}


int Trial::timeToSampleInTrial(qreal time)
{
    return int(timeToSamples(time)) + firstIndex(m_settings->m_gazeType); //round to upper
}


bool Trial::outsideTimeLimitIfActivated(int start, int end)
{
    return (m_settings->m_limitTimeRange &&
            (start < timeToSampleInTrial(m_settings->m_startTimeInMSec) ||
             end > timeToSampleInTrial(m_settings->m_endTimeInMSec)));
}
