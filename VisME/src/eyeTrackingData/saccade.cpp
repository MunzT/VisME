/***********************************************************************************************//**
 * @author Tanja Munz
 * @file saccade.cpp
 **************************************************************************************************/
#include "saccade.h"
#include <iostream>
#include <cmath>


Saccade::Saccade(size_t onset, size_t end, qreal peakVelocity, qreal horizontalComponent,
                 qreal verticalComponent, qreal horizontalAmplitude, qreal verticalAmplitude) :
    m_onset(onset),
    m_end(end),
    m_peakVelocity(peakVelocity),
    m_horizontalComponent(horizontalComponent),
    m_verticalComponent(verticalComponent),
    m_horizontalAmplitude(horizontalAmplitude),
    m_verticalAmplitude(verticalAmplitude),
    m_isValid(true)
{}


size_t Saccade::getOnsetIndex() const
{
    return m_onset;
}


size_t Saccade::getEndIndex() const
{
    return m_end;
}


size_t Saccade::getDuration() const
{
    return m_end - m_onset + 1;
}


qreal Saccade::getPeakVelocity() const
{
    return m_peakVelocity;
}


qreal Saccade::getHorizontalComponent() const
{
    return m_horizontalComponent;
}


qreal Saccade::getVerticalComponent() const
{
    return m_verticalComponent;
}


qreal Saccade::getHorizontalAmplitude() const
{
    return m_horizontalAmplitude;
}


qreal Saccade::getVerticalAmplitude() const
{
    return m_verticalAmplitude;
}


qreal Saccade::getAmplitude() const
{
    return sqrt(pow(getHorizontalAmplitude(), 2) + pow(getVerticalAmplitude(), 2));
}


bool Saccade::isValid() const
{
    return m_isValid;
}


void Saccade::setOnsetIndex(size_t onset)
{
    m_onset = onset;
}


void Saccade::setEndIndex(size_t end)
{
    m_end = end;
}


void Saccade::setPeakVelocity(qreal peakVelocity)
{
    m_peakVelocity = peakVelocity;
}


void Saccade::setHorizontalComponent(qreal horizontalComponent)
{
    m_horizontalComponent = horizontalComponent;
}


void Saccade::setVerticalComponent(qreal verticalComponent)
{
    m_verticalComponent = verticalComponent;
}


void Saccade::setHorizontalAmplitude(qreal horizontalAmplitude)
{
    m_horizontalAmplitude = horizontalAmplitude;
}


void Saccade::setVerticalAmplitude(qreal verticalAmplitude)
{
    m_verticalAmplitude = verticalAmplitude;
}


void Saccade::setValid(bool isValid)
{
    m_isValid = isValid;
}


void Saccade::print()
{
    std::cout << "---------------------" << std::endl;
    std::cout << std::fixed;
    std::cout << m_onset << std::endl;
    std::cout << m_end << std::endl;
    std::cout << m_peakVelocity << std::endl;
    std::cout << m_horizontalComponent << std::endl;
    std::cout << m_verticalComponent << std::endl;
    std::cout << m_horizontalAmplitude << std::endl;
    std::cout << m_verticalAmplitude << std::endl;
}
