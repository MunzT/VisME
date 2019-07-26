/***********************************************************************************************//**
 * @author Tanja Munz
 * @file event.cpp
 **************************************************************************************************/
#include "event.h"


Event::Event(int start, int duration, QString name) :
    m_start(start), m_duration(duration), m_name(name)
{}


int Event::getStartIndex()
{
    return m_start;
}


int Event::getEndIndex()
{
    return m_start + m_duration - 1;
}


QString Event::getName()
{
    return m_name;
}

int Event::getDuration() const
{
    return m_duration;
}
