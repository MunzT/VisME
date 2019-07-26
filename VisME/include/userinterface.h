/***********************************************************************************************//**
 * @author Tanja Munz
 * @file userinterface.h
 **************************************************************************************************/
#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "abstractplot.h"
#include "diagramlegend.h"
#include "eyedataexport.h"
#include "mainview.h"
#include "participant.h"
#include "settings.h"
#include "settingsexport.h"
#include "timelineview.h"
#include <cmath>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>
#include <QTableWidget>

class EyeDataExport;
class EyeHistogram;
class EyeScatterplot;
class MainWindow;
class SettingsExport;
class StatisticsExport;

/***********************************************************************************************//**
 * This class is responsible for creating and managing the user interface.
 **************************************************************************************************/
class UserInterface : public QObject
{
    // MainWindow and SettingsExport/-Import can use all functions and variables of this class
    friend class MainWindow;
    friend class SettingsExport;
    friend class SettingsImport;

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     *
     * @param mainWindow   main window
     * @param settings     settings containing parameter values for filtering, data plot and
     *                     microsaccade detection
     * @param mainView     mainView for main visualization
     * @param timelineView mainView for timeline visualization
     **********************************************************************************************/
    UserInterface(MainWindow* mainWindow, Settings* settings, MainView* mainView,
                  TimelineView* timelineView);

    /*******************************************************************************************//**
     * Destructor
     **********************************************************************************************/
    ~UserInterface();

    /*******************************************************************************************//**
     * Updates all color buttons from the settings.
     **********************************************************************************************/
    void updateColorButtons();

private:

    /*******************************************************************************************//**
     * Creates the menu of the main window with entries to open files, export data or images, ...
     **********************************************************************************************/
    void createMenu();

    /*******************************************************************************************//**
     * Creates the status line of the main window.
     **********************************************************************************************/
    void createStatusline();

    /*******************************************************************************************//**
     * Creates all dock widgets of the user interface
     * and sets the necessary settings for them.
     **********************************************************************************************/
    void createDockWidgets();

    /*******************************************************************************************//**
     * Creates the general dock widget that shows general information about the
     * data.
     **********************************************************************************************/
    void createGeneralInformationDock();

    /*******************************************************************************************//**
     * Creates the fixations dock widget with a table containing fixation information.
     **********************************************************************************************/
    void createFixationsDock();

    /*******************************************************************************************//**
     * Creates the microsaccades dock widget with a table containing microsaccade information.
     **********************************************************************************************/
    void createMicrosaccadesDock();

    /*******************************************************************************************//**
     * Creates the polar plot dock widget.
     **********************************************************************************************/
    void createPlotDock();

    /*******************************************************************************************//**
     * Creates the filter dock widget for the data.
     **********************************************************************************************/
    void createFilterDock();

    /*******************************************************************************************//**
     * Creates the filter dock widget for the views.
     **********************************************************************************************/
    void createViewDock();

    /*******************************************************************************************//**
     * Creates the filter dock widget for the microsaccade detection algorithm.
     **********************************************************************************************/
    void createMicrosaccadesDetectionDock();

    /*******************************************************************************************//**
     * Creates the filter dock widget for the fixation detection algorithm.
     **********************************************************************************************/
    void createFixationsDetectionDock();

    /*******************************************************************************************//**
     * Creates the dock widget for the colors used in the application.
     **********************************************************************************************/
    void createColorDock();

    /*******************************************************************************************//**
     * Creates the dock widget for further diagrams.
     **********************************************************************************************/
    void createDiagramDock();

    /*******************************************************************************************//**
     * Updates the given color button to the given color and updates the colors in the
     * visualizations.
     **********************************************************************************************/
    void updateColorButton(QColor* color, QPushButton* button);

    /*******************************************************************************************//**
     * Shows a color dialog.
     **********************************************************************************************/
    void showColorDialog();

