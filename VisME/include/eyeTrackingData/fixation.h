/***********************************************************************************************//**
 * @author Tanja Munz
 * @file fixation.h
 **************************************************************************************************/
#ifndef FIXATION_H
#define FIXATION_H

#include "saccade.h"
#include "settings.h"
#include <vector>

struct Settings;

/***********************************************************************************************//**
 * This class contains all information of a fixation.
 **************************************************************************************************/
class Fixation
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    Fixation(Settings* settings);

    /*******************************************************************************************//**
     * Returns the x position.
     *
     * @return x position
     **********************************************************************************************/
    qreal getXPosition() const;

    /*******************************************************************************************//**
     * Sets the x position.
     *
     * @param xPosition x position
     **********************************************************************************************/
    void setXPosition(qreal xPosition);

    /*******************************************************************************************//**
     * Returns the y position.
     *
     * @return y position
     **********************************************************************************************/
    qreal getYPosition() const;

    /*******************************************************************************************//**
     * Sets the y position.
     *
     * @param yPosition y position
     **********************************************************************************************/
    void setYPosition(qreal yPosition);

    /*******************************************************************************************//**
     * Returns the start index of the fixation.
     *
     * @return start index
     **********************************************************************************************/
    int getStartIndex() const;

    /*******************************************************************************************//**
     * Sets the start index of the fixation.
     *
     * @param value start index
     **********************************************************************************************/
    void setStartIndex(int value);

    /*******************************************************************************************//**
     * Returns the last index of the fixation.
     *
     * @return last index
     **********************************************************************************************/
    int getEndIndex() const;

    /*******************************************************************************************//**
     * Returns the duration of the fixation.
     *
     * @return duration
     **********************************************************************************************/
    int getDuration() const;

    /*******************************************************************************************//**
     * Sets the duration of the fixation.
     *
     * @param value duration
     **********************************************************************************************/
    void setDuration(int value);

    /*******************************************************************************************//**
     * Returns the list containing all microsaccades in this fixation.
     *
     * @return list of microsaccades
     **********************************************************************************************/
    std::vector<Saccade>* getMicrosaccades(bool fromInput);

    /*******************************************************************************************//**
     * Removes all microsaccades with the given index.
     *
     * @param microsaccadeIndices list of microsaccade indices
     * @param fromInput           if microsaccade is from input or calculated
     **********************************************************************************************/
    void removeMicrosaccades(std::vector<int> microsaccadeIndices, bool fromInput);

    /*******************************************************************************************//**
     * Sets the list containing all microsaccades in this fixation.
     *
     * @param microsaccades list of microsaccades
     * @param fromInput if microsaccade is from input or calculated
     **********************************************************************************************/
    void setMicrosaccades(const std::vector<Saccade>& microsaccades, bool fromInput);

    /*******************************************************************************************//**
     * Adds the microsaccade to the microsaccade list.
     *
     * @param m         new microsaccade
     * @param fromInput if microsaccade is from input or calculated
     **********************************************************************************************/
    void addMicrosaccade(Saccade m, bool fromInput);

    /*******************************************************************************************//**
     * Deletes all microsaccades - either for the list of microsaccades from input
     *
     * @param fromInput if microsaccade is from input or calculated
     **********************************************************************************************/
    void clearMicrosaccades(bool fromInput);

    /*******************************************************************************************//**
     * Returns the microsaccade at the given index.
     *
     * @param index microsaccade index
     * @param fromInput if microsaccade is from input or calculated
     **********************************************************************************************/
    Saccade* getMicrosaccadeAt(const size_t index, bool fromInput);

    /*******************************************************************************************//**
     * Returns the number of microsaccades in this fixation.
     *
     * @return number of microsaccades
     **********************************************************************************************/
    size_t getMicrosaccadesCount(bool fromInput);

    /*******************************************************************************************//**
     * Getter method for m_marked.
     *
     * @return fixation was marked.
     **********************************************************************************************/
    bool isMarked() const;

    /*******************************************************************************************//**
     * Setter method for m_marked.
     *
     * @param isMarked mark fixation
     **********************************************************************************************/
    void setMarked(bool isMarked);

private:

    /// Current settings for visualizations.
    Settings* m_settings;

    /// x position (center of fixation).
    qreal m_xPosition;

    /// y position (center of fixation).
    qreal m_yPosition;

    /// Start index.
    int m_startIndex;

    /// Duration in umber of samples.
    int m_duration;

    /// Lists of currently used microsaccades.
    std::vector<Saccade> m_calculatedMicrosaccades;

    /// Lists of microsaccades from the input file.
    std::vector<Saccade> m_inputMicrosaccades;

    /// Interaction state of the element: marked (e.g. clicked).
    bool m_marked;

};

#endif // FIXATION_H
