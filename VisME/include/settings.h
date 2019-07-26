#ifndef SETTINGS_H
#define SETTINGS_H

#include "colors.h"
#include "enums.h"
#include "trial.h"
#include <QMap>
#include <QObject>
#include <vector>

class Trial;

/**********************************************************************************************//**
 * struct SaccadeFilterSettings
 **************************************************************************************************/
struct SaccadeFilterSettings
{
    SaccadeFilterSettings():
        m_fromInputFile(true),
        m_velocityThreshold(5),
        m_minDuration(6),
        m_binocular(true),
        m_maxDuration(100),
        m_maxAmplitude(1),
        m_minAmplitude(0),
        m_minIntersaccInterval(20),
        m_minVelocity(0),
        m_maxVelocity(300),
        m_ignoreAtStart(20),
        m_ignoreAtEnd(0),
        m_velocityWindowSize(5),
        m_ignoreBeforeMissingData(200),
        m_ignoreAfterMissingData(200),
        m_useMaxDuration(false),
        m_useMinAmplitude(false),
        m_useMaxAmplitude(true),
        m_useMinIntersaccInterval(true),
        m_useMinVelocity(false),
        m_useMaxVelocity(false),
        m_useIgnoreAtStart(true),
        m_useIgnoreAtEnd(false),
        m_useIgnoreBeforeMissingData(false),
        m_useIgnoreAfterMissingData(false)
    {}

    SaccadeFilterSettings(const SaccadeFilterSettings &m):
        m_fromInputFile(m.m_fromInputFile),
        m_velocityThreshold(m.m_velocityThreshold),
        m_minDuration(m.m_minDuration),
        m_binocular(m.m_binocular),
        m_maxDuration(m.m_maxDuration),
        m_maxAmplitude(m.m_maxAmplitude),
        m_minAmplitude(m.m_minAmplitude),
        m_minIntersaccInterval(m.m_minIntersaccInterval),
        m_minVelocity(m.m_minVelocity),
        m_maxVelocity(m.m_maxVelocity),
        m_ignoreAtStart(m.m_ignoreAtStart),
        m_ignoreAtEnd(m.m_ignoreAtEnd),
        m_velocityWindowSize(m.m_velocityWindowSize),
        m_ignoreBeforeMissingData(m.m_ignoreBeforeMissingData),
        m_ignoreAfterMissingData(m.m_ignoreAfterMissingData),
        m_useMaxDuration(m.m_useMaxDuration),
        m_useMinAmplitude(m.m_useMinAmplitude),
        m_useMaxAmplitude(m.m_useMaxAmplitude),
        m_useMinIntersaccInterval(m.m_useMinIntersaccInterval),
        m_useMinVelocity(m.m_useMinVelocity),
        m_useMaxVelocity(m.m_useMaxVelocity),
        m_useIgnoreAtStart(m.m_useIgnoreAtStart),
        m_useIgnoreAtEnd(m.m_useIgnoreAtEnd),
        m_useIgnoreBeforeMissingData(m.m_useIgnoreBeforeMissingData),
        m_useIgnoreAfterMissingData(m.m_useIgnoreAfterMissingData)
    {}

