/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mainwindow.h
 **************************************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "eyedataimport.h"
#include "eyetrackingdata.h"
#include "mainscene.h"
#include "participant.h"
#include "settingsimport.h"
#include "timelinescene.h"
#include "timelineview.h"
#include "timelinevisualization.h"
#include "userinterface.h"
#include <QDateTime>
#include <QMainWindow>
#include <set>

class MainScene;

/***********************************************************************************************//**
 * Main window of the application: responsible for application workflow (reading input file,
 * extracting relevant information and triggering visualization).
 **************************************************************************************************/
class MainWindow : public QMainWindow
{

    Q_OBJECT

    friend class UserInterface;

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    MainWindow(QWidget* parent = nullptr);

    /*******************************************************************************************//**
     * Destructor
     **********************************************************************************************/
    virtual ~MainWindow();

    /*******************************************************************************************//**
     * Reads the given file and extracts relevant fixation/microsaccade data to visualize them.
     *
     * @param fileNames list of file names
     * @param add       if new participants shall be added or if old participants shall be removed
     *                  first
     **********************************************************************************************/
    void openFiles(const QStringList& fileNames, bool add = false);

    /*******************************************************************************************//**
     * Shows the data for the given participant and trial indices in the view.
     *
     * @param resetTimeline size of timeline will be resetted
     **********************************************************************************************/
    void showData(bool resetTimeline = true);

private:

    /*******************************************************************************************//**
     * Creates a new main and timeline scene with necessary settings and
     * connections for a new visualization.
     **********************************************************************************************/
    void newScenes();

    /*******************************************************************************************//**
     * Determines microsaccades for the given trial using the parameters specified in the UI.
     *
     * @param trial trial which will be used for the calculation
     **********************************************************************************************/
    void updateMicrosaccadesDetection(Trial* trial);

    /*******************************************************************************************//**
     * Determines fixations for the given trial.
     *
     * @param trial trial for which fixations are determined
     **********************************************************************************************/
    void updateFixationsDetection(Trial* trial);

    /*******************************************************************************************//**
     * Updates the general infos in the dock widget.
     *
     * @param currentTrials currently visible trials
     **********************************************************************************************/
    void updateGeneralInfo(std::vector<Trial*> currentTrials);

    /*******************************************************************************************//**
     * Updates the statistics in the dock widget.
     *
     * @param currentTrials currently visible trial
     **********************************************************************************************/
    void updateStatisticsInfo(std::vector<Trial*> currentTrials);

    /*******************************************************************************************//**
     * Updates the main and timeline views.
     **********************************************************************************************/
    void updateMainAndTimelineView();

    /*******************************************************************************************//**
     * Updates just the main view.
     **********************************************************************************************/
    void updateMainView();

    /*******************************************************************************************//**
     * Updates just the timeline view.
     **********************************************************************************************/
    void updateTimelineView();

    /*******************************************************************************************//**
     * Shows the data plot for the current trials.
     **********************************************************************************************/
    void showDataPlotForTrials();

    /*******************************************************************************************//**
     * Shows the histogram for the current trials.
     **********************************************************************************************/
    void showHistogramForTrials();

    /*******************************************************************************************//**
     * Shows the scatterplot for the current trials.
     **********************************************************************************************/
    void showScatterplotForTrials();

    /*******************************************************************************************//**
     * Updates the fixation infos in the dock widget for the currently selected fixation and the
     * microsaccades list.
     *
     * @param fixation
     **********************************************************************************************/
    void updateFixationInfo(Fixation* fixation);

    /*******************************************************************************************//**
     * Returns a pointer to the currently selected fixation.
     **********************************************************************************************/
    Fixation* selectedFixation();

    /*******************************************************************************************//**
     * Returns for each trial a list of fixations - depending on plotElements (either all of them,
     * just the current fixation or all currently visible fixations) and the time limitation.
     *
     * @param trials       trials as data basis
     * @param plotElements element type for fixations (all, current, all visible)
     * @return for each trial a list of fixations
     **********************************************************************************************/
    std::vector<std::vector<Fixation*> > getFixationsForTrials(std::vector<Trial*>& trials,
                                                               PlotFixations plotElements);

    /*******************************************************************************************//**
     * Returns for each trial a list of all saccades - depending on the time limitation.
     *
     * @param trials       trials as data basis
     * @return for each trial a list of fixations
     **********************************************************************************************/
    std::vector<std::vector<Saccade*> > getSaccadesForTrial(std::vector<Trial *>& trials);