    /*******************************************************************************************//**
     * Updates the polar plot.
     **********************************************************************************************/
    void updateDataPlot(const std::vector<Trial*> trials,
                        const std::vector<std::vector<Fixation*> > fixationsForTrials);

    /*******************************************************************************************//**
     * Changes the data plot in the UI to the given one.
     **********************************************************************************************/
    void newDataPlot(AbstractPlot* newPlot);

    /*******************************************************************************************//**
     * Updates the trials in the combobox when a participant was selected.
     *
     * @param participant  participant which will be selected
     **********************************************************************************************/
    void selectParticipant(Participant* participant);

    /*******************************************************************************************//**
     * Updates all parameters in the user interface from m_settings.
     **********************************************************************************************/
     void updateValuesFromSettings();

     /*******************************************************************************************//**
      * Updates all microsaccade detection parameters in the user interface from the given trial to
      * see the current settings of its microsaccades.
      **********************************************************************************************/
     void updateMicrosaccadeSettings(Trial& trial);

     /*******************************************************************************************//**
      * Updates all fixation detection parameters in the user interface from the given trial to
      * see the current settings of its fixations.
      **********************************************************************************************/
     void updateFixationsSettings(Trial& trial);

    /*******************************************************************************************//**
     * Returns a QHBoxLayout containing two buttons: "Check All" and "Uncheck All".
     *
     * @param list listWidget for which the buttons will be used
     * @return layout containing the buttons
     **********************************************************************************************/
    QHBoxLayout* getCheckButtons(QListWidget* list);

    /*******************************************************************************************//**
     * Changes the display mode between individual and multi trial/participant/test condition mode.
     **********************************************************************************************/
    void changeDisplayMode(bool single);

    /*******************************************************************************************//**
     * Shows a dialog with the given title and the text of the given text file.
     **********************************************************************************************/
    void showDialog(const QString title, const QString fileName);

private slots:

    /*******************************************************************************************//**
     * Shows the about dialog with information about author, date, ...
     **********************************************************************************************/
    void showAboutDialog();

    /*******************************************************************************************//**
     * Shows information about the input formats from a text file.
     **********************************************************************************************/
    void showInputFormats();

    /*******************************************************************************************//**
     * Shows information about the features of the application.
     **********************************************************************************************/
    void showFeaturesDialog();

    /*******************************************************************************************//**
     * Checks all entries in the given list.
     *
     * @param list has to be a QListWidget; each item will be (un)checked
     **********************************************************************************************/
    void checkAllListEntries(QWidget* list);

    /*******************************************************************************************//**
     * Unchecks all entries in the given list.
     *
     * @param list has to be a QListWidget; each item will be (un)checked
     **********************************************************************************************/
    void uncheckAllListEntries(QWidget* list);

    /*******************************************************************************************//**
     * Updates the test conditions in the combobox list in multi mode.
     **********************************************************************************************/
    void setTestConditionsInUI(std::set<QString> testConditions);

    /*******************************************************************************************//**
     * Hides/shows settings for the histogram according on other settings.
     **********************************************************************************************/
    void updateHistogramOptions();

private:

    /// Data exporter.
    EyeDataExport* m_dataExport;
    SettingsExport* m_settingsExport;
    StatisticsExport* m_statisticsExport;

    /// Main window.
    MainWindow* m_window;

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// View that shows the eye tacking data.
    MainView* m_mainView;

    /// View that shows the timeline.
    TimelineView* m_timelineView;

    // -- dock widgets -----------------------------------------------------------------------------

    /// Dock widget that shows general information about the data.
    QDockWidget* m_generalInformationDock;

    /// Dock widget that shows filter options for the visible data.
    QDockWidget* m_filterDock;

    /// Dock widget that shows filter options for the visible elements in the views.
    QDockWidget* m_viewDock;

    /// Dock widget which contains a table of all fixations.
    QDockWidget* m_fixationsDockWidget;

