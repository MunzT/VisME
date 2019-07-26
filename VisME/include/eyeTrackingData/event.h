/***********************************************************************************************//**
 * @author Tanja Munz
 * @file event.h
 **************************************************************************************************/
#ifndef EVENT_H
#define EVENT_H

#include <QString>

/***********************************************************************************************//**
 * This class represents an event related to the eye movement.
 **************************************************************************************************/
class Event
{
public:

    /*******************************************************************************************//**
     * Constructor
     * Initializes a new event.
     *
     * @param start    start index of event
     * @param duration duration of event in indices
     * @param name     name of event
     **********************************************************************************************/
    Event(int start, int duration, QString name);

    /*******************************************************************************************//**
     * Returns the start index of the event.
     *
     * @return start index of event
     **********************************************************************************************/
    int getStartIndex();

    /*******************************************************************************************//**
     * Returns the end index of the event.
     *
     * @return end index of event
     **********************************************************************************************/
    int getEndIndex();

    /*******************************************************************************************//**
     * Returns the name of the event.
     *
     * @return name of event
     **********************************************************************************************/
    QString getName();

    /*******************************************************************************************//**
     * Returns the duration of the event.
     *
     * @return duration of event
     **********************************************************************************************/
    int getDuration() const;

private:

    /// Start index of event
    int m_start;

    /// Duration of event in indices.
    int m_duration;

    /// Name of event.
    QString m_name;
};

#endif // EVENT_H