    /*******************************************************************************************//**
     * Returns a list of trials from the specified participants, with a correct trial id and with
     * a correct test condition name.
     *
     * @return list of trials with given properties
     **********************************************************************************************/
    std::vector<Trial*> getTrials(const std::set<int> participantIds,
                                  const std::set<QString> trialNames,
                                  const std::set<QString> testConditions);

    /*******************************************************************************************//**
     * Updates the UI after microsaccades have changed.
     **********************************************************************************************/
    void updateUI(bool updateTrials = true);

    /*******************************************************************************************//**
     * Adds data of the given microsaccade to the microsaccades table.
     **********************************************************************************************/
    void addToMicrosaccadesTable(Trial* trial, Fixation* f, Saccade* m, int index);

    /*******************************************************************************************//**
     * Updates the microsaccade count values in the fixaton table.
     **********************************************************************************************/
    void updateMicrosaccadesInFixationTable();

    /*******************************************************************************************//**
     * Updates the table which contains all fixations of the current trial.
     **********************************************************************************************/
    void updateFixationTable();

    /*******************************************************************************************//**
     * Exports all active views (stimulus, timeline, histogram, scatterplot, data plot).
     **********************************************************************************************/
    void exportAllViews();

    /*******************************************************************************************//**
     * Saves the visible sections of the views or the full scenes as PNG file.
     *
     * @param fullScene  if the full scene or visible section shall be exported
     * @param view       view which is used for rendering
     * @param filePath   file path where the file shall be saved
     * @param modifyPath if path is fixed of it it can be modified
     **********************************************************************************************/
    void startPNGExport(bool fullScene, QGraphicsView* view, QString filePath = QString(),
                        bool modifyPath = true);

    /*******************************************************************************************//**
     * Starts the png export for the given widget.
     *
     * @param widget     widget which is ised for rendering
     * @param filePath   file path where the file shall be saved
     * @param modifyPath modify given path witha recommended file name
     **********************************************************************************************/
    void startPNGExport(QWidget* widget, QString filePath = QString(), bool modifyPath = true);

    /*******************************************************************************************//**
     * Creates a screenshot for the visible scene (either timeline o main view) with the given
     * file path.
     *
     * @param viewObject widget from which a screenshot shall be taken
     * @param fielPath   file path fro the screenshot
     **********************************************************************************************/
    void screenshotSceneToPath(QWidget* viewObject, QString fielPath);

    /*******************************************************************************************//**
     * Creates a screenshot of the visible data plot with the given file path.
     *
     * @param fielPath file path fro the screenshot
     **********************************************************************************************/
    void screenshotDataPlotToPath(QString fielPath);

    /*******************************************************************************************//**
     * Returns a string containing all filter settings (used as metadata in png files).
     *
     * @return string containing filter settings
     **********************************************************************************************/
    QString getFilterSettingString() const;

    /*******************************************************************************************//**
     * Returns a string containing all data plot settings (used as metadata in png files).
     *
     * @return string containing data plot settings
     **********************************************************************************************/
    QString getPlotSettingString() const;

    /*******************************************************************************************//**
     * Returns a string containing all microsaccade detection settings (used as metadata in png
     * files).
     *
     * @return string containing microsaccade detection settings
     **********************************************************************************************/
    QString getMicrosaccadesDetectionSettingString() const;

    /*******************************************************************************************//**
     * Returns a string containing all fixation detection settings (used as metadata in png
     * files).
     *
     * @return string containing microsaccade detection settings
     **********************************************************************************************/
    QString getFixationDetectionSettingString() const;

    /*******************************************************************************************//**
     * Returns a string with the creation time for the settings string.
     *
     * @return string with the creation time for the settings string
     **********************************************************************************************/
    QString getCreationTimeString() const;

    /*******************************************************************************************//**
     * Transforms a date to a string in the following format:
     * dd.MM.yyyy hh:mm:ss
     *
     * @param date           date time
     * @param withSpace if the shall be space between the date and the time
     * @return date as string in the format dd.MM.yyyy hh:mm:ss
     **********************************************************************************************/
    QString dateToString(const QDateTime date, bool withSpace = true) const;

    /*******************************************************************************************//**
     * Shows a dialog to where the user can decide of files shall be added or if they shall
     * replace previous loaded files.
     **********************************************************************************************/
    int showFileImportDialog();

    /*******************************************************************************************//**
     * Opens a save file dialog and returns the file name.
     *
     * @param reason     window title of the dialog
     * @param fileTypes  visible file types for the dialog
     * @param recentFile file name/path which will be shown in the dialog initially
     * @return returns the file name from the file dialog
     **********************************************************************************************/
    QString getFileName(const QString& reason, const QString& fileTypes,
                        const QString& recentFile = "") const;