    /// Dock widget which contains a table of all microsaccades.
    QDockWidget* m_microsaccadesDockWidget;

    /// Dock widget which contains a polar plot.
    QDockWidget* m_dataPlotDockWidget;

    /// Dock widget for microsaccade detection settings.
    QDockWidget* m_microsaccadesDetectionDockWidget;

    /// Dock widget to detect fixations.
    QDockWidget* m_fixationsDetectionDockWidget;

    /// Dock widget for colors.
    QDockWidget* m_colorDockWidget;

    /// Dock widget for diagrams.
    QDockWidget* m_diagramDockWidget;

    // -- Appearance Settings ----------------------------------------------------------------------
    /// General labels.
    QLabel* m_screenResolutionLabel;
    QLabel* m_frequencyValueLabel;

    QLabel* m_inputSamplesNumberLabelMin;
    QLabel* m_inputSamplesNumberLabelMax;
    QLabel* m_inputSamplesNumberLabelMean;
    QLabel* m_inputSamplesNumberLabelMedian;
    QLabel* m_inputSamplesNumberLabelSum;

    QLabel* m_durationValueLabelMin;
    QLabel* m_durationValueLabelMax;
    QLabel* m_durationValueLabelMean;
    QLabel* m_durationValueLabelMedian;
    QLabel* m_durationValueLabelSum;

    QLabel* m_fixationCountLabelMin;
    QLabel* m_fixationCountLabelMax;
    QLabel* m_fixationCountLabelMean;
    QLabel* m_fixationCountLabelMedian;
    QLabel* m_fixationCountLabelSum;

    QLabel* m_fixationsWithMicrosaccadesCountLabelMin;
    QLabel* m_fixationsWithMicrosaccadesCountLabelMax;
    QLabel* m_fixationsWithMicrosaccadesCountLabelMean;
    QLabel* m_fixationsWithMicrosaccadesCountLabelMedian;
    QLabel* m_fixationsWithMicrosaccadesCountLabelSum;

    QLabel* m_fixationsWithMicrosaccadesRatioLabelMin;
    QLabel* m_fixationsWithMicrosaccadesRatioLabelMax;
    QLabel* m_fixationsWithMicrosaccadesRatioLabelMean;
    QLabel* m_fixationsWithMicrosaccadesRatioLabelMedian;

    QLabel* m_fixationDurationLabelMin;
    QLabel* m_fixationDurationLabelMax;
    QLabel* m_fixationDurationLabelMean;
    QLabel* m_fixationDurationLabelMedian;
    QLabel* m_fixationDurationLabelSum;

    QLabel* m_fixationDurationWithMSLabelMin;
    QLabel* m_fixationDurationWithMSLabelMax;
    QLabel* m_fixationDurationWithMSLabelMean;
    QLabel* m_fixationDurationWithMSLabelMedian;
    QLabel* m_fixationDurationWithMSLabelSum;

    QLabel* m_microsaccadesCountLabelMin;
    QLabel* m_microsaccadesCountLabelMax;
    QLabel* m_microsaccadesCountLabelMean;
    QLabel* m_microsaccadesCountLabelMedian;
    QLabel* m_microsaccadesCountLabelSum;

    QLabel* m_microsaccadesPerFixationLabelMin;
    QLabel* m_microsaccadesPerFixationLabelMax;
    QLabel* m_microsaccadesPerFixationLabelMean;
    QLabel* m_microsaccadesPerFixationLabelMedian;
    QLabel* m_microsaccadesPerFixationLabelSum;

    QLabel* m_microsaccadesPerFixationWithMSLabelMin;
    QLabel* m_microsaccadesPerFixationWithMSLabelMax;
    QLabel* m_microsaccadesPerFixationWithMSLabelMean;
    QLabel* m_microsaccadesPerFixationWithMSLabelMedian;
    QLabel* m_microsaccadesPerFixationWithMSLabelSum;

