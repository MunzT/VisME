/***********************************************************************************************//**
 * @author Tanja Munz
 * @file saccadedetector.h
 **************************************************************************************************/
#ifndef SACCADESDETECTOR_H
#define SACCADESDETECTOR_H

#include "gazedata.h"
#include "settings.h"
#include <QPair>
#include <vector>

class Saccade;
class Trial;
struct Settings;

/***********************************************************************************************//**
 * This class is responsible for detecting (micro)saccades.
 **************************************************************************************************/
class SaccadeDetector
{

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    SaccadeDetector();

    /*******************************************************************************************//**
     * Determines microsaccades for the given trial and the given parameters.
     *
     * @param trial    trial for which microsaccades are determined
     * @param settings global settings
     * @return list of microsaccades
     **********************************************************************************************/
    std::vector<std::vector<Saccade> > determineMicrosaccades(Trial* trial, Settings* settings);

    /*******************************************************************************************//**
     * Determines Saccades for the given trial and the given parameters.
     *
     * @param trial    trial for which saccades are determined
     * @param settings contains microsaccade settings
     * @return list of saccades
     **********************************************************************************************/
    std::vector<std::vector<Saccade> > determineRegularSaccades(Trial* trial, Settings* settings);

    /*******************************************************************************************//**
     * Removes microsaccades from the fixation which are inside the inter-saccadic interval
     *
     * @param trial    trial for which saccades are determined
     * @param settings contains microsaccade settings
     * @param fixation current fixation
     **********************************************************************************************/
    void removeMicrosaccadesForIntersaccadicInterval(Trial* trial, Settings* settings, Fixation* fixation);

private:

    /*******************************************************************************************//**
     * Determines microsaccades for the given settings.
     *
     * @param gazeData      raw eye tracking data
     * @param indicesStart  start index in raw data that defines the range where the filter is used
     * @param indicesEnd    end index for the range
     * @param trial         trial for which microsaccades are determined
     * @param settings      global settings
     * @return list of microsaccades
     **********************************************************************************************/
    std::vector<Saccade> calculateMicrosaccades(const QMap<int, GazeData>& gazeData,
                                                int indicesStart, int indicesEnd,
                                                Trial* trial,
                                                Settings* settings);

    /*******************************************************************************************//**
     * Computes (micro)saccades.
     *
     * Method described in
     * Ralf Engbert and Reinhold Kliegl: Microsaccades uncover the orientation of covert attention.
     * Code in R is available at:
     * http://read.psych.uni-potsdam.de/index.php?option=com_content&view=article&id=140:engbert-et-
     * al-2015-microsaccade-toolbox-for-r&catid=26:publications&Itemid=34
     *
     * @param  x         input gaze data: matrix containing x and y position for one eye
     *                       (matrix with two columns)
     * @param newRadius parameters of elliptic threshold
     * @param settings  filter settings
     * @param trial     current trial
     * @return list of saccades with their properties (index range, peak velocity, amplitude...)
     **********************************************************************************************/
    std::vector<Saccade> saccadesFilter(std::vector<std::vector<qreal> >& x,
                                        QPair<qreal, qreal>& newRadius,
                                        SaccadeFilterSettings* settings,
                                        Trial* trial);

    /*******************************************************************************************//**
     * Compute velocity times series from position data taking multiple position values into account
     * for each time stamp. The result represents a moving average of velocities over 5
     * data sample to suppress noise.
     *
     * Method described in
     * Ralf Engbert and Reinhold Kliegl: Microsaccades uncover the orientation of covert attention.
     * Code in R is available at:
     * http://read.psych.uni-potsdam.de/index.php?option=com_content&view=article&id=140:engbert-et-
     * al-2015-microsaccade-toolbox-for-r&catid=26:publications&Itemid=34
     *
     * @param xValues    input gaze data: matrix containing x and y position for one eye
     *                   (matrix with two columns)
     * @param frequency  sampling rate
     * @param windowSize window size for the velocity calculation
     * @return matrix with velocity values
     **********************************************************************************************/
    std::vector<std::vector<qreal> > determineVelocity(std::vector<std::vector<qreal> >& xValues,
                                                       qreal frequency, size_t windowSize);
                                                       //, int type = 2);

    /*******************************************************************************************//**
     * Computes binocular microsaccades.
     *
     * Method described in
     * Ralf Engbert and Reinhold Kliegl: Microsaccades uncover the orientation of covert attention.
     * Code in R is available at:
     * http://read.psych.uni-potsdam.de/index.php?option=com_content&view=article&id=140:engbert-et-
     * al-2015-microsaccade-toolbox-for-r&catid=26:publications&Itemid=34
     *
     * @param leftEye saccades data for left eye
     * @param rightEye saccades data for right eye
     * @return list of saccades
     **********************************************************************************************/
    QPair<std::vector<Saccade>, std::vector<Saccade> > determineBinocularSaccades(
            std::vector<Saccade> leftEye,
            std::vector<Saccade> rightEye);
};

#endif // SACCADESDETECTOR_H