    /*******************************************************************************************//**
     * Creates and returns a vector which represents a sample. This sample is used for dtermining
     * fixations.
     *
     * @param time   time value of the sample
     * @param rightX x position of the reight eye
     * @param rightY y position of the reight eye
     * @return sample
     **********************************************************************************************/
    std::vector<qreal> createSample(const qreal time, const qreal rightX, const qreal rightY);

    /*******************************************************************************************//**
     * Checks if the given file path exists alrady and returns an alternative file path if it
     * already exists.
     *
     * @param path file path
     * @return non-existing file path
     **********************************************************************************************/
    QString newPath(const QString path);

private slots:

    /*******************************************************************************************//**
     * Opens a file which was drag and dropped. Removes loaded participants from the project.
     **********************************************************************************************/
    void openDragDropFiles();

    /*******************************************************************************************//**
     * Opens a file which was drag and dropped.
     **********************************************************************************************/
    void addDragDropFiles();

    /*******************************************************************************************//**
     * Shows an open file dialog and calls the method openFiles for processing.
     **********************************************************************************************/
    void open();

    /*******************************************************************************************//**
     * Shows an open file dialog and calls the method openFiles for processing.
     **********************************************************************************************/
    void add();

    /*******************************************************************************************//**
     * Loads a csv file in which test conditions are specified for participants and trials.
     **********************************************************************************************/
    void openCSV();

    /*******************************************************************************************//**
     * Loads a microsaccade detection settings file and updates the values in the UI.
     **********************************************************************************************/
    void loadMicrosaccadsSettingsFile();

    /*******************************************************************************************//**
     * Loads a fixation detection settings file and updates the values in the UI.
     **********************************************************************************************/
    void loadFixationSettingsFile();
    
    /*******************************************************************************************//**
     * Loads a a color settings file and updates the values in the UI.
     **********************************************************************************************/
    void loadColorSettingsFile();

    /*******************************************************************************************//**
     * Updates the microsaccades for the current trial.
     **********************************************************************************************/
    void updateCurrentMicrosaccadesDetection();

    /*******************************************************************************************//**
     * Updates the microsaccades for the current participant.
     **********************************************************************************************/
    void updateMicrosaccadesForParticipant();

    /*******************************************************************************************//**
     * Runs the microsaccade detection algorithm for all trials loaded to the application.
     **********************************************************************************************/
    void updateMicrosaccadesForAllDetection();

    /*******************************************************************************************//**
     * Updates/calculates the fixations for the current trial.
     **********************************************************************************************/
    void updateCurrentFixationsDetection();

    /*******************************************************************************************//**
     * Updates/calculates the fixations for the current participant.
     **********************************************************************************************/
    void updateFixationsForCurrentParticipantDetection();

    /*******************************************************************************************//**
     * Updates/calculates the fixations for all trials.
     **********************************************************************************************/
    void updateFixationsForAllDetection();

    /*******************************************************************************************//**
     * Resets the zoom of the main view.
     **********************************************************************************************/
    void resetMainViewZoom();

    /*******************************************************************************************//**
     * Zoom in for the main view.
     **********************************************************************************************/
    void zoomInMainView();

    /*******************************************************************************************//**
     * Zoom out for the main view.
     **********************************************************************************************/
    void zoomOutMainView();

    /*******************************************************************************************//**
     * Zoom in for the timeline view.
     **********************************************************************************************/
    void zoomInTimeline();

    /*******************************************************************************************//**
     * Zoom out for the timeline view.
     **********************************************************************************************/
    void zoomOutTimeline();

    /*******************************************************************************************//**
     * Resets the zoom of the timeline view.
     **********************************************************************************************/
    void resetTimelineZoom();

    /*******************************************************************************************//**
     * Updates the visible data in the views for all active trials in multi selection mode.
     **********************************************************************************************/
    void determineTrials();

    /*******************************************************************************************//**
     * Creates a screenshot for the visible view (either timeline o main view).
     *
     * @param viewObject widget from which a screenshot shall be taken
     **********************************************************************************************/
    void screenshotView(QWidget* viewObject);

    /*******************************************************************************************//**
     * Creates a screenshot for the visible scene (either timeline o main view).
     *
     * @param viewObject widget from which a screenshot shall be taken
     **********************************************************************************************/
    void screenshotScene(QWidget* viewObject);

    /*******************************************************************************************//**
     * Creates a screenshot of the visible data plot.
     **********************************************************************************************/
    void screenshotDataPlot();