    QLabel* m_microsaccadesPerSecondLabelMin;
    QLabel* m_microsaccadesPerSecondLabelMax;
    QLabel* m_microsaccadesPerSecondLabelMean;
    QLabel* m_microsaccadesPerSecondLabelMedian;

    QLabel* m_microsaccadesPerSecondInFixationsLabelMin;
    QLabel* m_microsaccadesPerSecondInFixationsLabelMax;
    QLabel* m_microsaccadesPerSecondInFixationsLabelMean;
    QLabel* m_microsaccadesPerSecondInFixationsLabelMedian;

    QLabel* m_microsaccadeAmplitudeLabelMin;
    QLabel* m_microsaccadeAmplitudeLabelMax;
    QLabel* m_microsaccadeAmplitudeLabelMean;
    QLabel* m_microsaccadeAmplitudeLabelMedian;

    QLabel* m_microsaccadesIntersaccIntervalLabelMin;
    QLabel* m_microsaccadesIntersaccIntervalLabelMax;
    QLabel* m_microsaccadesIntersaccIntervalLabelMean;
    QLabel* m_microsaccadesIntersaccIntervalLabelMedian;

    QLabel* m_microsaccadeDurationLabelMin;
    QLabel* m_microsaccadeDurationLabelMax;
    QLabel* m_microsaccadeDurationLabelMean;
    QLabel* m_microsaccadeDurationLabelMedian;

    QLabel* m_microsaccadeVelocityLabelMin;
    QLabel* m_microsaccadeVelocityLabelMax;
    QLabel* m_microsaccadeVelocityLabelMean;
    QLabel* m_microsaccadeVelocityLabelMedian;

    QLabel* m_numberOfTrialsLabel;

    /// Microsaccade labels.
    QLabel* m_selectedFixationDurationLabel;
    QLabel* m_selectedFixationMicrosaccadesCountLabel;
    QLabel* m_selectedFixationStartTimeLabel;
    QLabel* m_selectedFixationStartIndexLabel;

    QLabel* m_selectedFixationXLabel;
    QLabel* m_selectedFixationYLabel;

    /// Group boxes
    QGroupBox* m_selectionGroupBox;

    /// Combo boxes
    QComboBox* m_participantCombobox;
    QComboBox* m_trialCombobox;

    /// Color buttons;
    QMap<QPushButton*, QColor*> buttonsToColorMapping;

    /// List widgets used in multi mode.
    QListWidget* m_participantList;
    QListWidget* m_trialList;
    QListWidget* m_testConditionList;

    /// Tables containing information on fixations/moicrosaccades.
    QTableWidget* m_fixationsTable;
    QTableWidget* m_microsaccadesTable;

    /// Data plot: either polar  plot or a regular plot.
    AbstractPlot *m_dataPlot;

    /// Parameters for data plot.
    QComboBox* m_dataPlotFixationsComboBox;
    QComboBox* m_dataPlotDataComboBox;
    QComboBox* m_dataPlotDirectionComboBox;
    QComboBox* m_dataPlotTypeComboBox;
    QComboBox* m_dataDirectionPlotVisTypeComboBox;
    QComboBox* m_dataScatterplotVisTypeComboBox;
    QSpinBox* m_dataPlotBinsSpinBox;
    QDoubleSpinBox* m_dataPlotRadiusSpinBox;
    QDoubleSpinBox* m_dataPlotRadiusSpinBox_2;
    QComboBox* m_aggregatePlotDataComboBox;
    QCheckBox* m_testConditionColorsCheckbox;

    QLabel* dataPlotRadiusLabel;
    QLabel* dataPlotRadiusLabel_2;
    QLabel* m_dataPlotGraphLabel;
    QLabel* m_dataPlotGraphLabel2;
    QLabel* m_dataPlotAggregationBinsValueLabel;
    QLabel* m_aggregatePlotLabel;

    /// Further plots
    QComboBox* m_dataComboBox;

