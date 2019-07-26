/***********************************************************************************************//**
 * @author Tanja Munz
 * @file participant.cpp
 **************************************************************************************************/
#include "participant.h"


Participant::Participant(QString filePath) :
    m_filePath(filePath)
{}


void Participant::addTrial(Trial& trial)
{
    m_trials.push_back(trial);
}


size_t Participant::numberOfTrials() const
{
    return m_trials.size();
}


Trial* Participant::getTrialAt(int index)
{
    if (int(m_trials.size()) <= index)
    {
        return nullptr;
    }
    return &m_trials[size_t(index)];
}


QString Participant::filePath() const
{
    return m_filePath;
}


QString Participant::name() const
{
    return m_name;
}


QString Participant::modifiedName() const
{
    QString newName = m_name;
    newName.replace(".", "_");
    return newName;
}


void Participant::setName(const QString& name)
{
    m_name = name;
}


qreal Participant::getPixelsPerDegree() const
{
    return m_pixelsPerDegree;
}


void Participant::setPixelsPerDegree(const qreal& pixelsPerDegree)
{
    m_pixelsPerDegree = pixelsPerDegree;
}