    /*******************************************************************************************//**
     * Creates a screenshot of the visible histogram.
     **********************************************************************************************/
    void screenshotHistogram();

    /*******************************************************************************************//**
     * Creates a screenshot of the visible scatterplot.
     **********************************************************************************************/
    void screenshotScatterplot();

    /*******************************************************************************************//**
     * Updates the ui, ... when a fixation was selected.
     **********************************************************************************************/
    void fixationSelected(Fixation* fixation);

    /*******************************************************************************************//**
     * Updates the ui, ... when a fixation was selected.
     **********************************************************************************************/
    void fixationIndexSelected(int index);

    /*******************************************************************************************//**
     * Selects the fixation with the given index in the table.
     **********************************************************************************************/
    void selectFixationInTable(Fixation* fixation);

    /*******************************************************************************************//**
     * Selects the next fixation to the currently selected one.
     **********************************************************************************************/
    void selectNextFixation();

    /*******************************************************************************************//**
     * Selects the previous fixation to the currently selected one.
     **********************************************************************************************/
    void selectPreviousFixation();

    /*******************************************************************************************//**
     * Updates the currently visible participant.
     *
     * @param index new participant index
     **********************************************************************************************/
    void participantChanged(int index);

    /*******************************************************************************************//**
     * Updates the currently visible trial.
     *
     * @param index new trial index
     **********************************************************************************************/
    void trialChanged(int index);

    /*******************************************************************************************//**
     * Updates the gaze type which is used in the visalizations (left/right/averaged).
     *
     * @param index for gaze type
     **********************************************************************************************/
    void gazeTypeChanged(int index);

    /*******************************************************************************************//**
     * Updates the settings that microsaccades will be highlighted in the main view and updates the
     * view.
     *
     * @param show whether microsaccades shall be highlighted
     **********************************************************************************************/
    void showMicrosaccades(bool show);

    /*******************************************************************************************//**
     * Updates the settings that sample positions will be shown in the main view and updates the
     * view.
     *
     * @param show whether sample points shall be shown
     **********************************************************************************************/
    void showSamples(bool show);

    /*******************************************************************************************//**
     * Updates the settings that sample connections will be shown in the main view and updates the
     * view.
     *
     * @param show whether sample connections shall be shown
     **********************************************************************************************/
    void showSampleConnections(bool show);

    /*******************************************************************************************//**
     * Updates the settings that sample microsaccade direction lines will be shown in the main view
     * and updates the view.
     *
     * @param show whether microsaccade direction lines shall be shown
     **********************************************************************************************/
    void showMicrosaccadesDirection(bool show);

    /*******************************************************************************************//**
     * Updates the settings that fixation samples and connections will be highlighted in the main
     * view and updates the view.
     *
     * @param highlight whether fixation samples and connections shall be highlighted
     **********************************************************************************************/
    void highlightFixations(bool highlight);

