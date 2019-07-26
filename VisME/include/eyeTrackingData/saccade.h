/***********************************************************************************************//**
 * @author Tanja Munz
 * @file saccade.h
 **************************************************************************************************/
#ifndef SACCADE_H
#define SACCADE_H

#include <QString>

/***********************************************************************************************//**
 * This class contains information of a microsaccade.
 **************************************************************************************************/
class Saccade
{

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    Saccade(size_t onset, size_t end, qreal peakVelocity, qreal horizontalComponent,
            qreal verticalComponent, qreal horizontalAmplitude, qreal verticalAmplitude);

    /*******************************************************************************************//**
     * Returns the start index.
     *
     * @return start index
     **********************************************************************************************/
    size_t getOnsetIndex() const;

    /*******************************************************************************************//**
     * Returns the end index.
     *
     * @return end index
     **********************************************************************************************/
    size_t getEndIndex() const;

    /*******************************************************************************************//**
     * Returns the duration (in indices).
     *
     * @return duration
     **********************************************************************************************/
    size_t getDuration() const;

    /*******************************************************************************************//**
     * Returns the velocity peak.
     *
     * @return velocity peak
     **********************************************************************************************/
    qreal getPeakVelocity() const;

    /*******************************************************************************************//**
     * Returns the horizontal component.
     *
     * @return horizontal component
     **********************************************************************************************/
    qreal getHorizontalComponent() const;

    /*******************************************************************************************//**
     * Returns the vertical component.
     *
     * @return vertical component
     **********************************************************************************************/
    qreal getVerticalComponent() const;

    /*******************************************************************************************//**
     * Returns the horizontal amplitude.
     *
     * @return horizontal amplitude
     **********************************************************************************************/
    qreal getHorizontalAmplitude() const;

    /*******************************************************************************************//**
     * Returns the vertical amplitude.
     *
     * @return vertical amplitude
     **********************************************************************************************/
    qreal getVerticalAmplitude() const;

    /*******************************************************************************************//**
     * Returns the amplitude calculated from vertical and horizontal amplitude.
     *
     * @return vertical amplitude
     **********************************************************************************************/
    qreal getAmplitude() const;

    /*******************************************************************************************//**
     * Returns the isValid value (e.g if all conditions for a microsaccade are fullfilled).
     *
     * @return if element is valid
     **********************************************************************************************/
    bool isValid() const;

    /*******************************************************************************************//**
     * Sets the start index.
     *
     * @param onset start index
     **********************************************************************************************/
    void setOnsetIndex(size_t onset);

    /*******************************************************************************************//**
     * Sets the end index.
     *
     * @param end end index
     **********************************************************************************************/
    void setEndIndex(size_t end);

    /*******************************************************************************************//**
     * Sets the velocity peak.
     *
     * @param peakVelocity velocity peak
     **********************************************************************************************/
    void setPeakVelocity(qreal peakVelocity);

    /*******************************************************************************************//**
     * Sets the horizontal component.
     *
     * @param horizontalComponent horizontal component
     **********************************************************************************************/
    void setHorizontalComponent(qreal horizontalComponent);

    /*******************************************************************************************//**
     * Sets the vertical component.
     *
     * @param verticalComponent vertical component
     **********************************************************************************************/
    void setVerticalComponent(qreal verticalComponent);

    /*******************************************************************************************//**
     * Sets the horizontal amplitude.
     *
     * @param horizontalAmplitude horizontal amplitude
     **********************************************************************************************/
    void setHorizontalAmplitude(qreal horizontalAmplitude);

    /*******************************************************************************************//**
     * Sets the vertical amplitude.
     *
     * @param verticalAmplitude vertical amplitude
     **********************************************************************************************/
    void setVerticalAmplitude(qreal verticalAmplitude);

    /*******************************************************************************************//**
     * Sets the m_isValid value.
     *
     * @param isValid if element fulfills all microsaccades criteria
     **********************************************************************************************/
    void setValid(bool isValid);

    /*******************************************************************************************//**
     * Prints all important values of this element.
     **********************************************************************************************/
    void print();

private:

    /// Start index of microsaccade.
    size_t m_onset;

    /// End index of microsaccade.
    size_t m_end;

    /// Peak velocity of microsaccade.
    qreal m_peakVelocity;

    /// Further properties of microsaccade. All in degree.
    qreal m_horizontalComponent;
    qreal m_verticalComponent;
    qreal m_horizontalAmplitude;
    qreal m_verticalAmplitude;

    /// If this element fullfills all criterias for e.g. being a microsaccade.
    bool m_isValid;
};

#endif // SACCADE_H
