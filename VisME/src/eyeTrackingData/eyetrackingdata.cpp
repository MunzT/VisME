/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyetrackingdata.cpp
 **************************************************************************************************/
#include "eyetrackingdata.h"


EyeTrackingData::EyeTrackingData()
{}


EyeTrackingData::~EyeTrackingData()
{
    qDeleteAll(m_participants);
}


void EyeTrackingData::deleteAllParticipants()
{
    qDeleteAll(m_participants);
    m_participants.clear();
}


void EyeTrackingData::clearAllParticipants()
{
    m_participants.clear();
}


size_t EyeTrackingData::numberOfParticipants() const
{
    return m_participants.size();
}


Participant* EyeTrackingData::participantAt(const int index)
{
    if (index >= 0)
    {
        return m_participants[size_t(index)];
    }
    return nullptr;
}


void EyeTrackingData::addParticipant(Participant* p)
{
    m_participants.push_back(p);
}