    /*******************************************************************************************//**
     * Updates the settings that just for the current fixation samples and connections will be
     * highlighted in the main view and updates the view.
     * In order to show fixations for the current fixation it has to be enabled for fixations in
     * general with highlightFixations() as well.
     *
     * @param highlight whether for the current fixation samples and connections shall be highlighted
     **********************************************************************************************/
    void highlightCurrentFixation(bool highlight);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the saccade directions in the main view and updates the
     * view.
     *
     * @param show show/hide directions of saccades
     **********************************************************************************************/
    void showSaccadeDirections(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the scan path with fixations and saccades in the main
     * view and updates the view.
     *
     * @param show show/hide the scan path in the main view
     **********************************************************************************************/
    void showScanPath(bool show);

    /*******************************************************************************************//**
     * Updates the opacity of the stimulus.
     *
     * @param value new opacity value
     **********************************************************************************************/
    void backgroundOpacityChanged(int value);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the microsaccade areas in the timeline view and updates
     *  the view.
     *
     * @param show show/hide the fixation areas in the timeline view
     **********************************************************************************************/
    void showMicrosaccadesInTimeline(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the fixation areas in the timeline view and updates the
     * view.
     *
     * @param show show/hide the fixation areas in the timeline view
     **********************************************************************************************/
    void showFixationsInTimeline(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the event areas in the timeline view and updates the
     * view.
     *
     * @param show show/hide the event areas in the timeline view
     **********************************************************************************************/
    void showEventsInTimeline(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the x values in the timeline view and updates the view.
     *
     * @param show show/hide the x values in the timeline view
     **********************************************************************************************/
    void showXValuesInTimeline(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the y values in the timeline view and updates the view.
     *
     * @param show show/hide the y values in the timeline view
     **********************************************************************************************/
    void showYValuesInTimeline(bool show);

    /*******************************************************************************************//**
     * Updates the settings to show/hide the velocities in the timeline view and updates the view.
     *
     * @param show show/hide the velocity in the timeline view
     **********************************************************************************************/
    void showVelocityValuesInTimeline(bool show);

    /*******************************************************************************************//**
     * Shows all fixations or only the following ones to the selected fixation.
     *
     * @param show whether all fixations shall be visible or only following ones.
     **********************************************************************************************/
    void showNeighboringFixations(bool show);

    /*******************************************************************************************//**
     * Updates the value for the number of following fixations of the selected one which shall
     * be visible before and after the current fixation.
     *
     * @param value number of following fixations both before and after the current fixation
     **********************************************************************************************/
    void followingFixationsCountChanged(int value);

    /*******************************************************************************************//**
     * Updates the value for the number of previous fixations of the selected one which shall
     * be visible before and after the current fixation.
     *
     * @param value number of following fixations both before and after the current fixation
     **********************************************************************************************/
    void previousFixationsCountChanged(int value);

    /*******************************************************************************************//**
     * Changes the fixation radius depending on the selected mode (equal, by duration or by number
     * of microsaccades).
     *
     * @param index specifies the size mode
     **********************************************************************************************/
    void fixationSizeChanged(int index);

    /*******************************************************************************************//**
     * Changes the scale value for the size of fixations in the main view.
     *
     * @param value scale value
     **********************************************************************************************/
    void fixationScaleValueChanged(int value);

    /*******************************************************************************************//**
     * When activated timeline zoomes to the visible fixation areas (current fixation and neighbors).
     *
     * @param use activate this option
     **********************************************************************************************/
    void activateZoomToFixationArea(bool use);

    /*******************************************************************************************//**
     * Activates that a filter for the time range shall be used.
     *
     * @param use filter for time range shall be used
     **********************************************************************************************/
    void activateDataTimeLimit(bool use);

    /*******************************************************************************************//**
     * Updates the time range used when a filter for the time range shall be used
     **********************************************************************************************/
    void updateDataTimeLimit();

    /*******************************************************************************************//**
     * Sets the boundaries for the time range.
     *
     * @param min minimum index value
     * @param max maximum index value
     **********************************************************************************************/
    void limitTimeRangeToSelection(int min, int max);

    /*******************************************************************************************//**
     * Zooms the timeline to the specified time range.
     **********************************************************************************************/
    void zoomToTimeLimit();

    /*******************************************************************************************//**
     * Updates the participants and trials in the combobox.
     **********************************************************************************************/
    void addParticipantsInUI();

    /*******************************************************************************************//**
     * Updates the test conditions in the user interface.
     **********************************************************************************************/
    void addTestConditionsInUI();

    /*******************************************************************************************//**
     * Updates the microsaccade counts in the participants, trials and test conditions in the
     * combobox.
     **********************************************************************************************/
    void updateMicrosaccadeStatBarsInUI();

protected:

    /*******************************************************************************************//**
     * Show event: uses the settings stored in the close event to rememeber window
     * position/size, ...
     *
     * @param event
     **********************************************************************************************/
    virtual void showEvent(QShowEvent* event);

    /*******************************************************************************************//**
     * Close event: saves settings.
     *
     * @param event
     **********************************************************************************************/
    virtual void closeEvent(QCloseEvent* event);

    /*******************************************************************************************//**
     * Drag event: starts drag and drop for a file.
     *
     * @param e QDragEnterEvent
     **********************************************************************************************/
    void dragEnterEvent(QDragEnterEvent* e);

    /*******************************************************************************************//**
     * Drop event: imports the files into the application.
     *
     * @param e QDropEvent
     **********************************************************************************************/
    void dropEvent(QDropEvent* e);

private:

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// Window settings file path.
    QString m_settingsPath;

    /// Contains all eye tracking data.
    EyeTrackingData* m_eyeTrackingData;

    /// This object is responsible for the main visualization.
    MainVisualization* m_mainVisualization;

    /// This object is responsible for the timeline visualization.
    TimelineVisualization* m_timelineVisualization;

    /// Main view for gaze data visualization.
    MainView* m_mainView;

    /// Additional view for timeline visualization.
    TimelineView* m_timelineView;

    /// Scene for the Eye Tracking data.
    MainScene* m_mainScene;

    /// Scene for the timeline.
    TimelineScene* m_timelineScene;

    /// List of files from drag and drop operation.
    QStringList m_dragDropFiles;

    /// Creates the user interface and holds current settings.
    UserInterface* m_ui;
};

#endif