    SaccadeFilterSettings(
            bool fromInputFile,
            qreal velocityThreshold,
            int minMicrosaccadeDuration,
            bool binocular,
            int maxMicrosaccadeDuration,
            qreal maxMicrosaccadeAmplitude,
            qreal minMicrosaccadeAmplitude,
            int minMicrosaccadeIntersaccInterval,
            int minMicrosaccadeVelocity,
            int maxMicrosaccadeVelocity,
            int ignoreAtFixationStart,
            int ignoreAtFixationEnd,
            int velocityWindowSize,
            int ignoreBeforeMissingData,
            int ignoreAfterMissingData,
            bool useMaxDuration,
            bool useMinAmplitude,
            bool useMaxAmplitude,
            bool useMinIntersaccInterval,
            bool useMinVelocity,
            bool useMaxVelocity,
            bool useIgnoreAtFixationStart,
            bool useIgnoreAtFixationEnd,
            bool useIgnoreBeforeMissingData,
            bool useIgnoreAfterMissingData):
        m_fromInputFile(fromInputFile),
        m_velocityThreshold(velocityThreshold),
        m_minDuration(minMicrosaccadeDuration),
        m_binocular(binocular),
        m_maxDuration(maxMicrosaccadeDuration),
        m_maxAmplitude(maxMicrosaccadeAmplitude),
        m_minAmplitude(minMicrosaccadeAmplitude),
        m_minIntersaccInterval(minMicrosaccadeIntersaccInterval),
        m_minVelocity(minMicrosaccadeVelocity),
        m_maxVelocity(maxMicrosaccadeVelocity),
        m_ignoreAtStart(ignoreAtFixationStart),
        m_ignoreAtEnd(ignoreAtFixationEnd),
        m_velocityWindowSize(velocityWindowSize),
        m_ignoreBeforeMissingData(ignoreBeforeMissingData),
        m_ignoreAfterMissingData(ignoreAfterMissingData),
        m_useMaxDuration(useMaxDuration),
        m_useMinAmplitude(useMinAmplitude),
        m_useMaxAmplitude(useMaxAmplitude),
        m_useMinIntersaccInterval(useMinIntersaccInterval),
        m_useMinVelocity(useMinVelocity),
        m_useMaxVelocity(useMaxVelocity),
        m_useIgnoreAtStart(useIgnoreAtFixationStart),
        m_useIgnoreAtEnd(useIgnoreAtFixationEnd),
        m_useIgnoreBeforeMissingData(useIgnoreBeforeMissingData),
        m_useIgnoreAfterMissingData(useIgnoreAfterMissingData)
    {}

    bool m_fromInputFile;
    qreal m_velocityThreshold;
    int m_minDuration;
    bool m_binocular;
    int m_maxDuration;
    qreal m_maxAmplitude;
    qreal m_minAmplitude;
    int m_minIntersaccInterval;
    qreal m_minVelocity;
    qreal m_maxVelocity;
    int m_ignoreAtStart;
    int m_ignoreAtEnd;
    size_t m_velocityWindowSize;
    int m_ignoreBeforeMissingData;
    int m_ignoreAfterMissingData;

    bool m_useMaxDuration;
    bool m_useMinAmplitude;
    bool m_useMaxAmplitude;
    bool m_useMinIntersaccInterval;
    bool m_useMinVelocity;
    bool m_useMaxVelocity;
    bool m_useIgnoreAtStart;
    bool m_useIgnoreAtEnd;
    bool m_useIgnoreBeforeMissingData;
    bool m_useIgnoreAfterMissingData;
};

/**********************************************************************************************//**
 * struct Settings
 **************************************************************************************************/
struct Settings
{

    // Initialization values
    Settings():
        m_displayMode(Individual),
        m_currentFixationIndex(-1),
        m_backgroundOpacity(50),
        m_showSamples(true),
        m_showSampleConnections(true),
        m_showMicrosaccades(true),
        m_showMicrosaccadeDirections(false),
        m_highlightFixations(true),
        m_highlightCurrentFixation(true),
        m_showSaccadeDirections(false),
        m_showMicrosaccadesInTimeline(true),
        m_showFixationsInTimeline(true),
        m_showEventsInTimeline(false),
        m_showXValuesInTimeline(true),
        m_showYValuesInTimeline(true),
        m_showVelocityValuesInTimeline(true),
        m_showScanpath(true),
        m_fixationSize(FromDuration),
        m_fixationScale(100),
        m_showNeighboringFixations(false),
        m_followingFixationsCount(2),
        m_previousFixationsCount(0),
        m_dataPlotFixations(PF_All),
        m_dataPlotData(PD_Microsaccades),
        m_dataPlotDirection(PDir_FromView),
        m_dataPlotType(PT_PolarPlotWithHole),
        m_dataPlotBins(12),
        m_dataPlotRadiusCount(0),
        m_dataPlotRadiusDegree(0),
        m_overplot(PA_ForTestConditions),
        m_testConditionColors(true),
        m_histogramAndScatterplotData(PD_Microsaccades),
        m_histogramYAxis(Amplitude),
        m_histogramStartTimeBinSize(50),
        m_histogramPeakVelocityBinSize(5),
        m_histogramAmplitudeBinSize(0.05),
        m_histogramDurationBinSize(1),
        m_histogramMaxX(0.0),
        m_histogramMaxY(0.0),
        m_scatterplotXAxis(Amplitude),
        m_scatterplotYAxis(PeakVelocity),
        m_scatterplotMaxX(0),
        m_scatterplotMaxY(0),
        m_logarithmicScale(true),
        m_microsaccadeSettings(new SaccadeFilterSettings()),
        m_fixationSettings(new SaccadeFilterSettings(true, 8, 3, false, 100, 100000, 1, 50, 0,
                                                     1000, 0, 0, 9, 0, 0,
                                                     false, true, false, true, false, false,
                                                     false, false, false, false)),
        m_recentDir("."),
        m_colors(Colors()),
        m_activateZoomToFixationArea(false),
        participantsLoaded(false),
        trialsLoaded(false),
        testConditionsLoaded(false),
        m_gazeType(GT_Average),
        m_startTimeInMSec(0),
        m_endTimeInMSec(0),
        m_limitTimeRange(false)
    {}

