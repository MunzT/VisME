#ifndef ENUMS_H
#define ENUMS_H

/**********************************************************************************************//**
 * Differnt display modes for the visualizations (show for just one participant/tiral or use
 * overplotting for multiple trials).
 **************************************************************************************************/
enum DisplayMode
{
    Individual,
    Multi
};

/**********************************************************************************************//**
 * Enum for fixation size.
 **************************************************************************************************/
enum FixationSize
{
    FromDuration,
    Equal,
    FromMicrosaccadesCount
};

/***********************************************************************************************//**
 * Different plot types.
 **************************************************************************************************/
enum PlotType
{
    PT_RegularPolarPlot, // 0
    PT_PolarPlotWithHole, // 1
    PT_BoxRosePlot,
    PT_BoxRosePlotWithHole,
    PT_RosePlot,
    PT_RosePlotWithHole,
    PT_HistogramBarPlot,
    PT_RadialScatterPlot,
    PT_RadialConnectedScatterPlot,
    PT_RadialRawDataPlot,
    PT_None //10
};

/***********************************************************************************************//**
 * Different plot values.
 **************************************************************************************************/
enum PlotData
{
    PD_Microsaccades,
    PD_Saccades,
    PD_GazeRawData
};

/***********************************************************************************************//**
 * Different plot data for histograms/scatterplots.
 **************************************************************************************************/
enum PlotValues
{
    PeakVelocity,
    Duration,
    Amplitude,
    MSStart
};

/***********************************************************************************************//**
 * Orientations for plotted data.
 **************************************************************************************************/
enum PlotDirection
{
    PDir_FromView,
    PDir_ToNextFixation,
    PDir_ToPreviousFixation
};

/***********************************************************************************************//**
 * Elements which will be used for the plots (current fixation/trial/all). If multiple trials
 * shall be plotted "all" is used.
 **************************************************************************************************/
enum PlotFixations
{
    PF_All,
    PF_Visible,
    PF_Current
};

/***********************************************************************************************//**
 * Specifies the gaze type: the left, right, average eye positions or if not specified.
 **************************************************************************************************/
enum GazeType
{
    GT_Right,
    GT_Left,
    GT_Average,
    GT_Unset
};

/***********************************************************************************************//**
 * Specifies the aggregation type in the radial plots.
 **************************************************************************************************/
enum PlotAggregation
{
    PA_NoAggregation,
    PA_All,
    PA_ForTestConditions,
    PA_ForTrials,
    PA_ForParticipants
};

#endif // ENUMS_H
