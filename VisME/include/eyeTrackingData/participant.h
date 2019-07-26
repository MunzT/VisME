/***********************************************************************************************//**
 * @author Tanja Munz
 * @file participant.h
 **************************************************************************************************/
#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "trial.h"
#include <iostream>
#include <vector>

class Trial;

/***********************************************************************************************//**
 * This class represents a participant - it contains general eye tracking setup data and trials.
 **************************************************************************************************/
class Participant
{

public:

    /*******************************************************************************************//**
     * Constructor.
     *
     * @param filePath file path for the data file which identifies this participant.
     **********************************************************************************************/
    Participant(QString filePath);

    /*******************************************************************************************//**
     * Adds a trial to this participant.
     *
     * @param trial new trial
     **********************************************************************************************/
    void addTrial(Trial& trial);

    /*******************************************************************************************//**
     * Returns the number of trials this participant participated.
     *
     * @return number of trials
     **********************************************************************************************/
    size_t numberOfTrials() const;

    /*******************************************************************************************//**
     * Returns the trials with the given index.
     *
     * @return trial with the given index
     **********************************************************************************************/
    Trial* getTrialAt(int index);

    /*******************************************************************************************//**
     * Returns the data file path for this participant.
     *
     * @return file path
     **********************************************************************************************/
    QString filePath() const;

    /*******************************************************************************************//**
     * Returns the name of this participant (e.g. file name).
     *
     * @return name of participant
     **********************************************************************************************/
    QString name() const;

    /*******************************************************************************************//**
     * Returns the name of this participant (e.g. file name) but each "." is replaced by a "_".
     *
     * @return name of participant
     **********************************************************************************************/
    QString modifiedName() const;

    /*******************************************************************************************//**
     * Sets the name of this participant (e.g. file name).
     *
     * @param name name of participant
     **********************************************************************************************/
    void setName(const QString& name);

    /*******************************************************************************************//**
     * Returns the value for pixels per degree.
     *
     * @return pixels per degree
     **********************************************************************************************/
    qreal getPixelsPerDegree() const;

    /*******************************************************************************************//**
     * Sets the value for pixels per degree.
     *
     * @param pixelsPerDegree pixels per degree
     **********************************************************************************************/
    void setPixelsPerDegree(const qreal& pixelsPerDegree);

private:

    /// List of all trials.
    std::vector<Trial> m_trials;

    /// File path of .maf file.
    QString m_filePath;

    /// Name of participant (e.g. file name).
    QString m_name;

    /// Pixels per degree
    qreal m_pixelsPerDegree;
};

#endif // PARTICIPANT_H