    // ** Filter Settings **

    // Display mode: one ore multiple trials.
    DisplayMode m_displayMode;

    // List of currently selected trials in multi trial mode.
    std::vector<Trial*> m_currentTrials;

    // Index of the currently selected fixation in single trial mode.
    int m_currentFixationIndex;

    int m_backgroundOpacity;

    bool m_showSamples;
    bool m_showSampleConnections;
    bool m_showMicrosaccades;
    bool m_showMicrosaccadeDirections;
    bool m_highlightFixations;
    bool m_highlightCurrentFixation;
    bool m_showSaccadeDirections;

    bool m_showMicrosaccadesInTimeline;
    bool m_showFixationsInTimeline;
    bool m_showEventsInTimeline;
    bool m_showXValuesInTimeline;
    bool m_showYValuesInTimeline;
    bool m_showVelocityValuesInTimeline;

    bool m_showScanpath;
    FixationSize m_fixationSize;
    int m_fixationScale;

    bool m_showNeighboringFixations;
    int m_followingFixationsCount;
    int m_previousFixationsCount;

    // ** Data Plot Settings **
    PlotFixations m_dataPlotFixations;
    PlotData m_dataPlotData;
    PlotDirection m_dataPlotDirection;
    PlotType m_dataPlotType;
    int m_dataPlotBins;
    qreal m_dataPlotRadiusCount;
    qreal m_dataPlotRadiusDegree;
    PlotAggregation m_overplot;
    bool m_testConditionColors;

    // other plots
    PlotData m_histogramAndScatterplotData;

    // ** Histogram Settings **
    PlotValues m_histogramYAxis;
    int m_histogramStartTimeBinSize;
    qreal m_histogramPeakVelocityBinSize;
    qreal m_histogramAmplitudeBinSize;
    int m_histogramDurationBinSize;
    qreal m_histogramMaxX;
    qreal m_histogramMaxY;

    // ** Scatterplot Settings **
    PlotValues m_scatterplotXAxis;
    PlotValues m_scatterplotYAxis;
    qreal m_scatterplotMaxX;
    qreal m_scatterplotMaxY;
    bool m_logarithmicScale;

    // ** Saccade Detection Settings **
    SaccadeFilterSettings* m_microsaccadeSettings;

    // ** Fixation Detection Settings **
    SaccadeFilterSettings* m_fixationSettings;

    // ** General **
    // Directory which was last used to open a file.
    QString m_recentDir;

    // Colors used in the visualizations
    Colors m_colors;

    // When activated, timeline is zoomed to current fixation area.
    bool m_activateZoomToFixationArea;

    // Available test conditions
    QMap<QString, QColor> m_testConditions;

    // If participants/trials/test conditions were already loaded in the user interface
    bool participantsLoaded;
    bool trialsLoaded;
    bool testConditionsLoaded;

    // Gaze type: left, right or average.
    GazeType m_gazeType;

    // Values to limit analysis to a time range
    qreal m_startTimeInMSec;
    qreal m_endTimeInMSec;
    bool m_limitTimeRange;
};

#endif // SETTINGS_H
