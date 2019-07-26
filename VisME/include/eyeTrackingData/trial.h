/***********************************************************************************************//**
 * @author Tanja Munz
 * @file trial.h
 **************************************************************************************************/
#ifndef TRIAL_H
#define TRIAL_H

#include "event.h"
#include "fixation.h"
#include "gazedata.h"
#include "participant.h"
#include "saccade.h"
#include "settings.h"
#include <iostream>
#include <QMap>
#include <set>
#include <vector>

class Fixation;
class Participant;
struct SaccadeFilterSettings;
struct Settings;

/***********************************************************************************************//**
 * This class contains information on a trialof an eye tracking study.
 **************************************************************************************************/
class Trial
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    Trial(Participant* p, Settings* settings);

    /*******************************************************************************************//**
     * Destructor
     **********************************************************************************************/
    ~Trial();

    /*******************************************************************************************//**
     * Returns the frequency rate of the trial.
     *
     * @return frequency rate
     **********************************************************************************************/
    qreal frequencyRate() const;

    /*******************************************************************************************//**
     * Sets the frequency rate of the trial.
     *
     * @param frequencyRate frequency rate
     **********************************************************************************************/
    void setFrequencyRate(qreal frequencyRate);

    /*******************************************************************************************//**
     * Returns the trial name.
     *
     * @return trial name
     **********************************************************************************************/
    QString name() const;

    /*******************************************************************************************//**
     * Returns the trial name but each "." is replaced by a "_".
     *
     * @return trial name
     **********************************************************************************************/
    QString modifiedName() const;

    /*******************************************************************************************//**
     * Sets the trial name.
     *
     * @param name trial name
     **********************************************************************************************/
    void setName(const QString& name);

    /*******************************************************************************************//**
     * Returns the participant.
     *
     * @return participant
     **********************************************************************************************/
    Participant* getParticipant() const;

    /*******************************************************************************************//**
     * Returns the stimulus file path.
     *
     * @return stimulus fiel path of the trial
     **********************************************************************************************/
    QString getStimulus() const;

    /*******************************************************************************************//**
     * Sets the stimulus file path.
     *
     * @param stimulus stimulus fiel path of the trial
     **********************************************************************************************/
    void setStimulus(const QString& stimulus);

    /*******************************************************************************************//**
     * Sets the coordinates for the screen resoultion.
     *
     * @param minX minimum x value
     * @param maxX maximum x value
     * @param minY minimum y value
     * @param maxY maximum y value
     **********************************************************************************************/
    void setMinMax(qreal minX, qreal maxX, qreal minY, qreal maxY);

    /*******************************************************************************************//**
     * Returns the minimum x coordinate of the screen resoultion.
     *
     * @return minimum x value
     **********************************************************************************************/
    qreal minX() const;

    /*******************************************************************************************//**
     * Returns the minimum y coordinate of the screen resoultion.
     *
     * @return minimum y value
     **********************************************************************************************/
    qreal minY() const;

    /*******************************************************************************************//**
     * Returns the maximum x coordinate of the screen resoultion.
     *
     * @return maximum x value
     **********************************************************************************************/
    qreal maxX() const;

    /*******************************************************************************************//**
     * Returns the maximum y coordinate of the screen resoultion.
     *
     * @return maximum y value
     **********************************************************************************************/
    qreal maxY() const;

    /*******************************************************************************************//**
     * Sets the given gaze data to the given gaze type.
     *
     * @param gazeData gaze data with index and values
     * @param gazeType gaze type of the eye values
     **********************************************************************************************/
    void setGazeData(const QMap<int, GazeData> gazeData, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the number of elements of the gaze data.
     *
     * @param gazeType gaze type of the eye values
     **********************************************************************************************/
    size_t gazeDataCount(GazeType gazeType = GazeType::GT_Unset) const;

    /*******************************************************************************************//**
     * Returns the gaze data for the given gaze type.
     *
     * @param gazeType gaze type of the eye values
     * @return gaze data
     **********************************************************************************************/
    QMap<int, GazeData> gazeData(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the list of fixations.
     *
     * @param gazeType gaze type of the eye values
     * @return list of fixations
     **********************************************************************************************/
    std::vector<Fixation>* fixations(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the list of pointers to fixations.
     *
     * @param gazeType gaze type of the eye values
     * @return list of pointers to fixations
     **********************************************************************************************/
    std::vector<Fixation*> fixationPointers(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Sets the list of fixations.
     *
     * @param fixations list of fixations
     * @param gazeType  gaze type of the eye values
     **********************************************************************************************/
    void setFixations(const std::vector<Fixation>& fixations, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Sets the list of saccades.
     *
     * @param saccades  list of saccades
     * @param gazeType  gaze type of the eye values
     **********************************************************************************************/
    void setSaccades(const std::vector<Saccade>& saccades, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the number of fixations in this trial.
     *
     * @param gazeType gaze type of the eye values
     * @return number of fixations
     **********************************************************************************************/
    int fixationsCount(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the fixation with the given index.
     *
     * @param index    fixation index
     * @param gazeType gaze type of the eye values
     * @return fixation with the given index
     **********************************************************************************************/
    Fixation* getFixationAt(int index, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the next fixation to the given one.
     *
     * @param fixation current fixation
     * @param gazeType gaze type of the eye values
     * @return next fixation
     **********************************************************************************************/
    Fixation* getNextFixation(Fixation* fixation, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the previous fixation to the given one.
     *
     * @param fixation current fixation
     * @param gazeType gaze type of the eye values
     * @return previous fixation
     **********************************************************************************************/
    Fixation* getPrevFixation(Fixation* fixation, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the fixation index of the given fixation.
     *
     * @param f        fixation
     * @param gazeType gaze type of the eye values
     **********************************************************************************************/
    int getFixationIndex(Fixation* f, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the number of saccades in this trial.
     *
     * @param gazeType gaze type of the eye values
     * @return number of saccades
     **********************************************************************************************/
    size_t saccadesCount(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the saccade with the given index.
     *
     * @param index    fixation index
     * @param gazeType gaze type of the eye values
     * @return saccade with the given index
     **********************************************************************************************/
    Saccade* getSaccadeAt(size_t index, GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns all saccades for the given gaze type.
     *
     * @param gazeType gaze type of the eye values
     * @return list of all saccades
     **********************************************************************************************/
    std::vector<Saccade>* saccades(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns if fixations from the input file shall be used.
     *
     * @return if fixations from the input file shall be used.
     **********************************************************************************************/
    bool useFixationsFromInput() const;

    /*******************************************************************************************//**
     * Sets if fixations from the input file shall be used.
     *
     * @param useFixationsFromInput if fixations from the input file shall be used
     **********************************************************************************************/
    void setUseFixationsFromInput(bool useFixationsFromInput);

    /*******************************************************************************************//**
     * Sets the given microsaccades for the fixations to which they belong.
     * Expects that list of microsaccades is sorted!
     *
     * @param microsaccades list of microsaccades
     * @param gazeType      gaze type of the eye values
     **********************************************************************************************/
    void setMicrosaccadesForFixations(std::vector<Saccade>* microsaccades,
                                      GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the microsaccade for the givenfixation and microsaccades index.
     *
     * @param fixationIndex     fixation index
     * @param microsaccadeIndex microsaccade index within the fixation
     * @param gazeType          gaze type of the eye values
     **********************************************************************************************/
    Saccade* getMicrosaccadeAt(int fixationIndex, size_t microsaccadeIndex,
                               GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the sum of all microsaccades in tis trial.
     *
     * @param gazeType gaze type of the eye values
     * @return number of microsaccades in this trial
     **********************************************************************************************/
    size_t microsaccadesCount(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Updates the current microseccade detection settings from the given settings.
     **********************************************************************************************/
    void updateMicrosaccadesSettings();

    /*******************************************************************************************//**
     * Updates the current fixation detection settings from the given settings.
     **********************************************************************************************/
    void updateFixationSettings();

    /*******************************************************************************************//**
     * Returns if the visible microsaccades were defined in the input file.
     *
     * @return if the visible microsaccades were defined in the input file
     **********************************************************************************************/
    bool msFromInputFile();

    /*******************************************************************************************//**
     * Returns if the visible fixations were defined in the input file.
     *
     * @return if the visible fixations were defined in the input file
     **********************************************************************************************/
    bool fixationsFromInputFile();

    /*******************************************************************************************//**
     * Returns the microsaccade detection settings of this trial.
     *
     * @return microsaccade detection settings
     **********************************************************************************************/
    SaccadeFilterSettings* getMsFilterSettings() const;

    /*******************************************************************************************//**
     * Returns the Fixation detection settings of this trial.
     *
     * @return microsaccade detection settings
     **********************************************************************************************/
    SaccadeFilterSettings* getFixationFilterSettings() const;

    /*******************************************************************************************//**
     * Returns all indeces of the gaze data list which belong to microsaccades.
     *
     * @param gazeType gaze type of the eye values
     * @return return list of indeces of gazeData which beong to microsaccades.
     **********************************************************************************************/
    std::set<int> getMicrosaccadeIndices(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Prints all microsaccade data.
     *
     * @param gazeType gaze type of the eye values
     **********************************************************************************************/
    void printAllMicrosaccades(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the test condition type.
     *
     * @return test condition type
     **********************************************************************************************/
    QString getTestConditionType() const;

    /*******************************************************************************************//**
     * Sets the test condition type.
     *
     * @param testConditionType test condition type
     **********************************************************************************************/
    void setTestConditionType(const QString& testConditionType);

    /*******************************************************************************************//**
     * Sets events.
     *
     * @param events events related to the eye movement
     **********************************************************************************************/
    void setEvents(std::vector<Event> events);

    /*******************************************************************************************//**
     * Returns the number of events.
     *
     * @return number of events
     **********************************************************************************************/
    size_t eventCount();

    /*******************************************************************************************//**
     * Returns the event with the given index.
     *
     * @return event with the given index
     **********************************************************************************************/
    Event* getEventAt(size_t index);

    /*******************************************************************************************//**
     * Returns the duration of this trial in ms.
     *
     * @param gazeType gaze type of the eye values
     * @return duration of the trial
     **********************************************************************************************/
    int duration(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the sample index of the first gaze position.
     *
     * @param gazeType gaze type of the eye values
     * @return sample index of the first gaze position
     **********************************************************************************************/
    int firstIndex(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Returns the sample index of the last gaze position.
     *
     * @param gazeType gaze type of the eye values
     * @return sample index of the last gaze position
     **********************************************************************************************/
    int lastIndex(GazeType gazeType = GazeType::GT_Unset);

    /*******************************************************************************************//**
     * Transforms the samples to time considering the frequency.
     *
     * @param samples
     * @return corresponding time in ms
     **********************************************************************************************/
    qreal samplesToTime(int samples);

    /*******************************************************************************************//**
     * Transforms the time to samples considering the frequency.
     *
     * @param time in ms
     * @return corresponding samples
     **********************************************************************************************/
    size_t timeToSamples(qreal time);

    /*******************************************************************************************//**
     * Transforms the time value to the sample in the trial considering the frequency.
     *
     * @param time in ms
     * @return corresponding sample
     **********************************************************************************************/
    int timeToSampleInTrial(qreal time);

    /*******************************************************************************************//**
     * Checks if start and end indices are outside the time range if time limit is activated.
     *
     * @param start start index
     * @param end   end index
     * @return if start and end indices are within the time range if it is activated
     **********************************************************************************************/
    bool outsideTimeLimitIfActivated(int start, int end);

private:

    /// Name of participant/file.
    QString m_name;

    /// Participant to which this trial belongs.
    Participant* m_participant;

    /// Current settings for visualizations.
    Settings* m_settings;

    /// Test Condition type of this trial
    QString m_testConditionType;

    /// Stimulus file path.
    QString m_stimulus;

    /// events
    std::vector<Event> m_events;

    /// Frequency of trial.
    qreal m_frequencyRate;

    /// List of raw data samples for the right eye.
    QMap<int, GazeData> m_gazeDataRight;

    /// List of raw data samples for the left eye.
    QMap<int, GazeData> m_gazeDataLeft;

    /// List of raw data samples with averaged values.
    QMap<int, GazeData> m_gazeDataAverage;

    /// If fixations shall be used from the input file or if the ones calculated by the
    /// application shall be used.
    bool m_useFixationsFromInput;

    /// Lists of fixations from the input file.
    std::vector<Fixation> m_inputFixationsRight;
    std::vector<Fixation> m_inputFixationsLeft;
    std::vector<Fixation> m_inputFixationsAverage;

    /// Lists of calculated fixations.
    std::vector<Fixation> m_calculatedFixationsRight;
    std::vector<Fixation> m_calculatedFixationsLeft;
    std::vector<Fixation> m_calculatedFixationsAverage;

    /// Lists of calculated saccades.
    std::vector<Saccade> m_calculatedSaccadesRight;
    std::vector<Saccade> m_calculatedSaccadesLeft;
    std::vector<Saccade> m_calculatedSaccadesAverage;

    /// Settings for the microsaccade detection algorithm.
    SaccadeFilterSettings* m_microsaccadeSettings;

    /// Settings for the microsaccade detection algorithm.
    SaccadeFilterSettings* m_fixationSettings;

    /// Screen dimensions of trial.
    qreal m_minX = 0;
    qreal m_minY = 0;
    qreal m_maxX = 0;
    qreal m_maxY = 0;

};

#endif // TRIAL_H
