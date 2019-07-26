/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixation.cpp
 **************************************************************************************************/
#include "fixation.h"


Fixation::Fixation(Settings* settings) :
    m_settings(settings), m_xPosition(0), m_yPosition(0),
    m_startIndex(0), m_duration(0), m_marked(false)
{}


qreal Fixation::getXPosition() const
{
    return m_xPosition;
}


void Fixation::setXPosition(qreal xPosition)
{
    m_xPosition = xPosition;
}


qreal Fixation::getYPosition() const
{
    return m_yPosition;
}


void Fixation::setYPosition(qreal yPosition)
{
    m_yPosition = yPosition;
}


int Fixation::getStartIndex() const
{
    return m_startIndex;
}


void Fixation::setStartIndex(int value)
{
    m_startIndex = value;
}


int Fixation::getEndIndex() const
{
    return m_startIndex + m_duration - 1;
}


int Fixation::getDuration() const
{
    return m_duration;
}


void Fixation::setDuration(int value)
{
    m_duration = value;
}


std::vector<Saccade>* Fixation::getMicrosaccades(bool fromInput)
{
    if (fromInput)
    {
        return &m_inputMicrosaccades;
    }
    return &m_calculatedMicrosaccades;
}


void Fixation::removeMicrosaccades(std::vector<int> microsaccadeIndices, bool fromInput)
{
    if (fromInput)
    {
        for (int i = int(microsaccadeIndices.size()) - 1; i >= 0 ; --i)
        {
            m_inputMicrosaccades.erase(m_inputMicrosaccades.begin() + microsaccadeIndices[size_t(i)]);
        }
    }
    else
    {
        for (int i = int(microsaccadeIndices.size()) - 1; i >= 0 ; --i)
        {
            m_calculatedMicrosaccades.erase(m_calculatedMicrosaccades.begin() + microsaccadeIndices[size_t(i)]);
        }
    }
}


void Fixation::setMicrosaccades(const std::vector<Saccade>& microsaccades, bool fromInput)
{
    if (fromInput)
    {
        m_inputMicrosaccades = microsaccades;
    }
    else
    {
        m_calculatedMicrosaccades = microsaccades;
    }
}


void Fixation::addMicrosaccade(Saccade m, bool fromInput)
{
    if (fromInput)
    {
        m_inputMicrosaccades.push_back(m);
    }
    else
    {
        m_calculatedMicrosaccades.push_back(m);
    }
}


void Fixation::clearMicrosaccades(bool fromInput)
{
    if (fromInput)
    {
        m_inputMicrosaccades.clear();
    }
    else
    {
        m_calculatedMicrosaccades.clear();
    }
}


Saccade* Fixation::getMicrosaccadeAt(const size_t index, bool fromInput)
{
    return  &((*getMicrosaccades(fromInput))[index]);
}


size_t Fixation::getMicrosaccadesCount(bool fromInput)
{
    std::vector<Saccade>* ms = getMicrosaccades(fromInput);
    return ms == nullptr ? 0 : ms->size();
}


bool Fixation::isMarked() const
{
    return m_marked;
}


void Fixation::setMarked(bool mark)
{
    m_marked = mark;
}