    /// Histogram
    EyeHistogram *m_histogramPlot;

    /// Parameters for histogram
    QSpinBox* m_histogramStartTimeBinSizeSpinBox;
    QDoubleSpinBox* m_histogramPeakVelocityBinSizeSpinBox;
    QDoubleSpinBox* m_histogramAmplitudeBinSizeSpinBox;
    QSpinBox* m_histogramDurationBinSizeSpinBox;

    QLabel* m_histogramStartTimeBinsLabel;
    QLabel* m_histogramPeakVelocityBinsLabel;
    QLabel* m_histogramAmplitudeBinsLabel;
    QLabel* m_histogramDurationBinsLabel;

    QDoubleSpinBox* m_histogramMaxXSpinBox;
    QDoubleSpinBox* m_histogramMaxYSpinBox;
    QComboBox* m_histogramYAxisCombooBox;

    /// Scatterplot
    EyeScatterplot *m_scatterplot;

    /// Parameters for scatterplot
    QComboBox* m_scatterplotXAxisComboBox;
    QComboBox* m_scatterplotYAxisComboBox;
    QDoubleSpinBox* m_scatterplotMaxXSpinBox;
    QDoubleSpinBox* m_scatterplotMaxYSpinBox;
    QCheckBox* m_logarithmicScale;

    /// Layout for data plot.
    QVBoxLayout* m_dataPlotDockWidgetLayout;

    /// Legend for test conditions
    DiagramLegend* m_diagramsLegend;
    DiagramLegend* m_dataPlotLegend;

    /// Forms for single and multi mode.
    QWidget *m_formWidgetSingle;
    QWidget *m_formWidgetMulti;

    // -- Menu -------------------------------------------------------------------------------------

    // view menu
    /// View menu.
    QMenu* m_viewMenu;

    /// Parameters for main and timeline view.
    QSlider* m_backgroundOpacitySlider;
    QSpinBox* m_followingFixationsCountSpinBox;
    QCheckBox* m_showNeighboringFixationsCheckbox;
    QSpinBox* m_previousFixationsCountSpinBox;

    QComboBox* m_gazeDataComboBox;

    QCheckBox* m_showScanpathCheckBox;
    QCheckBox* m_showMicrosaccadesCheckbox;
    QCheckBox* m_showSamplesCheckbox;
    QCheckBox* m_showSampleConnectionsCheckbox;
    QCheckBox* m_showMicrosaccadeDirectionsCheckbox;
    QCheckBox* m_highlightFixationsCheckbox;
    QCheckBox* m_highlightCurrentFixationCheckbox;
    QCheckBox* m_showSaccadeDirectionsCheckbox;

    QCheckBox* m_showMicrosaccadesInTimelineCheckBox;
    QCheckBox* m_showFixationsInTimelineCheckBox;
    QCheckBox* m_showEventsInTimelineCheckBox;
    QCheckBox* m_showXValuesInTimelineCheckBox;
    QCheckBox* m_showYValuesInTimelineCheckBox;
    QCheckBox* m_showVelocityValuesInTimelineCheckBox;

    QComboBox* m_fixationSizeComboBox;
    QSlider* m_fixationSizeSlider;
    QRadioButton* m_radioSingle;
    QRadioButton* m_radioMulti;

    QGroupBox* m_viewGroupBoxMainView;
    QGroupBox* m_viewGroupBoxTimeline;
    QGroupBox* m_viewGroupBoxFixations;

    QPushButton* m_updateMicrosaccadesButton;
    QPushButton* m_updateCurrentParticipantMicrosaccadesButton;
    QPushButton* m_updateFixationsButton;
    QPushButton* m_updateFixationsForParticipantButton;

    QCheckBox* m_activateZoomToFixationAreaCheckBox;

    QCheckBox* m_timeLimitCheckBox;
    QPushButton* m_timeLimitZoomButton;
    QDoubleSpinBox* m_startTimeLimitSpinBox;
    QDoubleSpinBox* m_endTimeLimitSpinBox;

    /// Parameters for microsaccade detection algorithm.
    QCheckBox* m_microsaccadesFromInputFileCheckbox;
    QDoubleSpinBox* m_msVelocityThresholdSpinBox;
    QSpinBox* m_msMinDurationSpinBox;
    QCheckBox* m_msBinocularCheckBox;
    QSpinBox* m_msVelocityWindowSizeSpinBox;
    QSpinBox* m_msMaxDurationSpinBox;
    QDoubleSpinBox* m_msMaxAmplitudeSpinBox;
    QDoubleSpinBox* m_msMinAmplitudeSpinBox;
    QSpinBox* m_msMinIntersaccIntervalSpinBox;
    QDoubleSpinBox* m_msMinVelocitySpinBox;
    QDoubleSpinBox* m_msMaxVelocitySpinBox;
    QSpinBox* m_msIgnoreAtFixationStartSpinBox;
    QSpinBox* m_msIgnoreAtFixationEndSpinBox;
    QSpinBox* m_msIgnoreBeforeMissingDataSpinBox;
    QSpinBox* m_msIgnoreAfterMissingDataSpinBox;

    QCheckBox* m_msMaxDurationCheckBox;
    QCheckBox* m_msMinAmplitudeCheckBox;
    QCheckBox* m_msMaxAmplitudeCheckBox;
    QCheckBox* m_msMinIntersaccIntervalCheckBox;
    QCheckBox* m_msMinVelocityCheckBox;
    QCheckBox* m_msMaxVelocityCheckBox;
    QCheckBox* m_msIgnoreAtFixationStartCheckBox;
    QCheckBox* m_msIgnoreAtFixationEndCheckBox;
    QCheckBox* m_msIgnoreBeforeMissingDataCheckBox;
    QCheckBox* m_msIgnoreAfterMissingDataCheckBox;

    /// Parameters for saccade/fixation detection.
    QCheckBox* m_fixationsFromInputFileCheckbox;
    QDoubleSpinBox* m_fixVelocityThresholdSpinBox;
    QSpinBox* m_fixMinSaccadeDurationSpinBox;
    QCheckBox* m_fixBinocularCheckBox;
    QSpinBox* m_fixMaxDurationSpinBox;
    QSpinBox* m_fixVelocityWindowSizeSpinBox;
    QDoubleSpinBox* m_fixMaxSaccadeAmplitudeSpinBox;
    QDoubleSpinBox* m_fixMinSaccadeAmplitudeSpinBox;
    QSpinBox* m_fixMinSaccadeIntersaccIntervalSpinBox;
    QDoubleSpinBox* m_fixMinVelocitySpinBox;
    QDoubleSpinBox* m_fixMaxVelocitySpinBox;
    QSpinBox* m_fixIgnoreAtStartSpinBox;
    QSpinBox* m_fixIgnoreAtEndSpinBox;
    QSpinBox* m_fixIgnoreBeforeMissingDataSpinBox;
    QSpinBox* m_fixIgnoreAfterMissingDataSpinBox;

    QCheckBox* m_fixMaxDurationCheckBox;
    QCheckBox* m_fixMinAmplitudeCheckBox;
    QCheckBox* m_fixMaxAmplitudeCheckBox;
    QCheckBox* m_fixMinIntersaccIntervalCheckBox;
    QCheckBox* m_fixMinVelocityCheckBox;
    QCheckBox* m_fixMaxVelocityCheckBox;
    QCheckBox* m_fixIgnoreAtFixationStartCheckBox;
    QCheckBox* m_fixIgnoreAtFixationEndCheckBox;
    QCheckBox* m_fixIgnoreBeforeMissingDataCheckBox;
    QCheckBox* m_fixIgnoreAfterMissingDataCheckBox;

};

#endif // USERINTERFACE_H
