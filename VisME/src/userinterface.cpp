/***********************************************************************************************//**
 * @author Tanja Munz
 * @file userinterface.cpp
 **************************************************************************************************/
#define _USE_MATH_DEFINES
#include "colorgenerator.h"
#include "eyedirectionhistogram.h"
#include "eyegazeplot.h"
#include "eyehistogram.h"
#include "eyehistogram.h"
#include "eyescatterplot.h"
#include "itemdelegate.h"
#include "mainwindow.h"
#include "polarplot.h"
#include "radialscatterplot.h"
#include "roseplot.h"
#include "statisticsexport.h"
#include "timelineview.h"
#include "userinterface.h"
#include <cmath>
#include <QApplication>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QFontDatabase>
#include <QFormLayout>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QTextStream>


UserInterface::UserInterface(MainWindow* mainWindow, Settings* settings,
                             MainView* mainView, TimelineView* timelineView) :
    QObject(mainWindow),
    m_window(mainWindow),
    m_settings(settings),
    m_mainView(mainView),
    m_timelineView(timelineView),
    m_dataPlot(nullptr)
{

    m_dataExport = new EyeDataExport(m_window, m_window->m_eyeTrackingData, m_settings);
    m_settingsExport = new SettingsExport(m_window, m_settings);
    m_statisticsExport = new StatisticsExport(m_window, m_window->m_eyeTrackingData, m_settings);

    // the main window contains two main views
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(m_mainView);
    splitter->addWidget(m_timelineView);
    m_window->setCentralWidget(splitter);

    // size policies for the views
    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sizePolicy.setVerticalStretch(3);
    QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sizePolicy2.setVerticalStretch(1);
    mainView->setSizePolicy(sizePolicy);
    timelineView->setSizePolicy(sizePolicy2);

    // create the menu
    createMenu();

    // create the status line
    createStatusline();

    // create and show the dock widgets with some possibilities for interaction,
    // settings and information on the right side of the window and set values
    createDockWidgets();

    // Initialize values
    updateValuesFromSettings();
}


UserInterface::~UserInterface()
{
    delete m_dataExport;
    delete m_settingsExport;
    delete m_dataPlot;
}


void UserInterface::updateColorButtons()
{
    for (auto key : buttonsToColorMapping.keys() )
    {
        updateColorButton(buttonsToColorMapping.value(key), key);
    }
}


void UserInterface::createMenu()
{
    // file ****************************************************************************************
    QMenu* fileMenu = m_window->menuBar()->addMenu(tr("&File"));

    // -- open file --------------------------------------------------------------------------------
    QAction* openAction = new QAction(tr("&Open"), m_window);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Load eye tracking files."));
    fileMenu->addAction(openAction);
    QObject::connect(openAction, &QAction::triggered,
                     m_window, &MainWindow::open);

    QAction* addFileAction = new QAction(tr("&Add Files"), m_window);
    addFileAction->setShortcut(tr("Ctrl+A"));
    addFileAction->setStatusTip(tr("Add eye tracking file."));
    fileMenu->addAction(addFileAction);
    QObject::connect(addFileAction, &QAction::triggered,
                     m_window, &MainWindow::add);

    QAction* openTestConditionsFileAction = new QAction(tr("&Open Trial Test Conditions File"), m_window);
    openTestConditionsFileAction->setShortcut(tr("Ctrl+Shift+O"));
    openTestConditionsFileAction->setStatusTip(tr("Open trial test conditions file."));
    fileMenu->addAction(openTestConditionsFileAction);
    QObject::connect(openTestConditionsFileAction, &QAction::triggered,
                     m_window, &MainWindow::openCSV);

    QAction* loadMSAction = new QAction(tr("&Load Microsaccades Parameters"), m_window);
    loadMSAction->setStatusTip(tr("Load microsaccades parameter settings files."));
    fileMenu->addAction(loadMSAction);
    QObject::connect(loadMSAction, &QAction::triggered,
                     m_window, &MainWindow::loadMicrosaccadsSettingsFile);

    QAction* loadFixAction = new QAction(tr("&Load Fixation Filter Parameters"), m_window);
    loadFixAction->setStatusTip(tr("Load fixation filter parameter settings files."));
    fileMenu->addAction(loadFixAction);
    QObject::connect(loadFixAction, &QAction::triggered,
                     m_window, &MainWindow::loadFixationSettingsFile);

    QAction* loadColorsAction = new QAction(tr("&Load Color Settings"), m_window);
    loadColorsAction->setStatusTip(tr("Load color settings files."));
    fileMenu->addAction(loadColorsAction);
    QObject::connect(loadColorsAction, &QAction::triggered,
                     m_window, &MainWindow::loadColorSettingsFile);

    fileMenu->addSeparator();

    // -- export data ------------------------------------------------------------------------------

    QMenu* exportDataMenu = fileMenu->addMenu(tr("Export Data"));

    QAction* exportData1 = new
            QAction(tr("&Data for current Trial"), m_window);
    exportData1->setStatusTip(
                tr("Export data for current Trial (gaze data, fixations and microsaccades)."));
    exportDataMenu->addAction(exportData1);
    QObject::connect(exportData1, &QAction::triggered,
                     m_dataExport, &EyeDataExport::exportTrialDataToFile);

    QAction* exportData2 = new
            QAction(tr("Data for current Participant"), m_window);
    exportData2->setStatusTip(
                tr("Export data for current Participant (gaze data, fixations and microsaccades)."));
    exportDataMenu->addAction(exportData2);
    QObject::connect(exportData2, &QAction::triggered,
                     m_dataExport, &EyeDataExport::exportCurrentParticipantDataToFile);

    QAction* exportData3 = new
            QAction(tr("Data for all Participants"), m_window);
    exportData3->setShortcut(tr("Ctrl+D"));
    exportData3->setStatusTip(
                tr("Export data for all participants (gaze data, fixations and microsaccades)."));
    exportDataMenu->addAction(exportData3);
    QObject::connect(exportData3, &QAction::triggered,
                     m_dataExport, &EyeDataExport::exportAllParticipantDataToFile);

    // -- export data 2 ----------------------------------------------------------------------------

    exportDataMenu->addSeparator();

    QAction* exportData7 = new
            QAction(tr("Aggregated Data"), m_window);
    exportData7->setStatusTip(
                tr("Export aggregated data for all participants/test conditions."));
    exportDataMenu->addAction(exportData7);
    QObject::connect(exportData7, &QAction::triggered,
                     m_statisticsExport, &StatisticsExport::exportDataToFile);

    QAction* exportData8 = new
            QAction(tr("Aggregated Directional Data"), m_window);
    exportData8->setStatusTip(
                tr("Export aggregated directional data for all participants/test conditions."));
    exportDataMenu->addAction(exportData8);
    QObject::connect(exportData8, &QAction::triggered,
                     m_statisticsExport, &StatisticsExport::exportDirectionalDataToFile);

    // -- export data 3 ----------------------------------------------------------------------------

    exportDataMenu->addSeparator();

    QAction* exportData4 = new
            QAction(tr("Currently Used Microsaccades Parameters"), m_window);
    exportData4->setStatusTip(
                tr("Export the current microsaccades parameters from the UI to a file."));
    exportDataMenu->addAction(exportData4);
    QObject::connect(exportData4, &QAction::triggered,
                     m_settingsExport, &SettingsExport::exportMicroseccadesSettingsToFile);

    QAction* exportData5 = new
            QAction(tr("Currently Used Fixation Parameters"), m_window);
    exportData5->setStatusTip(
                tr("Export the current fixation parameters from the UI to a file."));
    exportDataMenu->addAction(exportData5);
    QObject::connect(exportData5, &QAction::triggered,
                     m_settingsExport, &SettingsExport::exportFixationSettingsToFile);

    QAction* exportData6 = new
            QAction(tr("Color Settings"), m_window);
    exportData6->setStatusTip(
                tr("Export the currently used color settings to a file."));
    exportDataMenu->addAction(exportData6);
    QObject::connect(exportData6, &QAction::triggered,
                     m_settingsExport, &SettingsExport::exportColorSettingsToFile);

    // -- export in PNG format ---------------------------------------------------------------------

    QMenu* exportViewMenu = fileMenu->addMenu(tr("Export Views"));

    QAction* exportPNG8 = new
            QAction(tr("All View as &PNG file"), m_window);
    exportPNG8->setStatusTip(
                tr("Export the all views as PNG file."));
    exportViewMenu->addAction(exportPNG8);
    connect(exportPNG8, &QAction::triggered, m_window, &MainWindow::exportAllViews);

    QAction* exportPNG = new
            QAction(tr("Stimulus View as &PNG file"), m_window);
    exportPNG->setShortcut(tr("Ctrl+Shift+E"));
    exportPNG->setStatusTip(
                tr("Export the stimulus view as PNG file."));
    exportViewMenu->addAction(exportPNG);
    connect(exportPNG, &QAction::triggered, [this](){ m_window->screenshotView(m_mainView); });

    QAction* exportPNG2 = new
            QAction(tr("Stimulus Scene as P&NG file"), m_window);
    exportPNG2->setShortcut(tr("Ctrl+E"));
    exportPNG2->setStatusTip(
                tr("Export the stimulus scene (eye tracking screen) as PNG file."));
    exportViewMenu->addAction(exportPNG2);
    connect(exportPNG2, &QAction::triggered, [this](){ m_window->screenshotScene(m_mainView); });

    QAction* exportPNG3 = new
            QAction(tr("Timeline View as PNG file"), m_window);
    exportPNG3->setStatusTip(
                tr("Export the data plot view as PNG file."));
    exportViewMenu->addAction(exportPNG3);
    connect(exportPNG3, &QAction::triggered, [this](){ m_window->screenshotView(m_timelineView); });

    QAction* exportPNG4 = new
            QAction(tr("Timeline Scene as PNG file"), m_window);
    exportPNG4->setStatusTip(
                tr("Export the entire data plot scene as PNG file."));
    exportViewMenu->addAction(exportPNG4);
    connect(exportPNG4, &QAction::triggered, [this](){ m_window->screenshotScene(m_timelineView); });

    QAction* exportPNG5 = new
            QAction(tr("Data Plot as PNG file"), m_window);
    exportPNG5->setStatusTip(
                tr("Export the data plot view as PNG file."));
    exportViewMenu->addAction(exportPNG5);
    QObject::connect(exportPNG5, &QAction::triggered, m_window, &MainWindow::screenshotDataPlot);

    QAction* exportPNG6 = new
            QAction(tr("Histogram as PNG file"), m_window);
    exportPNG6->setStatusTip(
                tr("Export the histogram view as PNG file."));
    exportViewMenu->addAction(exportPNG6);
    QObject::connect(exportPNG6, &QAction::triggered, m_window, &MainWindow::screenshotHistogram);

    QAction* exportPNG7 = new
            QAction(tr("Scatterplot as PNG file"), m_window);
    exportPNG7->setStatusTip(
                tr("Export the scatterplot view as PNG file."));
    exportViewMenu->addAction(exportPNG7);
    QObject::connect(exportPNG7, &QAction::triggered, m_window, &MainWindow::screenshotScatterplot);

    exportViewMenu->addSeparator();

    // -- exit -------------------------------------------------------------------------------------
    QAction* quitAction = new QAction(tr("&Exit"), m_window);
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setStatusTip(tr("Exit the program."));
    fileMenu->addAction(quitAction);
    QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    // view ****************************************************************************************
    m_viewMenu = m_window->menuBar()->addMenu(tr("&View"));

    // -- zoom -------------------------------------------------------------------------------------
    QMenu* zoomMenu = m_viewMenu->addMenu(tr("Zoom"));

    // -- zoom in
    QAction* zoomInAction = new QAction(tr("Timeline zoom &in"), m_window);
    zoomInAction->setStatusTip(tr("Timeline zoom in."));
    zoomInAction->setShortcut(tr("Ctrl++"));
    zoomMenu->addAction(zoomInAction);
    QObject::connect(zoomInAction, &QAction::triggered,
                     m_window, &MainWindow::zoomInMainView);

    // -- zoom out
    QAction* zoomOutAction = new QAction(tr("Timeline zoom &out"), m_window);
    zoomOutAction->setStatusTip(tr("Timeline zoom out."));
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    zoomMenu->addAction(zoomOutAction);
    QObject::connect(zoomOutAction, &QAction::triggered,
                     m_window, &MainWindow::zoomOutMainView);

    // -- zoom in
    QAction* zoomInTimelineAction = new QAction(tr("Timeline zoom &in"), m_window);
    zoomInTimelineAction->setStatusTip(tr("Timeline zoom in."));
    zoomInTimelineAction->setShortcut(tr("Ctrl+Alt++"));
    zoomMenu->addAction(zoomInTimelineAction);
    QObject::connect(zoomInTimelineAction, &QAction::triggered,
                     m_window, &MainWindow::zoomInTimeline);

    // -- zoom out
    QAction* zoomOutTimelineAction = new QAction(tr("Timeline zoom &out"), m_window);
    zoomOutTimelineAction->setStatusTip(tr("Timeline zoom out."));
    zoomOutTimelineAction->setShortcut(tr("Ctrl+Alt+-"));
    zoomMenu->addAction(zoomOutTimelineAction);
    QObject::connect(zoomOutTimelineAction, &QAction::triggered,
                     m_window, &MainWindow::zoomOutTimeline);

    zoomMenu->addSeparator();

    // -- normal view
    QAction* zoomResetAction = new QAction(tr("&Reset Stimulus View"), m_window);
    zoomResetAction->setStatusTip(tr("Reset the stimulus view."));
    zoomResetAction->setShortcut(tr("Ctrl+0"));
    zoomMenu->addAction(zoomResetAction);
    QObject::connect(zoomResetAction, &QAction::triggered,
                     m_window, &MainWindow::resetMainViewZoom);

    // -- timeline view
    QAction* zoomTimelineResetAction = new QAction(tr("&Reset Timeline View"), m_window);
    zoomTimelineResetAction->setStatusTip(tr("Reset the timeline view."));
    zoomTimelineResetAction->setShortcut(tr("Ctrl+Alt+0"));
    zoomMenu->addAction(zoomTimelineResetAction);
    QObject::connect(zoomTimelineResetAction, &QAction::triggered,
                     m_window, &MainWindow::resetTimelineZoom);

    zoomMenu->addSeparator();

    m_viewMenu->addSeparator();

    // info ****************************************************************************************
    QMenu* infoMenu = m_window->menuBar()->addMenu(tr("&Info"));

    // -- about ------------------------------------------------------------------------------------
    QAction* aboutAction = new QAction(tr("&About"), m_window);
    aboutAction->setStatusTip(tr("Show information about the program."));
    infoMenu->addAction(aboutAction);
    QObject::connect(aboutAction, &QAction::triggered,
                     this, &UserInterface::showAboutDialog);

    // -- input files ------------------------------------------------------------------------------
    QAction* inputAction = new QAction(tr("&Input"), m_window);
    inputAction->setStatusTip(tr("Show information about the input formats."));
    infoMenu->addAction(inputAction);
    QObject::connect(inputAction, &QAction::triggered,
                     this, &UserInterface::showInputFormats);

    // -- features file ----------------------------------------------------------------------------
    QAction* featuresAction = new QAction(tr("&Features"), m_window);
    featuresAction->setStatusTip(tr("Show information about available features of the application."));
    infoMenu->addAction(featuresAction);
    QObject::connect(featuresAction, &QAction::triggered,
                     this, &UserInterface::showFeaturesDialog);
}


void UserInterface::createStatusline()
{
    QLabel* statusLabel = new QLabel();
    m_window->statusBar()->addWidget(statusLabel);
}


void UserInterface::createDockWidgets()
{
    m_window->setDockOptions(QMainWindow::AllowNestedDocks |
                             QMainWindow::AllowTabbedDocks);

    // create the dock widgets
    createGeneralInformationDock();
    createFilterDock();
    createViewDock();
    createMicrosaccadesDetectionDock();
    createFixationsDetectionDock();
    createPlotDock();
    createDiagramDock();
    createFixationsDock();
    createMicrosaccadesDock();
    createColorDock();

    // list of all dock widgets
    QVector<QDockWidget*> dockWidgetList;
    dockWidgetList << m_generalInformationDock
                   << m_filterDock
                   << m_viewDock
                   << m_microsaccadesDetectionDockWidget
                   << m_fixationsDetectionDockWidget
                   << m_dataPlotDockWidget
                   << m_diagramDockWidget
                   << m_microsaccadesDockWidget
                   << m_fixationsDockWidget
                   << m_colorDockWidget;

    // dock widgets are arranged vertically to the overview dock widget
    //m_window->splitDockWidget(m_generalInformationDock, otherDock,
    //                          Qt::Vertical);

    // add features to all dock widgets
    for (int i = 0; i < dockWidgetList.size(); i++)
    {
        // add the dock widgets to the window
        m_window->addDockWidget(Qt::RightDockWidgetArea, dockWidgetList[i]);

        // option to close/open the dock widgets with a menu entry
        dockWidgetList[i]->setFeatures(QDockWidget::DockWidgetClosable|
                                       QDockWidget::DockWidgetMovable |
                                       QDockWidget::DockWidgetFloatable);
        m_viewMenu->addAction(dockWidgetList[i]->toggleViewAction());

        // the widgets are allowed to be placed on the right and left side of
        // the window
        dockWidgetList[i]->setAllowedAreas(Qt::LeftDockWidgetArea |
                                           Qt::RightDockWidgetArea);

        //if (i > 0)
        //{
        //    // dock widgets are tabbed
        //    m_window->tabifyDockWidget(dockWidgetList[i - 1], dockWidgetList[i]);
        //}

        // show the dock widget
        dockWidgetList[i]->show();
    }

    m_window->splitDockWidget(m_generalInformationDock, m_dataPlotDockWidget,
                              Qt::Horizontal);
    m_window->splitDockWidget(m_generalInformationDock, m_microsaccadesDetectionDockWidget,
                              Qt::Vertical);

    m_window->tabifyDockWidget(m_generalInformationDock, m_filterDock);
    m_window->tabifyDockWidget(m_filterDock, m_viewDock);
    m_window->tabifyDockWidget(m_viewDock, m_colorDockWidget);

    m_window->tabifyDockWidget(m_microsaccadesDetectionDockWidget, m_fixationsDetectionDockWidget);
    m_window->tabifyDockWidget(m_fixationsDetectionDockWidget, m_microsaccadesDockWidget);
    m_window->tabifyDockWidget(m_microsaccadesDockWidget, m_fixationsDockWidget);

    m_window->tabifyDockWidget(m_dataPlotDockWidget, m_diagramDockWidget);

    // show the general information dock widget on top
    //m_generalInformationDock->raise();
    m_filterDock->raise();
    m_dataPlotDockWidget->raise();
    m_microsaccadesDetectionDockWidget->raise();

    // tab positions
    m_window->setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
    m_window->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
}


void UserInterface::createGeneralInformationDock()
{

    QScrollArea* scrollArea = new QScrollArea;

    // general information:
    QLabel* generalInformationTitle = new QLabel(tr("<b>Data</b>"), m_window);

    QLabel* numberOfTrialsLabel = new QLabel(tr("Number of Trials: "), m_window);
    numberOfTrialsLabel->setToolTip(tr("Number of currently selected trials."));
    m_numberOfTrialsLabel = new QLabel(tr("-"), m_window);

    QLabel* frequencyLabel = new QLabel(tr("Frequency: "), m_window);
    frequencyLabel->setToolTip(tr("Frequency of all currently selected trials - if they are different multiple values are given."));
    m_frequencyValueLabel = new QLabel(tr("-"), m_window);

    QLabel* screenResolutionLabel = new QLabel(tr("Screen Resolution: "), m_window);
    screenResolutionLabel->setToolTip(tr("Frequency resolution of all currently selected trials - if they are different multiple values are given."));
    m_screenResolutionLabel = new QLabel(tr("-"), m_window);

    QGridLayout* generalInformationLayout = new QGridLayout();
    generalInformationLayout->addWidget(generalInformationTitle, 0, 0, 1, 2,
                                        Qt::AlignCenter);

    generalInformationLayout->addWidget(numberOfTrialsLabel, 1, 0);
    generalInformationLayout->addWidget(m_numberOfTrialsLabel, 1, 1);

    generalInformationLayout->addWidget(screenResolutionLabel, 2, 0);
    generalInformationLayout->addWidget(m_screenResolutionLabel, 2, 1);

    generalInformationLayout->addWidget(frequencyLabel, 3, 0);
    generalInformationLayout->addWidget(m_frequencyValueLabel, 3, 1);

    // group box
    QGroupBox* generalGroupBox = new QGroupBox(m_window);
    generalGroupBox->setLayout(generalInformationLayout);

    // Statistics
    QLabel* statisticsTitle = new QLabel(tr("<b>Statistics</b>"), m_window);

    QLabel* headerLabel = new QLabel(tr(""), m_window);
    QLabel* headerLabelMin = new QLabel(tr("Min"), m_window);
    QLabel* headerLabelMax = new QLabel(tr("Max"), m_window);
    QLabel* headerLabelMean = new QLabel(tr("Mean"), m_window);
    QLabel* headerLabelMedian = new QLabel(tr("Median"), m_window);
    QLabel* headerLabelSum = new QLabel(tr("Sum"), m_window);

    QLabel* inputSamplesLabel = new QLabel(tr("#Raw Data Samples in Trials: "), m_window);
    inputSamplesLabel->setToolTip(tr("Number of raw data samples of all currently selected trials."));
    m_inputSamplesNumberLabelMin = new QLabel(tr("-"), m_window);
    m_inputSamplesNumberLabelMax = new QLabel(tr("-"), m_window);
    m_inputSamplesNumberLabelMean = new QLabel(tr("-"), m_window);
    m_inputSamplesNumberLabelMedian = new QLabel(tr("-"), m_window);
    m_inputSamplesNumberLabelSum = new QLabel(tr("-"), m_window);

    QLabel* durationLabel = new QLabel(tr("Duration of Trials [s]: "), m_window);
    durationLabel->setToolTip(tr("Duration of all currently selected trials."));
    m_durationValueLabelMin = new QLabel(tr("-"), m_window);
    m_durationValueLabelMax = new QLabel(tr("-"), m_window);
    m_durationValueLabelMean = new QLabel(tr("-"), m_window);
    m_durationValueLabelMedian = new QLabel(tr("-"), m_window);
    m_durationValueLabelSum = new QLabel(tr("-"), m_window);

    QLabel* fixationLabel = new QLabel(tr("#Fixations in Trials: "), m_window);
    fixationLabel->setToolTip(tr("Fixation count of all currently selected trials."));
    m_fixationCountLabelMin = new QLabel(tr("-"), m_window);
    m_fixationCountLabelMax = new QLabel(tr("-"), m_window);
    m_fixationCountLabelMean = new QLabel(tr("-"), m_window);
    m_fixationCountLabelMedian = new QLabel(tr("-"), m_window);
    m_fixationCountLabelSum = new QLabel(tr("-"), m_window);

    QLabel* fixationsWithMicrosaccadesLabel = new QLabel(tr("#Fixations with Microsaccades in Trials: "), m_window);
    fixationsWithMicrosaccadesLabel->setToolTip(tr("Number of fixations with microsaccades of all currently selected trials."));
    m_fixationsWithMicrosaccadesCountLabelMin = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesCountLabelMax = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesCountLabelMean = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesCountLabelMedian = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesCountLabelSum = new QLabel(tr("-"), m_window);

    QLabel* fixationsWithMicrosaccadesRatioLabel = new QLabel(tr("Fixations containing Microsaccades [%]: "), m_window);
    fixationsWithMicrosaccadesRatioLabel->setToolTip(tr("Ratio between all fixations and the ones containing microsaccades of all currently selected trials."));
    m_fixationsWithMicrosaccadesRatioLabelMin = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesRatioLabelMax = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesRatioLabelMean = new QLabel(tr("-"), m_window);
    m_fixationsWithMicrosaccadesRatioLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* fixationDurationLabel = new QLabel(tr("Fixation Duration [s]: "), m_window);
    fixationDurationLabel->setToolTip(tr("Fixation duration of all fixations of the currently selected trials."));
    m_fixationDurationLabelMin = new QLabel(tr("-"), m_window);
    m_fixationDurationLabelMax = new QLabel(tr("-"), m_window);
    m_fixationDurationLabelMean = new QLabel(tr("-"), m_window);
    m_fixationDurationLabelMedian = new QLabel(tr("-"), m_window);
    m_fixationDurationLabelSum = new QLabel(tr("-"), m_window);

    QLabel* fixationDurationWithMSLabel = new QLabel(tr("Fixation (with Microsaccades) Duration [s]: "), m_window);
    fixationDurationWithMSLabel->setToolTip(tr("Fixation duration of all fixations containing microsaccade of the currently selected trials."));
    m_fixationDurationWithMSLabelMin = new QLabel(tr("-"), m_window);
    m_fixationDurationWithMSLabelMax = new QLabel(tr("-"), m_window);
    m_fixationDurationWithMSLabelMean = new QLabel(tr("-"), m_window);
    m_fixationDurationWithMSLabelMedian = new QLabel(tr("-"), m_window);
    m_fixationDurationWithMSLabelSum = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesLabel = new QLabel(tr("#Microsaccades in Trials: "), m_window);
    microsaccadesLabel->setToolTip(tr("Microsaccades count of all currently selected trials."));
    m_microsaccadesCountLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesCountLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesCountLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesCountLabelMedian = new QLabel(tr("-"), m_window);
    m_microsaccadesCountLabelSum = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesPerFixationLabel = new QLabel(tr("#Microsaccades per Fixation: "), m_window);
    microsaccadesPerFixationLabel->setToolTip(tr("Microsaccades per Fixation; considering all fixation available."));
    m_microsaccadesPerFixationLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationLabelMedian = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationLabelSum = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesPerFixationWithMSLabel = new QLabel(tr("#Microsaccades per Fixation with Microsaccades: "), m_window);
    microsaccadesPerFixationWithMSLabel->setToolTip(tr("Microsaccades per Fixation; considering only fixations with microsaccades."));
    m_microsaccadesPerFixationWithMSLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationWithMSLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationWithMSLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationWithMSLabelMedian = new QLabel(tr("-"), m_window);
    m_microsaccadesPerFixationWithMSLabelSum = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesPerSecondLabel = new QLabel(tr("#Microsaccades per Second (for Trials): "), m_window);
    microsaccadesPerSecondLabel->setToolTip(tr("Uses the number of all microsaccades and the time of whole trials for all currently selected trials."));
    m_microsaccadesPerSecondLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesPerSecondInFixationsLabel = new QLabel(tr("#Microsaccades per Second in Fixations (for Trials): "), m_window);
    microsaccadesPerSecondInFixationsLabel->setToolTip(tr("Uses the number of all microsaccades and the time of fixations of a trial for all currently selected trials."));
    m_microsaccadesPerSecondInFixationsLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondInFixationsLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondInFixationsLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesPerSecondInFixationsLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* microsaccadeAmplitudeLabel = new QLabel(tr("Microsaccade Amplitude [°]: "), m_window);
    microsaccadeAmplitudeLabel->setToolTip(tr("Microsaccades amplitude of all currently selected trials"));
    m_microsaccadeAmplitudeLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadeAmplitudeLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadeAmplitudeLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadeAmplitudeLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* microsaccadesIntersaccIntervalLabel = new QLabel(tr("Inter-saccadic Interval [ms]: "), m_window);
    microsaccadesIntersaccIntervalLabel->setToolTip(tr("Inter-saccadic interval of microsaccades within fixations of all currently selected trials."));
    m_microsaccadesIntersaccIntervalLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadesIntersaccIntervalLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadesIntersaccIntervalLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadesIntersaccIntervalLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* microsaccadeDurationLabel = new QLabel(tr("Microsaccade Duration [ms]: "), m_window);
    microsaccadeDurationLabel->setToolTip(tr("Microsaccade duration of all currently selected trials."));
    m_microsaccadeDurationLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadeDurationLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadeDurationLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadeDurationLabelMedian = new QLabel(tr("-"), m_window);

    QLabel* microsaccadeVelocityLabel = new QLabel(tr("Microsaccade Peak Velocity [°/s]: "), m_window);
    microsaccadeVelocityLabel->setToolTip(tr("Microsaccade velocity of all currently selected trials."));
    m_microsaccadeVelocityLabelMin = new QLabel(tr("-"), m_window);
    m_microsaccadeVelocityLabelMax = new QLabel(tr("-"), m_window);
    m_microsaccadeVelocityLabelMean = new QLabel(tr("-"), m_window);
    m_microsaccadeVelocityLabelMedian = new QLabel(tr("-"), m_window);

    QGridLayout* statisticsLayout = new QGridLayout();
    statisticsLayout->addWidget(statisticsTitle, 0, 0, 1, 6,
                                Qt::AlignCenter);

    statisticsLayout->addWidget(headerLabel, 1, 0);
    statisticsLayout->addWidget(headerLabelMin, 1, 1);
    statisticsLayout->addWidget(headerLabelMax, 1, 2);
    statisticsLayout->addWidget(headerLabelMean, 1, 3);
    statisticsLayout->addWidget(headerLabelMedian, 1, 4);
    statisticsLayout->addWidget(headerLabelSum, 1, 5);

    statisticsLayout->addWidget(inputSamplesLabel, 2, 0);
    statisticsLayout->addWidget(m_inputSamplesNumberLabelMin, 2, 1);
    statisticsLayout->addWidget(m_inputSamplesNumberLabelMax, 2, 2);
    statisticsLayout->addWidget(m_inputSamplesNumberLabelMean, 2, 3);
    statisticsLayout->addWidget(m_inputSamplesNumberLabelMedian, 2, 4);
    statisticsLayout->addWidget(m_inputSamplesNumberLabelSum, 2, 5);

    statisticsLayout->addWidget(durationLabel, 3, 0);
    statisticsLayout->addWidget(m_durationValueLabelMin, 3, 1);
    statisticsLayout->addWidget(m_durationValueLabelMax, 3, 2);
    statisticsLayout->addWidget(m_durationValueLabelMean, 3, 3);
    statisticsLayout->addWidget(m_durationValueLabelMedian, 3, 4);
    statisticsLayout->addWidget(m_durationValueLabelSum, 3, 5);

    statisticsLayout->addWidget(fixationLabel, 4, 0);
    statisticsLayout->addWidget(m_fixationCountLabelMin, 4, 1);
    statisticsLayout->addWidget(m_fixationCountLabelMax, 4, 2);
    statisticsLayout->addWidget(m_fixationCountLabelMean, 4, 3);
    statisticsLayout->addWidget(m_fixationCountLabelMedian, 4, 4);
    statisticsLayout->addWidget(m_fixationCountLabelSum, 4, 5);

    statisticsLayout->addWidget(fixationsWithMicrosaccadesLabel, 5, 0);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesCountLabelMin, 5, 1);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesCountLabelMax, 5, 2);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesCountLabelMean, 5, 3);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesCountLabelMedian, 5, 4);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesCountLabelSum, 5, 5);

    statisticsLayout->addWidget(fixationsWithMicrosaccadesRatioLabel, 6, 0);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesRatioLabelMin, 6, 1);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesRatioLabelMax, 6, 2);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesRatioLabelMean, 6, 3);
    statisticsLayout->addWidget(m_fixationsWithMicrosaccadesRatioLabelMedian, 6, 4);

    statisticsLayout->addWidget(fixationDurationLabel, 7, 0);
    statisticsLayout->addWidget(m_fixationDurationLabelMin, 7, 1);
    statisticsLayout->addWidget(m_fixationDurationLabelMax, 7, 2);
    statisticsLayout->addWidget(m_fixationDurationLabelMean, 7, 3);
    statisticsLayout->addWidget(m_fixationDurationLabelMedian, 7, 4);
    statisticsLayout->addWidget(m_fixationDurationLabelSum, 7, 5);

    statisticsLayout->addWidget(fixationDurationWithMSLabel, 8, 0);
    statisticsLayout->addWidget(m_fixationDurationWithMSLabelMin, 8, 1);
    statisticsLayout->addWidget(m_fixationDurationWithMSLabelMax, 8, 2);
    statisticsLayout->addWidget(m_fixationDurationWithMSLabelMean, 8, 3);
    statisticsLayout->addWidget(m_fixationDurationWithMSLabelMedian, 8, 4);
    statisticsLayout->addWidget(m_fixationDurationWithMSLabelSum, 8, 5);

    statisticsLayout->addWidget(microsaccadesLabel, 9, 0);
    statisticsLayout->addWidget(m_microsaccadesCountLabelMin, 9, 1);
    statisticsLayout->addWidget(m_microsaccadesCountLabelMax, 9, 2);
    statisticsLayout->addWidget(m_microsaccadesCountLabelMean, 9, 3);
    statisticsLayout->addWidget(m_microsaccadesCountLabelMedian, 9, 4);
    statisticsLayout->addWidget(m_microsaccadesCountLabelSum, 9, 5);

    statisticsLayout->addWidget(microsaccadesPerFixationLabel, 10, 0);
    statisticsLayout->addWidget(m_microsaccadesPerFixationLabelMin, 10, 1);
    statisticsLayout->addWidget(m_microsaccadesPerFixationLabelMax, 10, 2);
    statisticsLayout->addWidget(m_microsaccadesPerFixationLabelMean, 10, 3);
    statisticsLayout->addWidget(m_microsaccadesPerFixationLabelMedian, 10, 4);
    statisticsLayout->addWidget(m_microsaccadesPerFixationLabelSum, 10, 5);

    statisticsLayout->addWidget(microsaccadesPerFixationWithMSLabel, 11, 0);
    statisticsLayout->addWidget(m_microsaccadesPerFixationWithMSLabelMin, 11, 1);
    statisticsLayout->addWidget(m_microsaccadesPerFixationWithMSLabelMax, 11, 2);
    statisticsLayout->addWidget(m_microsaccadesPerFixationWithMSLabelMean, 11, 3);
    statisticsLayout->addWidget(m_microsaccadesPerFixationWithMSLabelMedian, 11, 4);
    statisticsLayout->addWidget(m_microsaccadesPerFixationWithMSLabelSum, 11, 5);

    statisticsLayout->addWidget(microsaccadesPerSecondLabel, 12, 0);
    statisticsLayout->addWidget(m_microsaccadesPerSecondLabelMin, 12, 1);
    statisticsLayout->addWidget(m_microsaccadesPerSecondLabelMax, 12, 2);
    statisticsLayout->addWidget(m_microsaccadesPerSecondLabelMean, 12, 3);
    statisticsLayout->addWidget(m_microsaccadesPerSecondLabelMedian, 12, 4);

    statisticsLayout->addWidget(microsaccadesPerSecondInFixationsLabel, 13, 0);
    statisticsLayout->addWidget(m_microsaccadesPerSecondInFixationsLabelMin, 13, 1);
    statisticsLayout->addWidget(m_microsaccadesPerSecondInFixationsLabelMax, 13, 2);
    statisticsLayout->addWidget(m_microsaccadesPerSecondInFixationsLabelMean, 13, 3);
    statisticsLayout->addWidget(m_microsaccadesPerSecondInFixationsLabelMedian, 13, 4);

    statisticsLayout->addWidget(microsaccadeAmplitudeLabel, 14, 0);
    statisticsLayout->addWidget(m_microsaccadeAmplitudeLabelMin, 14, 1);
    statisticsLayout->addWidget(m_microsaccadeAmplitudeLabelMax, 14, 2);
    statisticsLayout->addWidget(m_microsaccadeAmplitudeLabelMean, 14, 3);
    statisticsLayout->addWidget(m_microsaccadeAmplitudeLabelMedian, 14, 4);

    statisticsLayout->addWidget(microsaccadesIntersaccIntervalLabel, 15, 0);
    statisticsLayout->addWidget(m_microsaccadesIntersaccIntervalLabelMin, 15, 1);
    statisticsLayout->addWidget(m_microsaccadesIntersaccIntervalLabelMax, 15, 2);
    statisticsLayout->addWidget(m_microsaccadesIntersaccIntervalLabelMean, 15, 3);
    statisticsLayout->addWidget(m_microsaccadesIntersaccIntervalLabelMedian, 15, 4);

    statisticsLayout->addWidget(microsaccadeDurationLabel, 16, 0);
    statisticsLayout->addWidget(m_microsaccadeDurationLabelMin, 16, 1);
    statisticsLayout->addWidget(m_microsaccadeDurationLabelMax, 16, 2);
    statisticsLayout->addWidget(m_microsaccadeDurationLabelMean, 16, 3);
    statisticsLayout->addWidget(m_microsaccadeDurationLabelMedian, 16, 4);

    statisticsLayout->addWidget(microsaccadeVelocityLabel, 17, 0);
    statisticsLayout->addWidget(m_microsaccadeVelocityLabelMin, 17, 1);
    statisticsLayout->addWidget(m_microsaccadeVelocityLabelMax, 17, 2);
    statisticsLayout->addWidget(m_microsaccadeVelocityLabelMean, 17, 3);
    statisticsLayout->addWidget(m_microsaccadeVelocityLabelMedian, 17, 4);

    // group box
    QGroupBox* statisticsGroupBox = new QGroupBox(m_window);
    statisticsGroupBox->setLayout(statisticsLayout);

    // general information:
    QLabel* selectionInformationTitle = new QLabel(tr("<b>Selected Fixation</b>"), m_window);

    QLabel* selectedFixationDurationLabel = new QLabel(tr("Duration: "), m_window);
    selectedFixationDurationLabel->setToolTip(tr("Fixation duration."));
    m_selectedFixationDurationLabel = new QLabel(tr("0"), m_window);

    QLabel* selectedFixationMicrosaccadesLabel = new QLabel(tr("#Microsaccades: "), m_window);
    selectedFixationMicrosaccadesLabel->setToolTip(tr("Number of microsaccades in fixation."));
    m_selectedFixationMicrosaccadesCountLabel = new QLabel(tr("0"), m_window);

    QLabel* selectedFixationStartTimeLabel = new QLabel(tr("Start Time: "), m_window);
    selectedFixationStartTimeLabel->setToolTip(tr("Start time of fixation."));
    m_selectedFixationStartTimeLabel = new QLabel(tr("0"), m_window);

    QLabel* selectedFixationStartIndexLabel = new QLabel(tr("Start Index: "), m_window);
    selectedFixationStartIndexLabel->setToolTip(tr("Start index of fixation."));
    m_selectedFixationStartIndexLabel = new QLabel(tr("0"), m_window);

    QLabel* selectedFixationXLabel = new QLabel(tr("x: "), m_window);
    selectedFixationXLabel->setToolTip(tr("x value of fixation."));
    m_selectedFixationXLabel = new QLabel(tr("0"), m_window);

    QLabel* selectedFixationYLabel = new QLabel(tr("y: "), m_window);
    selectedFixationYLabel->setToolTip(tr("y value of fixation.."));
    m_selectedFixationYLabel = new QLabel(tr("0"), m_window);

    QGridLayout* selectionInformationLayout = new QGridLayout;
    selectionInformationLayout->addWidget(selectionInformationTitle, 0, 0, 1, 2,
                                          Qt::AlignCenter);

    selectionInformationLayout->addWidget(selectedFixationStartTimeLabel, 1, 0);
    selectionInformationLayout->addWidget(m_selectedFixationStartTimeLabel, 1, 1);

    selectionInformationLayout->addWidget(selectedFixationStartIndexLabel, 2, 0);
    selectionInformationLayout->addWidget(m_selectedFixationStartIndexLabel, 2, 1);

    selectionInformationLayout->addWidget(selectedFixationDurationLabel, 3, 0);
    selectionInformationLayout->addWidget(m_selectedFixationDurationLabel, 3, 1);

    selectionInformationLayout->addWidget(selectedFixationMicrosaccadesLabel, 4, 0);
    selectionInformationLayout->addWidget(m_selectedFixationMicrosaccadesCountLabel, 4, 1);

    selectionInformationLayout->addWidget(selectedFixationXLabel, 5, 0);
    selectionInformationLayout->addWidget(m_selectedFixationXLabel, 5, 1);

    selectionInformationLayout->addWidget(selectedFixationYLabel, 6, 0);
    selectionInformationLayout->addWidget(m_selectedFixationYLabel, 6, 1);

    // group box
    m_selectionGroupBox = new QGroupBox(m_window);
    m_selectionGroupBox->setLayout(selectionInformationLayout);
    m_selectionGroupBox->hide();

    // layout of the dock widget
    QVBoxLayout* generalLayout = new QVBoxLayout();
    // add the widgets to the layout
    generalLayout->addWidget(generalGroupBox);
    generalLayout->addWidget(statisticsGroupBox);
    generalLayout->addWidget(m_selectionGroupBox);
    generalLayout->addStretch();

    // dock
    QWidget* generalWidget = new QWidget(m_window);
    generalWidget->setLayout(generalLayout);

    scrollArea->setWidget(generalWidget);
    scrollArea->setWidgetResizable(true);

    m_generalInformationDock = new QDockWidget(tr("General"), m_window);
    m_generalInformationDock->setWidget(scrollArea);

    m_generalInformationDock->setObjectName(tr("GeneralDockWidget"));
}


void UserInterface::createFixationsDock()
{
    m_fixationsTable = new QTableWidget(m_window);
    m_fixationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fixationsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fixationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QObject::connect(m_fixationsTable, &QTableWidget::currentCellChanged,
                     m_window, &MainWindow::fixationIndexSelected);

    QVBoxLayout* fixationDockWidgetLayout = new QVBoxLayout();

    fixationDockWidgetLayout->addWidget(m_fixationsTable);

    // dock
    QWidget* fixationWidget = new QWidget(m_window);
    fixationWidget->setLayout(fixationDockWidgetLayout);

    m_fixationsDockWidget = new QDockWidget(tr("Fixations"), m_window);
    m_fixationsDockWidget->setWidget(fixationWidget);

    m_fixationsDockWidget->setObjectName(tr("FixationsDockWidget"));
}


void UserInterface::createMicrosaccadesDock()
{
    m_microsaccadesTable = new QTableWidget(m_window);
    m_microsaccadesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_microsaccadesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_microsaccadesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout* microsaccadeDockWidgetLayout = new QVBoxLayout();

    microsaccadeDockWidgetLayout->addWidget(m_microsaccadesTable);

    // dock
    QWidget* microsaccadeWidget = new QWidget(m_window);
    microsaccadeWidget->setLayout(microsaccadeDockWidgetLayout);

    m_microsaccadesDockWidget = new QDockWidget(tr("Microsaccades"), m_window);
    m_microsaccadesDockWidget->setWidget(microsaccadeWidget);

    m_microsaccadesDockWidget->setObjectName(tr("MicrosaccadesDockWidget"));
}


void UserInterface::createPlotDock()
{
    QScrollArea* scrollArea = new QScrollArea;
    m_dataPlotDockWidgetLayout = new QVBoxLayout();

    // settings
    QGridLayout* gridLayout = new QGridLayout();

    QLabel* dataPlotFixationsLabel = new QLabel(tr("Fixations"), m_window);
    m_dataPlotFixationsComboBox = new QComboBox();
    m_dataPlotFixationsComboBox->addItem(tr("All"));
    m_dataPlotFixationsComboBox->addItem(tr("Current"));
    m_dataPlotFixationsComboBox->addItem(tr("Visible"));
    m_dataPlotFixationsComboBox->setCurrentIndex(0);
    QObject::connect(m_dataPlotFixationsComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    QLabel* dataPlotDataLabel = new QLabel(tr("Data Values"), m_window);
    m_dataPlotDataComboBox = new QComboBox(m_window);
    m_dataPlotDataComboBox->addItem(tr("Microsaccades"));
    m_dataPlotDataComboBox->addItem(tr("Saccades (Fixation Connections)"));
    m_dataPlotDataComboBox->addItem(tr("Gaze Movement (Samples of Fixations)"));
    m_dataPlotDataComboBox->setCurrentIndex(0);
    QObject::connect(m_dataPlotDataComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    QLabel* dataPlotDirectionLabel = new QLabel(tr("Direction"), m_window);
    m_dataPlotDirectionComboBox = new QComboBox(m_window);
    m_dataPlotDirectionComboBox->addItem(tr("Screen Coordinate System"));
    m_dataPlotDirectionComboBox->addItem(tr("To Next Fixation"));
    m_dataPlotDirectionComboBox->addItem(tr("To Previous Fixation"));
    m_dataPlotDirectionComboBox->setCurrentIndex(0);
    QObject::connect(m_dataPlotDirectionComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    QLabel* dataPlotTypeLabel = new QLabel(tr("Type"), m_window);
    m_dataPlotTypeComboBox = new QComboBox(m_window);
    m_dataPlotTypeComboBox->addItem(tr("Direction Counts"));
    m_dataPlotTypeComboBox->addItem(tr("Directional Length"));
    m_dataPlotTypeComboBox->addItem(tr("Movement in Relation to Fixation Center"));
    m_dataPlotTypeComboBox->setCurrentIndex(0);
    QObject::connect(m_dataPlotTypeComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    m_dataPlotGraphLabel = new QLabel(tr("Graph"), m_window);

    m_dataDirectionPlotVisTypeComboBox = new QComboBox(m_window);
    m_dataDirectionPlotVisTypeComboBox->addItem(tr("Polar Plot"));
    //m_dataDirectionPlotVisTypeComboBox->addItem(tr("Polar Plot with Hole"));
    //m_dataDirectionPlotVisTypeComboBox->addItem(tr("Rose Plot"));
    m_dataDirectionPlotVisTypeComboBox->addItem(tr("Rose Plot with Hole"));
    //m_dataDirectionPlotVisTypeComboBox->addItem(tr("Rose Rect Plot"));
    //m_dataDirectionPlotVisTypeComboBox->addItem(tr("Rose Rect Plot with Hole"));
    //m_dataDirectionPlotVisTypeComboBox->addItem(tr("Histogram"));
    QObject::connect(m_dataDirectionPlotVisTypeComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    m_dataPlotGraphLabel2 = new QLabel(tr("Graph"), m_window);
    m_dataScatterplotVisTypeComboBox = new QComboBox(m_window);
    m_dataScatterplotVisTypeComboBox->addItem(tr("Scatterplot"));
    m_dataScatterplotVisTypeComboBox->addItem(tr("Connected Scatterplot"));
    QObject::connect(m_dataScatterplotVisTypeComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    m_dataPlotAggregationBinsValueLabel = new QLabel(tr("Aggregation Bins:"), m_window);
    m_dataPlotBinsSpinBox = new QSpinBox(m_window);
    m_dataPlotBinsSpinBox->setMinimum(3);
    m_dataPlotBinsSpinBox->setMaximum(360);
    QObject::connect(m_dataPlotBinsSpinBox,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    dataPlotRadiusLabel = new QLabel(tr("Plot Radius (count/value):"), m_window);
    dataPlotRadiusLabel->setToolTip(tr("Use 0 to fit data in graph."));
    m_dataPlotRadiusSpinBox = new QDoubleSpinBox(m_window);
    m_dataPlotRadiusSpinBox->setMinimum(0);
    m_dataPlotRadiusSpinBox->setMaximum(1000);
    m_dataPlotRadiusSpinBox->setSingleStep(0.1);
    QObject::connect(m_dataPlotRadiusSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    dataPlotRadiusLabel_2 = new QLabel(tr("Marker value (°):"), m_window);
    dataPlotRadiusLabel_2->setToolTip(tr("Use 0 to fit data in graph."));
    m_dataPlotRadiusSpinBox_2 = new QDoubleSpinBox(m_window);
    m_dataPlotRadiusSpinBox_2->setMinimum(0);
    m_dataPlotRadiusSpinBox_2->setMaximum(1000);
    m_dataPlotRadiusSpinBox_2->setDecimals(2);
    m_dataPlotRadiusSpinBox_2->setSingleStep(0.1);
    QObject::connect(m_dataPlotRadiusSpinBox_2,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showDataPlotForTrials);
    m_dataPlotRadiusSpinBox_2->hide();
    dataPlotRadiusLabel_2->hide();

    m_aggregatePlotLabel = new QLabel(tr("Aggregation"), m_window);
    m_aggregatePlotDataComboBox = new QComboBox(m_window);
    m_aggregatePlotDataComboBox->addItem("No Aggregation");
    m_aggregatePlotDataComboBox->addItem("Aggregate All");
    m_aggregatePlotDataComboBox->addItem("Aggregate Test Conditions");
    m_aggregatePlotDataComboBox->addItem("Aggregate Trials");
    m_aggregatePlotDataComboBox->addItem("Aggregate Participants");
    QObject::connect(m_aggregatePlotDataComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showDataPlotForTrials);

    m_testConditionColorsCheckbox = new QCheckBox(tr("Use Test Condition Colors (if Available)"), m_window);
    m_testConditionColorsCheckbox->setCheckable(true);
    QObject::connect(m_testConditionColorsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showDataPlotForTrials);

    gridLayout->addWidget(dataPlotFixationsLabel, 0, 0);
    gridLayout->addWidget(m_dataPlotFixationsComboBox, 0, 1);
    gridLayout->addWidget(dataPlotDataLabel, 1, 0);
    gridLayout->addWidget(m_dataPlotDataComboBox, 1, 1);
    gridLayout->addWidget(dataPlotDirectionLabel, 2, 0);
    gridLayout->addWidget(m_dataPlotDirectionComboBox, 2, 1);
    gridLayout->addWidget(dataPlotTypeLabel, 3, 0);
    gridLayout->addWidget(m_dataPlotTypeComboBox, 3, 1);
    gridLayout->addWidget(m_dataPlotGraphLabel, 4, 0);
    gridLayout->addWidget(m_dataDirectionPlotVisTypeComboBox, 4, 1);
    gridLayout->addWidget(m_dataPlotGraphLabel2, 5, 0);
    gridLayout->addWidget(m_dataScatterplotVisTypeComboBox, 5, 1);
    gridLayout->addWidget(m_dataPlotAggregationBinsValueLabel, 6, 0);
    gridLayout->addWidget(m_dataPlotBinsSpinBox, 6, 1);
    gridLayout->addWidget(dataPlotRadiusLabel, 7, 0);
    gridLayout->addWidget(m_dataPlotRadiusSpinBox, 7, 1);
    gridLayout->addWidget(dataPlotRadiusLabel_2, 8, 0);
    gridLayout->addWidget(m_dataPlotRadiusSpinBox_2, 8, 1);
    gridLayout->addWidget(m_aggregatePlotLabel, 9, 0);
    gridLayout->addWidget(m_aggregatePlotDataComboBox, 9, 1);
    gridLayout->addWidget(m_testConditionColorsCheckbox, 10, 0, 1, 2);

    m_dataScatterplotVisTypeComboBox->hide();
    m_dataPlotGraphLabel2->hide();
    m_dataDirectionPlotVisTypeComboBox->show();
    m_dataPlotGraphLabel->show();

    m_dataPlotDockWidgetLayout->addLayout(gridLayout);

    // legend
    m_dataPlotLegend = new DiagramLegend(m_settings);
    m_dataPlotLegend->hide();
    m_dataPlotDockWidgetLayout->addWidget(m_dataPlotLegend);

    // plot
    newDataPlot(new PolarPlot(m_settings, false));

    // dock
    QWidget* dataPlotWidget = new QWidget(m_window);
    dataPlotWidget->setLayout(m_dataPlotDockWidgetLayout);

    scrollArea->setWidget(dataPlotWidget);
    scrollArea->setWidgetResizable(true);

    m_dataPlotDockWidget = new QDockWidget(tr("Data Plot"), m_window);
    m_dataPlotDockWidget->setWidget(scrollArea);

    m_dataPlotDockWidget->setObjectName(tr("DataPlotDockWidget"));
}


void UserInterface::createFilterDock()
{
    QScrollArea* scrollArea = new QScrollArea;

    // general information:
    QLabel* filterTitleData = new QLabel(tr("<b>Data</b>"), m_window);
    QLabel* gazeDataTitle = new QLabel(tr("<b>Eye Data</b>"), m_window);
    QLabel* timeLimitTitle = new QLabel(tr("<b>TimeLimit</b>"), m_window);

    QLabel* participantLabel = new QLabel(tr("Participant:"), m_window);
    QLabel* participantsLabel = new QLabel(tr("Participants:"), m_window);
    QLabel* trialLabel = new QLabel(tr("Trial:"), m_window);
    QLabel* trialsLabel = new QLabel(tr("Trials:"), m_window);
    QLabel* testConditionsLabel = new QLabel(tr("Test Conditions:"), m_window);

    m_gazeDataComboBox = new QComboBox(m_window);
    m_gazeDataComboBox->addItem(tr("Right Eye"));
    m_gazeDataComboBox->addItem(tr("Left Eye"));
    m_gazeDataComboBox->addItem(tr("Averaged Eye Values"));

    m_participantCombobox = new QComboBox(m_window);
    m_trialCombobox = new QComboBox(m_window);

    QHBoxLayout* timeLimitValueLayout1 = new QHBoxLayout();
    QHBoxLayout* timeLimitValueLayout2 = new QHBoxLayout();

    m_timeLimitCheckBox= new QCheckBox("Limit Visible Time Range [ms]:", m_window);
    m_timeLimitZoomButton= new QPushButton("Zoom to Time Range", m_window);
    m_startTimeLimitSpinBox = new QDoubleSpinBox(m_window);
    m_startTimeLimitSpinBox->setMinimumWidth(100);
    m_startTimeLimitSpinBox->setMaximum(std::numeric_limits<int>::max());
    m_endTimeLimitSpinBox = new QDoubleSpinBox(m_window);
    m_endTimeLimitSpinBox->setMinimumWidth(100);
    m_endTimeLimitSpinBox->setMaximum(std::numeric_limits<int>::max());

    QObject::connect(m_participantCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::participantChanged);
    QObject::connect(m_trialCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::trialChanged);

    QObject::connect(m_gazeDataComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::gazeTypeChanged);

    QObject::connect(m_timeLimitCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::activateDataTimeLimit);
    QObject::connect(m_startTimeLimitSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::updateDataTimeLimit);
    QObject::connect(m_endTimeLimitSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::updateDataTimeLimit);
    QObject::connect(m_timeLimitZoomButton, &QCheckBox::clicked,
                     m_window, &MainWindow::zoomToTimeLimit);

    QVBoxLayout* filterLayout = new QVBoxLayout();
    QVBoxLayout* eyeDataLayout = new QVBoxLayout();
    QVBoxLayout* timeLimitLayout = new QVBoxLayout();

    filterLayout->addWidget(filterTitleData);

    QGroupBox* modeGroupBox = new QGroupBox(m_window);

    m_radioSingle = new QRadioButton(tr("Trial"), m_window);
    m_radioMulti = new QRadioButton(tr("Group"), m_window);

    QObject::connect(m_radioSingle, &QCheckBox::toggled,
                     this, &UserInterface::changeDisplayMode);
    QObject::connect(m_radioSingle, &QRadioButton::toggled,
                     m_window, &MainWindow::showData);

    QHBoxLayout* modeBox = new QHBoxLayout();
    modeBox->addWidget(m_radioSingle);
    modeBox->addWidget(m_radioMulti);
    modeBox->addStretch(1);
    modeGroupBox->setLayout(modeBox);
    modeGroupBox->setFlat(true);

    filterLayout->addWidget(modeGroupBox);

    m_formWidgetSingle = new QWidget(m_window);
    QFormLayout* formLayoutSingle = new QFormLayout();
    formLayoutSingle->addRow(participantLabel, m_participantCombobox);
    formLayoutSingle->addRow(trialLabel, m_trialCombobox);
    m_formWidgetSingle->setLayout(formLayoutSingle);
    m_formWidgetSingle->hide();
    filterLayout->addWidget(m_formWidgetSingle);

    m_formWidgetMulti = new QWidget(m_window);
    QVBoxLayout* layoutMulti = new QVBoxLayout();
    QFormLayout* formLayoutMulti = new QFormLayout();

    m_participantList = new QListWidget(m_window);
    m_trialList = new QListWidget(m_window);
    m_testConditionList = new QListWidget(m_window);

    m_participantList->setItemDelegate(new ItemDelegate());
    m_trialList->setItemDelegate(new ItemDelegate());
    m_testConditionList->setItemDelegate(new ItemDelegate());

    formLayoutMulti->addRow(participantsLabel, m_participantList);
    formLayoutMulti->addRow(new QLabel(m_window), getCheckButtons(m_participantList));
    formLayoutMulti->addRow(trialsLabel, m_trialList);
    formLayoutMulti->addRow(new QLabel(m_window), getCheckButtons(m_trialList));
    formLayoutMulti->addRow(testConditionsLabel, m_testConditionList);
    formLayoutMulti->addRow(new QLabel(m_window), getCheckButtons(m_testConditionList));

    QPushButton* updateMultiButton = new QPushButton(tr("Update"), m_window);
    QObject::connect(updateMultiButton, &QCheckBox::clicked,
                     m_window, &MainWindow::showData);

    layoutMulti->addLayout(formLayoutMulti);
    layoutMulti->addWidget(updateMultiButton);

    m_formWidgetMulti->hide();
    m_formWidgetMulti->setLayout(layoutMulti);
    filterLayout->addWidget(m_formWidgetMulti);

    // Eye data
    eyeDataLayout->addWidget(gazeDataTitle);
    eyeDataLayout->addWidget(m_gazeDataComboBox);

    // Time limit
    timeLimitLayout->addWidget(timeLimitTitle);

    QLabel* timeLimitAdditionalLable= new QLabel("-");
    timeLimitValueLayout1->addWidget(m_timeLimitCheckBox);
    timeLimitValueLayout2->addWidget(m_startTimeLimitSpinBox);
    timeLimitValueLayout2->addWidget(timeLimitAdditionalLable);
    timeLimitValueLayout2->addWidget(m_endTimeLimitSpinBox);
    timeLimitValueLayout2->addWidget(m_timeLimitZoomButton);
    timeLimitValueLayout2->addStretch(0);

    timeLimitLayout->addLayout(timeLimitValueLayout1);
    timeLimitLayout->addLayout(timeLimitValueLayout2);

    // group boxes
    QGroupBox* filterGroupBoxFilter = new QGroupBox(m_window);
    filterGroupBoxFilter->setLayout(filterLayout);

    QGroupBox* eyeDataGroupBoxFilter = new QGroupBox(m_window);
    eyeDataGroupBoxFilter->setLayout(eyeDataLayout);

    QGroupBox* timeLimitGroupBox = new QGroupBox(m_window);
    timeLimitGroupBox->setLayout(timeLimitLayout);

    // layout of the dock widget
    QVBoxLayout* finalFilterLayout = new QVBoxLayout();
    // add the widgets to the layout
    finalFilterLayout->addWidget(filterGroupBoxFilter);
    finalFilterLayout->addWidget(eyeDataGroupBoxFilter);
    finalFilterLayout->addWidget(timeLimitGroupBox);
    finalFilterLayout->addStretch();

    // dock
    QWidget* filterWidget = new QWidget();
    filterWidget->setLayout(finalFilterLayout);

    scrollArea->setWidget(filterWidget);
    scrollArea->setWidgetResizable(true);

    m_filterDock = new QDockWidget(tr("Filter"), m_window);
    m_filterDock->setWidget(scrollArea);

    m_filterDock->setObjectName(tr("FilterDockWidget"));
}


void UserInterface::createViewDock()
{
    QScrollArea* scrollArea = new QScrollArea;

    // general information:
    QLabel* viewTitleMainView = new QLabel(tr("<b>Stimulus View</b>"), m_window);
    QLabel* viewTitleTimeline = new QLabel(tr("<b>Timeline</b>"), m_window);
    QLabel* viewTitleFixations = new QLabel(tr("<b>Visible Fixation Area</b>"), m_window);

    QLabel* backgroundLabel = new QLabel(tr("Image Opacity:"), m_window);

    m_showMicrosaccadeDirectionsCheckbox = new QCheckBox(tr("Show Microsaccade Directions"), m_window);
    m_showMicrosaccadesCheckbox = new QCheckBox(tr("Highlight Microsaccade Samples"), m_window);
    m_highlightFixationsCheckbox = new QCheckBox(tr("Highlight Fixations Samples"), m_window);
    m_highlightCurrentFixationCheckbox = new QCheckBox(tr("Highlight Samples for Current Fixation"), m_window);

    m_showScanpathCheckBox = new QCheckBox(tr("Show Scanpath"), m_window);
    m_showSaccadeDirectionsCheckbox = new QCheckBox(tr("Show Saccade Directions"), m_window);

    m_showSampleConnectionsCheckbox = new QCheckBox(tr("Show Sample Connections"), m_window);
    m_showSamplesCheckbox = new QCheckBox(tr("Show Samples"), m_window);

    m_backgroundOpacitySlider = new QSlider(Qt::Horizontal, m_window);
    m_backgroundOpacitySlider->setMaximum(0);
    m_backgroundOpacitySlider->setMaximum(100);

    m_showMicrosaccadesInTimelineCheckBox = new QCheckBox(tr("Show Microsaccades"), m_window);
    m_showFixationsInTimelineCheckBox = new QCheckBox(tr("Show Fixations"), m_window);
    m_showEventsInTimelineCheckBox = new QCheckBox(tr("Show Events"), m_window);
    m_showXValuesInTimelineCheckBox = new QCheckBox(tr("Show x Values"), m_window);
    m_showYValuesInTimelineCheckBox = new QCheckBox(tr("Show y Values"), m_window);
    m_showVelocityValuesInTimelineCheckBox = new QCheckBox(tr("Show Velocity"), m_window);

    m_showNeighboringFixationsCheckbox = new QCheckBox(tr("Show Neighboring Fixations Only"), m_window);
    m_followingFixationsCountSpinBox = new QSpinBox(m_window);
    m_followingFixationsCountSpinBox->setMinimum(0);
    m_followingFixationsCountSpinBox->setMaximum(100);

    QLabel* showFollowingFixationsLabel = new QLabel(tr("Following Fixations"), m_window);
    QLabel* showPreviousFixationsLabel = new QLabel(tr("Previous Fixations"), m_window);
    m_previousFixationsCountSpinBox = new QSpinBox(m_window);
    m_previousFixationsCountSpinBox->setMinimum(0);
    m_previousFixationsCountSpinBox->setMaximum(100);

    QLabel* fixationSizeLabel = new QLabel(tr("Fixation Size:"), m_window);
    QLabel* fixationScaleValueLabel = new QLabel(tr("Fixation Scale:"), m_window);

    m_fixationSizeComboBox = new QComboBox(m_window);
    m_fixationSizeComboBox->addItem(tr("Duration"));
    m_fixationSizeComboBox->addItem(tr("Equal"));
    m_fixationSizeComboBox->addItem(tr("Microsaccades Count"));
    m_fixationSizeSlider = new QSlider(Qt::Horizontal, m_window);
    m_fixationSizeSlider->setMaximum(0);
    m_fixationSizeSlider->setMaximum(500);

    m_activateZoomToFixationAreaCheckBox = new QCheckBox(tr("Zoom to Visible Fixation Area"), m_window);

    QObject::connect(m_showScanpathCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showScanPath);
    QObject::connect(m_showMicrosaccadesCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showMicrosaccades);
    QObject::connect(m_showSamplesCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showSamples);
    QObject::connect(m_showSampleConnectionsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showSampleConnections);
    QObject::connect(m_showMicrosaccadeDirectionsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showMicrosaccadesDirection);
    QObject::connect(m_highlightFixationsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::highlightFixations);
    QObject::connect(m_highlightCurrentFixationCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::highlightCurrentFixation);
    QObject::connect(m_showSaccadeDirectionsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showSaccadeDirections);

    QObject::connect(m_backgroundOpacitySlider, &QSlider::valueChanged,
                     m_window, &MainWindow::backgroundOpacityChanged);

    QObject::connect(m_showMicrosaccadesInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showMicrosaccadesInTimeline);
    QObject::connect(m_showFixationsInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showFixationsInTimeline);
    QObject::connect(m_showEventsInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showEventsInTimeline);
    QObject::connect(m_showXValuesInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showXValuesInTimeline);
    QObject::connect(m_showYValuesInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showYValuesInTimeline);
    QObject::connect(m_showVelocityValuesInTimelineCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::showVelocityValuesInTimeline);

    QObject::connect(m_showNeighboringFixationsCheckbox, &QCheckBox::toggled,
                     m_window, &MainWindow::showNeighboringFixations);
    QObject::connect(m_followingFixationsCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     m_window, &MainWindow::followingFixationsCountChanged);
    QObject::connect(m_previousFixationsCountSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     m_window, &MainWindow::previousFixationsCountChanged);

    QObject::connect(m_fixationSizeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::fixationSizeChanged);
    QObject::connect(m_fixationSizeSlider, &QSlider::valueChanged,
                     m_window, &MainWindow::fixationScaleValueChanged);

    QObject::connect(m_activateZoomToFixationAreaCheckBox, &QCheckBox::toggled,
                     m_window, &MainWindow::activateZoomToFixationArea);

    QVBoxLayout* viewLayoutMainView = new QVBoxLayout();
    QVBoxLayout* viewLayoutTimeline = new QVBoxLayout();

    // Main View
    viewLayoutMainView->addWidget(viewTitleMainView);

    QHBoxLayout* viewSubLayoutMainView1 = new QHBoxLayout();

    viewSubLayoutMainView1->addWidget(backgroundLabel);
    viewSubLayoutMainView1->addWidget(m_backgroundOpacitySlider);

    QVBoxLayout* viewSubLayoutMainView2 = new QVBoxLayout();

    viewSubLayoutMainView2->addWidget(m_showMicrosaccadeDirectionsCheckbox);
    viewSubLayoutMainView2->addWidget(m_showMicrosaccadesCheckbox);

    viewSubLayoutMainView2->addWidget(m_highlightFixationsCheckbox);
    QVBoxLayout* fixationDetailsLayout = new QVBoxLayout();
    fixationDetailsLayout->addWidget(m_highlightCurrentFixationCheckbox);
    //fixationDetailsLayout->setContentsMargins(15, 0, 0, 0);
    viewSubLayoutMainView2->addLayout(fixationDetailsLayout);

    viewSubLayoutMainView2->addWidget(m_showScanpathCheckBox);
    QFormLayout* scanpathDetailsLayout = new QFormLayout();
    scanpathDetailsLayout->addRow(fixationSizeLabel, m_fixationSizeComboBox);
    scanpathDetailsLayout->addRow(fixationScaleValueLabel, m_fixationSizeSlider);
    viewSubLayoutMainView2->addLayout(scanpathDetailsLayout);
    scanpathDetailsLayout->setContentsMargins(15, 0, 0, 0);

    viewSubLayoutMainView2->addWidget(m_showSaccadeDirectionsCheckbox);
    m_showSaccadeDirectionsCheckbox->setContentsMargins(15, 0, 0, 0);

    viewSubLayoutMainView2->addWidget(m_showSampleConnectionsCheckbox);
    viewSubLayoutMainView2->addWidget(m_showSamplesCheckbox);

    viewLayoutMainView->addLayout(viewSubLayoutMainView2);


    // Timeline
    viewLayoutTimeline->addWidget(viewTitleTimeline);

    viewLayoutTimeline->addWidget(m_showMicrosaccadesInTimelineCheckBox);
    viewLayoutTimeline->addWidget(m_showFixationsInTimelineCheckBox);
    viewLayoutTimeline->addWidget(m_showEventsInTimelineCheckBox);
    viewLayoutTimeline->addWidget(m_showXValuesInTimelineCheckBox);
    viewLayoutTimeline->addWidget(m_showYValuesInTimelineCheckBox);
    viewLayoutTimeline->addWidget(m_showVelocityValuesInTimelineCheckBox);

    // Fixations
    QVBoxLayout* viewFixationsLayout = new QVBoxLayout();
    viewFixationsLayout->addWidget(viewTitleFixations);

    QHBoxLayout* followingFixationsLayout = new QHBoxLayout();
    followingFixationsLayout->addWidget(showFollowingFixationsLabel);
    followingFixationsLayout->addWidget(m_followingFixationsCountSpinBox);

    QHBoxLayout* previousFixationsLayout = new QHBoxLayout();
    previousFixationsLayout->addWidget(showPreviousFixationsLabel);
    previousFixationsLayout->addWidget(m_previousFixationsCountSpinBox);
    viewFixationsLayout->addWidget(m_showNeighboringFixationsCheckbox);
    viewFixationsLayout->addLayout(previousFixationsLayout);
    viewFixationsLayout->addLayout(followingFixationsLayout);
    viewFixationsLayout->addWidget(m_activateZoomToFixationAreaCheckBox);

    previousFixationsLayout->setContentsMargins(15, 0, 0, 0);
    followingFixationsLayout->setContentsMargins(15, 0, 0, 0);


    // group boxes
    m_viewGroupBoxMainView = new QGroupBox(m_window);
    m_viewGroupBoxMainView->setLayout(viewLayoutMainView);

    QGroupBox* m_viewGroupBoxMainView2 = new QGroupBox(m_window);
    m_viewGroupBoxMainView2->setLayout(viewSubLayoutMainView1);

    m_viewGroupBoxTimeline = new QGroupBox(m_window);
    m_viewGroupBoxTimeline->setLayout(viewLayoutTimeline);

    m_viewGroupBoxFixations = new QGroupBox(m_window);
    m_viewGroupBoxFixations->setLayout(viewFixationsLayout);

    // layout of the dock widget
    QVBoxLayout* finalViewLayout = new QVBoxLayout();
    // add the widgets to the layout
    finalViewLayout->addWidget(m_viewGroupBoxMainView);
    finalViewLayout->addWidget(m_viewGroupBoxMainView2);
    finalViewLayout->addWidget(m_viewGroupBoxTimeline);
    finalViewLayout->addWidget(m_viewGroupBoxFixations);
    finalViewLayout->addStretch();

    // dock
    QWidget* viewWidget = new QWidget();
    viewWidget->setLayout(finalViewLayout);

    scrollArea->setWidget(viewWidget);
    scrollArea->setWidgetResizable(true);

    m_viewDock = new QDockWidget(tr("View"), m_window);
    m_viewDock->setWidget(scrollArea);

    m_viewDock->setObjectName(tr("ViewDockWidget"));
}


void UserInterface::createMicrosaccadesDetectionDock()
{
    QScrollArea* scrollArea = new QScrollArea;
    QGridLayout* microsaccadesDockWidgetLayout = new QGridLayout();

    // checkbox to use microsaccades from input file/override
    m_microsaccadesFromInputFileCheckbox =
            new QCheckBox(tr("Use Microsaccades from Input File"), m_window);
    m_microsaccadesFromInputFileCheckbox->setStyleSheet("font-weight: bold");

    // relative velocity threshold
    QLabel* msVelocityThresholdLabel = new QLabel(tr("Relative Velocity Threshold"), m_window);
    m_msVelocityThresholdSpinBox = new QDoubleSpinBox(m_window);
    m_msVelocityThresholdSpinBox->setMinimum(0);
    m_msVelocityThresholdSpinBox->setMaximum(100);

    // minimum microsaccade duration [ms]
    QLabel* msMinDurationLabel =
            new QLabel(tr("Minimum Microsaccade Duration [ms]"), m_window);
    m_msMinDurationSpinBox = new QSpinBox(m_window);
    m_msMinDurationSpinBox->setMinimum(1);
    m_msMinDurationSpinBox->setMaximum(1000);

    // velocity window size
    QLabel* velocityWindowSizeLabel =
            new QLabel(tr("Velocity Window Size [samples]"), m_window);
    m_msVelocityWindowSizeSpinBox = new QSpinBox(m_window);
    m_msVelocityWindowSizeSpinBox->setMinimum(3);
    m_msVelocityWindowSizeSpinBox->setMaximum(99);
    m_msVelocityWindowSizeSpinBox->setSingleStep(2);

    // binocular?
    m_msBinocularCheckBox = new QCheckBox(tr("Binocular Microsaccades Only"), m_window);
    //m_msBinocularCheckBox->setToolTip(tr("Use binocular microsaccades only."));

    // max duration
    m_msMaxDurationCheckBox =
            new QCheckBox(tr("Maximum Microsaccade Duration [ms]"), m_window);
    m_msMaxDurationSpinBox = new QSpinBox(m_window);
    m_msMaxDurationSpinBox->setMinimum(0);
    m_msMaxDurationSpinBox->setMaximum(10000);

    // min amplitude
    m_msMinAmplitudeCheckBox = new QCheckBox(tr("Minimum Amplitude [°]"), m_window);
    m_msMinAmplitudeSpinBox = new QDoubleSpinBox(m_window);
    m_msMinAmplitudeSpinBox->setMinimum(0);
    m_msMinAmplitudeSpinBox->setMaximum(1000);
    m_msMinAmplitudeSpinBox->setSingleStep(0.1);

    // max amplitude
    m_msMaxAmplitudeCheckBox = new QCheckBox(tr("Maximum Amplitude [°]"), m_window);
    m_msMaxAmplitudeSpinBox = new QDoubleSpinBox(m_window);
    m_msMaxAmplitudeSpinBox->setMinimum(0);
    m_msMaxAmplitudeSpinBox->setMaximum(1000);
    m_msMaxAmplitudeSpinBox->setSingleStep(0.1);

    // min inter-saccadic interval
    m_msMinIntersaccIntervalCheckBox =
            new QCheckBox(tr("Minimum Inter-Saccadic Interval [ms]"), m_window);
    m_msMinIntersaccIntervalSpinBox = new QSpinBox(m_window);
    m_msMinIntersaccIntervalSpinBox->setMinimum(0);
    m_msMinIntersaccIntervalSpinBox->setMaximum(10000);

    // velocities
    m_msMinVelocityCheckBox =
            new QCheckBox(tr("Minimum Peak Velocity [°/s]"), m_window);
    m_msMinVelocitySpinBox = new QDoubleSpinBox(m_window);
    m_msMinVelocitySpinBox->setMinimum(0);
    m_msMinVelocitySpinBox->setMaximum(10000);

    m_msMaxVelocityCheckBox =
            new QCheckBox(tr("Maximum Peak Velocity [°/s]"), m_window);
    m_msMaxVelocitySpinBox = new QDoubleSpinBox(m_window);
    m_msMaxVelocitySpinBox->setMinimum(0);
    m_msMaxVelocitySpinBox->setMaximum(10000);

    // ignore time spans
    m_msIgnoreAtFixationStartCheckBox =
            new QCheckBox(tr("Ignore Time at Fixation Start (e.g. Glissades) [ms]"), m_window);
    m_msIgnoreAtFixationStartSpinBox = new QSpinBox(m_window);
    m_msIgnoreAtFixationStartSpinBox->setMinimum(0);
    m_msIgnoreAtFixationStartSpinBox->setMaximum(10000);

    m_msIgnoreAtFixationEndCheckBox =
            new QCheckBox(tr("Ignore Time at Fixation End [ms]"), m_window);
    m_msIgnoreAtFixationEndSpinBox = new QSpinBox(m_window);
    m_msIgnoreAtFixationEndSpinBox->setMinimum(0);
    m_msIgnoreAtFixationEndSpinBox->setMaximum(10000);

    m_msIgnoreBeforeMissingDataCheckBox =
            new QCheckBox(tr("Ignore Time Before Missing Data [ms]"), m_window);
    m_msIgnoreBeforeMissingDataSpinBox = new QSpinBox(m_window);
    m_msIgnoreBeforeMissingDataSpinBox->setMinimum(0);
    m_msIgnoreBeforeMissingDataSpinBox->setMaximum(10000);

    m_msIgnoreAfterMissingDataCheckBox =
            new QCheckBox(tr("Ignore Time After Missing Data [ms]"), m_window);
    m_msIgnoreAfterMissingDataSpinBox = new QSpinBox(m_window);
    m_msIgnoreAfterMissingDataSpinBox->setMinimum(0);
    m_msIgnoreAfterMissingDataSpinBox->setMaximum(10000);

    m_updateMicrosaccadesButton = new QPushButton(tr("Update Microsaccades for Current Trial"), m_window);
    QObject::connect(m_updateMicrosaccadesButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateCurrentMicrosaccadesDetection);

    m_updateCurrentParticipantMicrosaccadesButton =
            new QPushButton(tr("Update Microsaccades for Current Participant"), m_window);
    QObject::connect(m_updateCurrentParticipantMicrosaccadesButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateMicrosaccadesForParticipant);

    QPushButton* updateAllMicrosaccadesButton =
            new QPushButton(tr("Update Microsaccades for all Trials"), m_window);
    QObject::connect(updateAllMicrosaccadesButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateMicrosaccadesForAllDetection);

    microsaccadesDockWidgetLayout->addWidget(m_microsaccadesFromInputFileCheckbox, 0, 0, 1, 2);
    microsaccadesDockWidgetLayout->addWidget(msVelocityThresholdLabel, 1, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msVelocityThresholdSpinBox, 1, 1);
    microsaccadesDockWidgetLayout->addWidget(msMinDurationLabel, 2, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMinDurationSpinBox, 2, 1);

    microsaccadesDockWidgetLayout->addWidget(velocityWindowSizeLabel, 3, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msVelocityWindowSizeSpinBox, 3, 1);

    microsaccadesDockWidgetLayout->addWidget(m_msBinocularCheckBox, 4, 0);

    microsaccadesDockWidgetLayout->addWidget(m_msMaxDurationCheckBox, 5, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMaxDurationSpinBox, 5, 1);

    microsaccadesDockWidgetLayout->addWidget(m_msMinAmplitudeCheckBox, 6, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMinAmplitudeSpinBox, 6, 1);
    microsaccadesDockWidgetLayout->addWidget(m_msMaxAmplitudeCheckBox, 7, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMaxAmplitudeSpinBox, 7, 1);
    microsaccadesDockWidgetLayout->addWidget(m_msMinIntersaccIntervalCheckBox, 8, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMinIntersaccIntervalSpinBox, 8, 1);

    microsaccadesDockWidgetLayout->addWidget(m_msMinVelocityCheckBox, 9, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMinVelocitySpinBox, 9, 1);
    microsaccadesDockWidgetLayout->addWidget(m_msMaxVelocityCheckBox, 10, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msMaxVelocitySpinBox, 10, 1);

    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAtFixationStartCheckBox, 11, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAtFixationStartSpinBox, 11, 1);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAtFixationEndCheckBox, 12, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAtFixationEndSpinBox, 12, 1);

    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreBeforeMissingDataCheckBox, 13, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreBeforeMissingDataSpinBox, 13, 1);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAfterMissingDataCheckBox, 14, 0);
    microsaccadesDockWidgetLayout->addWidget(m_msIgnoreAfterMissingDataSpinBox, 14, 1);

    microsaccadesDockWidgetLayout->addWidget(m_updateMicrosaccadesButton, 15, 0, 1, 2);
    microsaccadesDockWidgetLayout->addWidget(m_updateCurrentParticipantMicrosaccadesButton, 16, 0, 1, 2);
    microsaccadesDockWidgetLayout->addWidget(updateAllMicrosaccadesButton, 17, 0, 1, 2);
    microsaccadesDockWidgetLayout->setRowStretch(18, 1); // add stretch in next line

    // dock
    QWidget* microsaccadesWidget = new QWidget();
    microsaccadesWidget->setLayout(microsaccadesDockWidgetLayout);

    scrollArea->setWidget(microsaccadesWidget);
    scrollArea->setWidgetResizable(true);

    m_microsaccadesDetectionDockWidget = new QDockWidget(tr("Microsaccade Detection"), m_window);
    m_microsaccadesDetectionDockWidget->setWidget(scrollArea);

    m_microsaccadesDetectionDockWidget->setObjectName(tr("MicrosaccadesDetectionWidget"));
}


void UserInterface::createFixationsDetectionDock()
{
    QScrollArea* scrollArea = new QScrollArea;
    QGridLayout* fixationsDockWidgetLayout = new QGridLayout();

    // checkbox to use fixations from input file/override
    m_fixationsFromInputFileCheckbox = new QCheckBox(tr("Use Fixations from Input File"), m_window);
    m_fixationsFromInputFileCheckbox->setStyleSheet("font-weight: bold");

    // relative velocity threshold
    QLabel* velocityThresholdLabel = new QLabel(tr("Relative Velocity Threshold"), m_window);
    m_fixVelocityThresholdSpinBox = new QDoubleSpinBox(m_window);
    m_fixVelocityThresholdSpinBox->setMinimum(0);
    m_fixVelocityThresholdSpinBox->setMaximum(100);

    // minimum saccade duration [ms]
    QLabel* minSaccadeDurationLabel =
            new QLabel(tr("Minimum Saccade Duration [ms]"), m_window);
    m_fixMinSaccadeDurationSpinBox = new QSpinBox(m_window);
    m_fixMinSaccadeDurationSpinBox->setMinimum(1);
    m_fixMinSaccadeDurationSpinBox->setMaximum(1000);

    // velocity window size
    QLabel* velocityWindowSizeLabel =
            new QLabel(tr("Velocity Window Size [samples]"), m_window);
    m_fixVelocityWindowSizeSpinBox = new QSpinBox(m_window);
    m_fixVelocityWindowSizeSpinBox->setMinimum(3);
    m_fixVelocityWindowSizeSpinBox->setMaximum(99);
    m_fixVelocityWindowSizeSpinBox->setSingleStep(2);

    // binocular?
    m_fixBinocularCheckBox = new QCheckBox(tr("Binocular Saccades Only"), m_window);
    m_fixBinocularCheckBox->setToolTip(tr("Use binocular saccades only."));

    // max duration
    m_fixMaxDurationCheckBox =
            new QCheckBox(tr("Maximum Saccade Duration [ms]"), m_window);
    m_fixMaxDurationSpinBox = new QSpinBox(m_window);
    m_fixMaxDurationSpinBox->setMinimum(0);
    m_fixMaxDurationSpinBox->setMaximum(10000);

    // max amplitude
    m_fixMaxAmplitudeCheckBox = new QCheckBox(tr("Maximum Saccade Amplitude [°]"), m_window);
    m_fixMaxSaccadeAmplitudeSpinBox = new QDoubleSpinBox(m_window);
    m_fixMaxSaccadeAmplitudeSpinBox->setMinimum(0);
    m_fixMaxSaccadeAmplitudeSpinBox->setMaximum(1000);

    // min amplitude
    m_fixMinAmplitudeCheckBox = new QCheckBox(tr("Minimum Saccade Amplitude [°]"), m_window);
    m_fixMinSaccadeAmplitudeSpinBox = new QDoubleSpinBox(m_window);
    m_fixMinSaccadeAmplitudeSpinBox->setMinimum(0);
    m_fixMinSaccadeAmplitudeSpinBox->setMaximum(1000);

    // min inter-saccadic interval
    m_fixMinIntersaccIntervalCheckBox =
            new QCheckBox(tr("Minimum Inter-Saccadic Interval (i.e. Minimum Fixation Time) [ms]"), m_window);
    m_fixMinSaccadeIntersaccIntervalSpinBox = new QSpinBox(m_window);
    m_fixMinSaccadeIntersaccIntervalSpinBox->setMinimum(0);
    m_fixMinSaccadeIntersaccIntervalSpinBox->setMaximum(10000);

    // velocities
    m_fixMinVelocityCheckBox =
            new QCheckBox(tr("Minimum Saccade Peak Velocity [°/s]"), m_window);
    m_fixMinVelocitySpinBox = new QDoubleSpinBox(m_window);
    m_fixMinVelocitySpinBox->setMinimum(0);
    m_fixMinVelocitySpinBox->setMaximum(10000);

    m_fixMaxVelocityCheckBox =
            new QCheckBox(tr("Maximum Saccade Peak Velocity [°/s]"), m_window);
    m_fixMaxVelocitySpinBox = new QDoubleSpinBox(m_window);
    m_fixMaxVelocitySpinBox->setMinimum(0);
    m_fixMaxVelocitySpinBox->setMaximum(10000);

    // ignore time spans
    m_fixIgnoreAtFixationStartCheckBox =
            new QCheckBox(tr("Ignore Time at Start of Data [ms]"), m_window);
    m_fixIgnoreAtStartSpinBox = new QSpinBox(m_window);
    m_fixIgnoreAtStartSpinBox->setMinimum(0);
    m_fixIgnoreAtStartSpinBox->setMaximum(1000000);

    m_fixIgnoreAtFixationEndCheckBox=
            new QCheckBox(tr("Ignore Time at End of Data [ms]"), m_window);
    m_fixIgnoreAtEndSpinBox = new QSpinBox(m_window);
    m_fixIgnoreAtEndSpinBox->setMinimum(0);
    m_fixIgnoreAtEndSpinBox->setMaximum(1000000);

    m_fixIgnoreBeforeMissingDataCheckBox=
            new QCheckBox(tr("Ignore Time Before Missing Data [ms]"), m_window);
    m_fixIgnoreBeforeMissingDataSpinBox = new QSpinBox(m_window);
    m_fixIgnoreBeforeMissingDataSpinBox->setMinimum(0);
    m_fixIgnoreBeforeMissingDataSpinBox->setMaximum(1000000);

    m_fixIgnoreAfterMissingDataCheckBox=
            new QCheckBox(tr("Ignore Time After Missing Data [ms]"), m_window);
    m_fixIgnoreAfterMissingDataSpinBox = new QSpinBox(m_window);
    m_fixIgnoreAfterMissingDataSpinBox->setMinimum(0);
    m_fixIgnoreAfterMissingDataSpinBox->setMaximum(1000000);

    m_updateFixationsButton = new QPushButton(tr("Update Fixations for Current Trial"), m_window);
    QObject::connect(m_updateFixationsButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateCurrentFixationsDetection);

    m_updateFixationsForParticipantButton = new QPushButton(tr("Update Fixations for Current Participant"), m_window);
    QObject::connect(m_updateFixationsForParticipantButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateFixationsForCurrentParticipantDetection);

    QPushButton* updateAllFixationsButton =
            new QPushButton(tr("Update Fixations for all Trials"), m_window);
    QObject::connect(updateAllFixationsButton, &QCheckBox::clicked,
                     m_window, &MainWindow::updateFixationsForAllDetection);

    fixationsDockWidgetLayout->addWidget(m_fixationsFromInputFileCheckbox, 0, 0, 1, 2);
    fixationsDockWidgetLayout->addWidget(velocityThresholdLabel, 1, 0);
    fixationsDockWidgetLayout->addWidget(m_fixVelocityThresholdSpinBox, 1, 1);
    fixationsDockWidgetLayout->addWidget(minSaccadeDurationLabel, 2, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMinSaccadeDurationSpinBox, 2, 1);

    fixationsDockWidgetLayout->addWidget(velocityWindowSizeLabel, 3, 0);
    fixationsDockWidgetLayout->addWidget(m_fixVelocityWindowSizeSpinBox, 3, 1);

    fixationsDockWidgetLayout->addWidget(m_fixBinocularCheckBox, 4, 0);

    fixationsDockWidgetLayout->addWidget(m_fixMaxDurationCheckBox, 5, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMaxDurationSpinBox, 5, 1);

    fixationsDockWidgetLayout->addWidget(m_fixMinAmplitudeCheckBox, 6, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMinSaccadeAmplitudeSpinBox, 6, 1);
    fixationsDockWidgetLayout->addWidget(m_fixMaxAmplitudeCheckBox, 7, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMaxSaccadeAmplitudeSpinBox, 7, 1);
    fixationsDockWidgetLayout->addWidget(m_fixMinIntersaccIntervalCheckBox, 8, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMinSaccadeIntersaccIntervalSpinBox, 8, 1);

    fixationsDockWidgetLayout->addWidget(m_fixMinVelocityCheckBox, 9, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMinVelocitySpinBox, 9, 1);
    fixationsDockWidgetLayout->addWidget(m_fixMaxVelocityCheckBox, 10, 0);
    fixationsDockWidgetLayout->addWidget(m_fixMaxVelocitySpinBox, 10, 1);

    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAtFixationStartCheckBox, 11, 0);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAtStartSpinBox, 11, 1);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAtFixationEndCheckBox, 12, 0);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAtEndSpinBox, 12, 1);

    fixationsDockWidgetLayout->addWidget(m_fixIgnoreBeforeMissingDataCheckBox, 13, 0);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreBeforeMissingDataSpinBox, 13, 1);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAfterMissingDataCheckBox, 14, 0);
    fixationsDockWidgetLayout->addWidget(m_fixIgnoreAfterMissingDataSpinBox, 14, 1);

    fixationsDockWidgetLayout->addWidget(m_updateFixationsButton, 15, 0, 1, 2);
    fixationsDockWidgetLayout->addWidget(m_updateFixationsForParticipantButton, 16, 0, 1, 2);
    fixationsDockWidgetLayout->addWidget(updateAllFixationsButton, 17, 0, 1, 2);
    fixationsDockWidgetLayout->setRowStretch(18, 1); // add stretch in next line

    // dock
    QWidget* fixationsWidget = new QWidget;
    fixationsWidget->setLayout(fixationsDockWidgetLayout);

    scrollArea->setWidget(fixationsWidget);
    scrollArea->setWidgetResizable(true);

    m_fixationsDetectionDockWidget = new QDockWidget(tr("Fixation Detection"), m_window);
    m_fixationsDetectionDockWidget->setWidget(scrollArea);

    m_fixationsDetectionDockWidget->setObjectName(tr("FixationssDetectionWidget"));
}


void UserInterface::createColorDock()
{
    QScrollArea* scrollArea = new QScrollArea;
    QGridLayout* colorDockWidgetLayout = new QGridLayout();

    std::vector<QColor*> colors;

    // Mainview
    colors.push_back(m_settings->m_colors.sampleColor);
    colors.push_back(m_settings->m_colors.sampleConnectionColor);
    colors.push_back(m_settings->m_colors.fixationSamplesColor);

    colors.push_back(m_settings->m_colors.fixationColor);
    colors.push_back(m_settings->m_colors.fixationHighlightColor);

    colors.push_back(m_settings->m_colors.saccadeColor);
    colors.push_back(m_settings->m_colors.saccadesDirectionStartColor);

    colors.push_back(m_settings->m_colors.microsaccadesColor);
    colors.push_back(m_settings->m_colors.microsaccadesDirectionEndColor);

    colors.push_back(m_settings->m_colors.missingDataColor);

    colors.push_back(m_settings->m_colors.sceneBorderColor);

    // Timeline
    colors.push_back(m_settings->m_colors.yValueColor);
    colors.push_back(m_settings->m_colors.xValueColor);
    colors.push_back(m_settings->m_colors.velocityColor);
    colors.push_back(m_settings->m_colors.eventColor);

    // Data plots
    colors.push_back(m_settings->m_colors.dataPlotColor);
    colors.push_back(m_settings->m_colors.dataPlotBaseColor);
    colors.push_back(m_settings->m_colors.meanValueColor);

    std::vector<QString> colorDescription;

    // Mainview
    colorDescription.push_back("Samples in Stimulus View"); // sampleColor
    colorDescription.push_back("Sample Connections in Stimulus View"); // sampleConnectionsColor
    colorDescription.push_back("Fixation Samples in Stimulus View"); // fixationSampleData

    colorDescription.push_back("Fixation Color in Stimulus View and Timeline"); // fixationColor
    colorDescription.push_back("Fixation Highlight Color in Stimulus View and Timeline"); // fixationHighlightColor

    colorDescription.push_back("Saccade Color in Stimulus View"); // saccadeColor
    colorDescription.push_back("Saccade  Direction Start Color"); // saccadesDirectionStartColor

    colorDescription.push_back("Microsaccades Color"); // microsaccadesColor
    colorDescription.push_back("Microsaccades Direction End Color"); // microsaccadesDirEndColor

    colorDescription.push_back("Missing Data in Stimulus View and Timeline"); // missingDataColor

    colorDescription.push_back("Scene Border in Stimulus View"); // sceneBorder

    // Timeline
    colorDescription.push_back("y Value of Samples in Timeline"); // yDirColor
    colorDescription.push_back("x Value of Samples in Timeline"); // xDirColor
    colorDescription.push_back("Velocity in Timeline"); // velocityColor
    colorDescription.push_back("Events"); // velocityColor

    // Data plots
    colorDescription.push_back("Data Pots"); // dataPlotColor
    colorDescription.push_back("Data Plot Base Color"); // dataPlotBaseColor
    colorDescription.push_back("Data Plot Mean Color"); // dataPlotBaseColor


    for (size_t i = 0; i < colors.size(); ++i)
    {
        QPushButton* button = new QPushButton();
        QPalette pal = button->palette();
        pal.setColor(QPalette::Button, QColor(*colors[i]));

        button->setAutoFillBackground(true);
        button->setPalette(pal);
        button->setFlat(true);
        button->update();

        QWidget* widget = new QWidget();
        widget->setStyleSheet("background-color:white;");

        QLabel* label = new QLabel(colorDescription[i]);

        updateColorButton(colors[i], button);

        colorDockWidgetLayout->addWidget(widget, int(i), 0);
        colorDockWidgetLayout->addWidget(button, int(i), 0);

        colorDockWidgetLayout->addWidget(label, int(i), 1);

        QObject::connect(button, &QPushButton::clicked, this, &UserInterface::showColorDialog);

        buttonsToColorMapping.insert(button, colors[i]);
    }
    colorDockWidgetLayout->setRowStretch(int(colors.size()), 1); // add stretch in next line

    // dock
    QWidget* colorWidget = new QWidget;

    colorWidget->setLayout(colorDockWidgetLayout);

    scrollArea->setWidget(colorWidget);
    scrollArea->setWidgetResizable(true);

    m_colorDockWidget = new QDockWidget(tr("Colors"), m_window);
    m_colorDockWidget->setWidget(scrollArea);

    m_colorDockWidget->setObjectName(tr("ColorWidget"));
}


void UserInterface::createDiagramDock()
{
    QScrollArea* scrollArea = new QScrollArea;
    QVBoxLayout* m_diagramDockWidgetLayout = new QVBoxLayout();

    QGridLayout* gridLayout = new QGridLayout();

    QLabel* dataLabel = new QLabel(tr("Data for Plots"), m_window);
    m_dataComboBox = new QComboBox();
    m_dataComboBox->addItem(tr("Microsaccades"));
    m_dataComboBox->addItem(tr("Saccades (Detected)"));
    m_dataComboBox->setCurrentIndex(0);
    QObject::connect(m_dataComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showHistogramForTrials);
    QObject::connect(m_dataComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showScatterplotForTrials);

    gridLayout->addWidget(dataLabel, 1, 0);
    gridLayout->addWidget(m_dataComboBox, 1, 1);

    //QGroupBox* generalGroupBox = new QGroupBox(m_window);
    //generalGroupBox->setLayout(gridLayout);

    m_diagramDockWidgetLayout->addLayout(gridLayout);
    m_diagramDockWidgetLayout->addSpacing(30);

    // ** histogram ** //
    // plot
    m_histogramPlot = new EyeHistogram(m_settings);
    m_histogramPlot->setMinimumHeight(220);

    QGridLayout* gridLayoutHistogramSettings = new QGridLayout();

    // settings
    QLabel* histogramYAxisLabel = new QLabel(tr("On y Axis:"), m_window);
    m_histogramYAxisCombooBox = new QComboBox();
    m_histogramYAxisCombooBox->addItem(tr("Peak Velocity [°/s]"));
    m_histogramYAxisCombooBox->addItem(tr("Duration [ms]"));
    m_histogramYAxisCombooBox->addItem(tr("Amplitude [°]"));
    m_histogramYAxisCombooBox->addItem(tr("Microsaccades start time [ms]"));
    m_histogramYAxisCombooBox->setCurrentIndex(3);
    QObject::connect(m_histogramYAxisCombooBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showHistogramForTrials);
    QObject::connect(m_histogramYAxisCombooBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     this, &UserInterface::updateHistogramOptions);

    m_histogramStartTimeBinsLabel = new QLabel(tr("Bin size:"), m_window);
    m_histogramStartTimeBinSizeSpinBox = new QSpinBox(m_window);
    m_histogramStartTimeBinsLabel->setToolTip(tr("Bin size for the bars."));
    m_histogramStartTimeBinSizeSpinBox->setMinimum(1);
    m_histogramStartTimeBinSizeSpinBox->setMaximum(10000);
    QObject::connect(m_histogramStartTimeBinSizeSpinBox,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    m_histogramPeakVelocityBinsLabel = new QLabel(tr("Bin size:"), m_window);
    m_histogramPeakVelocityBinSizeSpinBox = new QDoubleSpinBox(m_window);
    m_histogramPeakVelocityBinsLabel->setToolTip(tr("Bin size for the bars."));
    m_histogramPeakVelocityBinSizeSpinBox->setMinimum(0.001);
    m_histogramPeakVelocityBinSizeSpinBox->setSingleStep(0.001);
    m_histogramPeakVelocityBinSizeSpinBox->setDecimals(3);
    m_histogramPeakVelocityBinSizeSpinBox->setMaximum(10000);
    m_histogramPeakVelocityBinsLabel->hide();
    m_histogramPeakVelocityBinSizeSpinBox->hide();
    QObject::connect(m_histogramPeakVelocityBinSizeSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    m_histogramAmplitudeBinsLabel = new QLabel(tr("Bin size:"), m_window);
    m_histogramAmplitudeBinSizeSpinBox = new QDoubleSpinBox(m_window);
    m_histogramAmplitudeBinsLabel->setToolTip(tr("Bin size for the bars."));
    m_histogramAmplitudeBinSizeSpinBox->setMinimum(0.001);
    m_histogramAmplitudeBinSizeSpinBox->setSingleStep(0.001);
    m_histogramAmplitudeBinSizeSpinBox->setDecimals(3);
    m_histogramAmplitudeBinSizeSpinBox->setMaximum(10000);
    m_histogramAmplitudeBinsLabel->hide();
    m_histogramAmplitudeBinSizeSpinBox->hide();
    QObject::connect(m_histogramAmplitudeBinSizeSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    m_histogramDurationBinsLabel = new QLabel(tr("Bin size:"), m_window);
    m_histogramDurationBinSizeSpinBox = new QSpinBox(m_window);
    m_histogramDurationBinsLabel->setToolTip(tr("Bin size for the bars."));
    m_histogramDurationBinSizeSpinBox->setMinimum(1);
    m_histogramDurationBinSizeSpinBox->setMaximum(10000);
    m_histogramDurationBinsLabel->hide();
    m_histogramDurationBinSizeSpinBox->hide();
    QObject::connect(m_histogramDurationBinSizeSpinBox,
                     static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    QLabel* histogramMaxXLabel = new QLabel(tr("Max x:"), m_window);
    m_histogramMaxXSpinBox = new QDoubleSpinBox(m_window);
    histogramMaxXLabel->setToolTip(tr("Maximum fo x axis; 0 uses default value"));
    m_histogramMaxXSpinBox->setMinimum(0);
    m_histogramMaxXSpinBox->setMaximum(100000);
    QObject::connect(m_histogramMaxXSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    QLabel* histogramMaxYLabel = new QLabel(tr("Max y:"), m_window);
    m_histogramMaxYSpinBox = new QDoubleSpinBox(m_window);
    histogramMaxYLabel->setToolTip(tr("Maximum fo y axis; 0 uses default value"));
    m_histogramMaxYSpinBox->setMinimum(0);
    m_histogramMaxYSpinBox->setMaximum(100000);
    QObject::connect(m_histogramMaxYSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showHistogramForTrials);

    gridLayoutHistogramSettings->addWidget(histogramYAxisLabel, 0, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramYAxisCombooBox, 0, 1);
    gridLayoutHistogramSettings->addWidget(m_histogramStartTimeBinsLabel, 1, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramStartTimeBinSizeSpinBox, 1, 1);
    gridLayoutHistogramSettings->addWidget(m_histogramPeakVelocityBinsLabel, 2, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramPeakVelocityBinSizeSpinBox, 2, 1);
    gridLayoutHistogramSettings->addWidget(m_histogramAmplitudeBinsLabel, 3, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramAmplitudeBinSizeSpinBox, 3, 1);
    gridLayoutHistogramSettings->addWidget(m_histogramDurationBinsLabel, 4, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramDurationBinSizeSpinBox, 4, 1);
    gridLayoutHistogramSettings->addWidget(histogramMaxXLabel, 5, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramMaxXSpinBox, 5, 1);
    gridLayoutHistogramSettings->addWidget(histogramMaxYLabel, 6, 0);
    gridLayoutHistogramSettings->addWidget(m_histogramMaxYSpinBox, 6, 1);
    gridLayoutHistogramSettings->addWidget(m_histogramPlot, 7, 0, 1, 2);

    m_diagramDockWidgetLayout->addLayout(gridLayoutHistogramSettings);
    m_diagramDockWidgetLayout->addSpacing(30);

    // ** scatterplot ** //
    // plot
    m_scatterplot = new EyeScatterplot(m_settings);
    m_scatterplot->setMinimumHeight(220);

    QGridLayout* gridLayoutScatterplotSettings = new QGridLayout();

    // settings
    QLabel* scatterplotXAxisLabel = new QLabel(tr("On x Axis:"), m_window);
    m_scatterplotXAxisComboBox = new QComboBox();
    m_scatterplotXAxisComboBox->addItem(tr("Peak Velocity [°/s]"));
    m_scatterplotXAxisComboBox->addItem(tr("Duration [ms]"));
    m_scatterplotXAxisComboBox->addItem(tr("Amplitude [°]"));
    m_scatterplotXAxisComboBox->setCurrentIndex(0);
    QObject::connect(m_scatterplotXAxisComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showScatterplotForTrials);

    QLabel* scatterplotYAxisLabel = new QLabel(tr("On y Axis:"), m_window);
    m_scatterplotYAxisComboBox = new QComboBox();
    m_scatterplotYAxisComboBox->addItem(tr("Peak Velocity [°/s]"));
    m_scatterplotYAxisComboBox->addItem(tr("Duration [ms]"));
    m_scatterplotYAxisComboBox->addItem(tr("Amplitude [°]"));
    m_scatterplotYAxisComboBox->setCurrentIndex(0);
    QObject::connect(m_scatterplotYAxisComboBox,
                     static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged),
                     m_window, &MainWindow::showScatterplotForTrials);

    QLabel* scatterplotMaxXLabel = new QLabel(tr("Max x:"), m_window);
    m_scatterplotMaxXSpinBox = new QDoubleSpinBox(m_window);
    scatterplotMaxXLabel->setToolTip(tr("Maximum fo x axis; 0 uses default value"));
    m_scatterplotMaxXSpinBox->setMinimum(0);
    m_scatterplotMaxXSpinBox->setMaximum(100000);
    QObject::connect(m_scatterplotMaxXSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showScatterplotForTrials);

    QLabel* scatterplotMaxYLabel = new QLabel(tr("Max y:"), m_window);
    m_scatterplotMaxYSpinBox = new QDoubleSpinBox(m_window);
    scatterplotMaxYLabel->setToolTip(tr("Maximum fo y axis; 0 uses default value"));
    m_scatterplotMaxYSpinBox->setMinimum(0);
    m_scatterplotMaxYSpinBox->setMaximum(100000);
    QObject::connect(m_scatterplotMaxYSpinBox,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     m_window, &MainWindow::showScatterplotForTrials);

    m_logarithmicScale = new QCheckBox(tr("Log Scale"));
    m_logarithmicScale->setToolTip(tr("Use logarithmic scale. If disabled linear scale is used."));
    QObject::connect(m_logarithmicScale, &QCheckBox::clicked,
                     m_window, &MainWindow::showScatterplotForTrials);

    gridLayoutScatterplotSettings->addWidget(scatterplotXAxisLabel, 0, 0);
    gridLayoutScatterplotSettings->addWidget(m_scatterplotXAxisComboBox, 0, 1);
    gridLayoutScatterplotSettings->addWidget(scatterplotYAxisLabel, 1, 0);
    gridLayoutScatterplotSettings->addWidget(m_scatterplotYAxisComboBox, 1, 1);
    gridLayoutScatterplotSettings->addWidget(scatterplotMaxXLabel, 2, 0);
    gridLayoutScatterplotSettings->addWidget(m_scatterplotMaxXSpinBox, 2, 1);
    gridLayoutScatterplotSettings->addWidget(scatterplotMaxYLabel, 3, 0);
    gridLayoutScatterplotSettings->addWidget(m_scatterplotMaxYSpinBox, 3, 1);
    gridLayoutScatterplotSettings->addWidget(m_logarithmicScale, 4, 0, 1, 2);
    gridLayoutScatterplotSettings->addWidget(m_scatterplot, 5, 0, 1, 2);

    //QGroupBox* scatterplotGroupBox = new QGroupBox(m_window);
    //scatterplotGroupBox->setLayout(gridLayoutScatterplotSettings);

    m_diagramDockWidgetLayout->addLayout(gridLayoutScatterplotSettings);

    m_diagramsLegend = new DiagramLegend(m_settings);
    m_diagramsLegend->hide();
    m_diagramDockWidgetLayout->addWidget(m_diagramsLegend);

    //m_diagramDockWidgetLayout->addStretch(1);

    // dock
    QWidget* diagramWidget = new QWidget(m_window);
    diagramWidget->setLayout(m_diagramDockWidgetLayout);

    scrollArea->setWidget(diagramWidget);
    scrollArea->setWidgetResizable(true);

    m_diagramDockWidget = new QDockWidget(tr("Diagrams"), m_window);
    m_diagramDockWidget->setWidget(scrollArea);

    m_diagramDockWidget->setObjectName(tr("DiagramDockWidget"));

    updateHistogramOptions();
}


void UserInterface::updateColorButton(QColor* color, QPushButton* button)
{
    QString colorName = QString("rgba(%1, %2, %3, %4)").arg(QString::number((*color).red()),
                                                            QString::number((*color).green()),
                                                            QString::number((*color).blue()),
                                                            QString::number((*color).alpha()));
    button->setText(colorName);

    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, *color);

    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->setFlat(true);

    m_window->updateUI();
}


void UserInterface::showColorDialog()
{
    QPushButton* sender = static_cast<QPushButton*>(QObject::sender());

    QColorDialog* dialog = new QColorDialog();
    QColor* color = buttonsToColorMapping.value(sender);

    QColor newColor = dialog->getColor(*color, m_window, "Change Color",
                                       QColorDialog::ShowAlphaChannel);

    if (!newColor.isValid())
    {
        return;
    }

    *color = newColor;

    // update button and timeline, ...
    updateColorButton(color, sender);
}


void UserInterface::updateDataPlot(const std::vector<Trial*> trials,
                                   const std::vector<std::vector<Fixation*> > fixationsForTrials)
{
    if (m_settings->m_dataPlotType == PT_RadialScatterPlot ||
            m_settings->m_dataPlotType == PT_RadialConnectedScatterPlot)
    {
        RadialScatterplot* plot = new RadialScatterplot(m_settings,
                                                        m_settings->m_dataPlotType == PT_RadialConnectedScatterPlot);
        newDataPlot(plot);
        plot->updatePlot(trials, fixationsForTrials);
    }
    else if (m_settings->m_dataPlotType == PT_RegularPolarPlot ||
             m_settings->m_dataPlotType == PT_PolarPlotWithHole)
    {
        PolarPlot* plot = new PolarPlot(m_settings,
                                        m_settings->m_dataPlotType == PT_PolarPlotWithHole);
        newDataPlot(plot);
        plot->updatePlot(trials, fixationsForTrials);
    }
    else if (m_settings->m_dataPlotType == PT_RosePlot ||
             m_settings->m_dataPlotType == PT_RosePlotWithHole ||
             m_settings->m_dataPlotType == PT_BoxRosePlot ||
             m_settings->m_dataPlotType == PT_BoxRosePlotWithHole)
    {
        RosePlot* plot = new RosePlot(m_settings, m_settings->m_dataPlotType == PT_RosePlotWithHole ||
                                      m_settings->m_dataPlotType == PT_BoxRosePlotWithHole,
                                      m_settings->m_dataPlotType == PT_RosePlot ||
                                      m_settings->m_dataPlotType == PT_RosePlotWithHole);
        newDataPlot(plot);
        plot->updatePlot(trials, fixationsForTrials);
    }
    else if (m_settings->m_dataPlotType == PT_RadialRawDataPlot)
    {
        EyeGazePlot* plot = new EyeGazePlot(m_settings);
        newDataPlot(plot);
        plot->updatePlot(trials, fixationsForTrials);
    }
    else if (m_settings->m_dataPlotType == PT_HistogramBarPlot)
    {
        EyeDirectionHistogram* plot = new EyeDirectionHistogram(m_settings);
        newDataPlot(plot);
        plot->updatePlot(trials, fixationsForTrials);
    }
}


void UserInterface::newDataPlot(AbstractPlot* newPlot)
{
    if (m_dataPlot != nullptr && m_dataPlotDockWidgetLayout->indexOf(m_dataPlot) != -1)
    {
        m_dataPlot->hide();
        m_dataPlotDockWidgetLayout->removeWidget(m_dataPlot);
    }

    if (m_dataPlot != nullptr)
    {
        delete m_dataPlot;
    }

    m_dataPlot = newPlot;
    m_dataPlot->setMinimumHeight(250);
    m_dataPlotDockWidgetLayout->insertWidget(1, m_dataPlot);
}


void UserInterface::selectParticipant(Participant* participant)
{
    int previousTrialIndex = m_trialCombobox->currentIndex();

    QObject::disconnect(m_trialCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                        m_window, &MainWindow::trialChanged);
    m_trialCombobox->clear();

    for (size_t i = 0; i < participant->numberOfTrials(); ++i)
    {
        m_trialCombobox->addItem(participant->getTrialAt(int(i))->name());
    }

    QObject::connect(m_trialCombobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                        m_window, &MainWindow::trialChanged);
    if (previousTrialIndex >= 0 && m_trialCombobox->count() > previousTrialIndex)
    {
        m_trialCombobox->setCurrentIndex(previousTrialIndex);
    }

    m_window->showData();
}


void UserInterface::updateValuesFromSettings()
{
    // ** filter settings **
    m_backgroundOpacitySlider->setValue(m_settings->m_backgroundOpacity);
    m_followingFixationsCountSpinBox->setValue(m_settings->m_followingFixationsCount);
    m_showNeighboringFixationsCheckbox->setChecked(m_settings->m_showNeighboringFixations);
    m_previousFixationsCountSpinBox->setValue(m_settings->m_previousFixationsCount);

    m_showScanpathCheckBox->setChecked(m_settings->m_showScanpath);

    m_showMicrosaccadesCheckbox->setChecked(m_settings->m_showMicrosaccades);

    m_showSamplesCheckbox->setChecked(m_settings->m_showSamples);
    m_showSampleConnectionsCheckbox->setChecked(m_settings->m_showSampleConnections);
    m_showMicrosaccadeDirectionsCheckbox->setChecked(m_settings->m_showMicrosaccadeDirections);

    m_highlightFixationsCheckbox->setChecked(m_settings->m_highlightFixations);
    m_highlightCurrentFixationCheckbox->setChecked(m_settings->m_highlightCurrentFixation);

    m_fixationSizeComboBox->setCurrentIndex(-1);
    m_fixationSizeComboBox->setCurrentIndex(m_settings->m_fixationSize);
    m_fixationSizeSlider->setValue(m_settings->m_fixationScale);

    m_radioSingle->setChecked(m_settings->m_displayMode == Individual);

    m_showSaccadeDirectionsCheckbox->setChecked(m_settings->m_showSaccadeDirections);

    m_showMicrosaccadesInTimelineCheckBox->setChecked(m_settings->m_showMicrosaccadesInTimeline);
    m_showFixationsInTimelineCheckBox->setChecked(m_settings->m_showFixationsInTimeline);
    m_showEventsInTimelineCheckBox->setChecked(m_settings->m_showEventsInTimeline);
    m_showXValuesInTimelineCheckBox->setChecked(m_settings->m_showXValuesInTimeline);
    m_showYValuesInTimelineCheckBox->setChecked(m_settings->m_showYValuesInTimeline);
    m_showVelocityValuesInTimelineCheckBox->setChecked(m_settings->m_showVelocityValuesInTimeline);

    m_gazeDataComboBox->setCurrentIndex(m_settings->m_gazeType);
    m_timeLimitCheckBox->setChecked(m_settings->m_limitTimeRange);
    m_startTimeLimitSpinBox->setValue(m_settings->m_startTimeInMSec);
    m_endTimeLimitSpinBox->setValue(m_settings->m_endTimeInMSec);

    // ** microsaccade detection **
    m_microsaccadesFromInputFileCheckbox->setChecked(m_settings->m_microsaccadeSettings->m_fromInputFile);
    m_msVelocityThresholdSpinBox->setValue(m_settings->m_microsaccadeSettings->m_velocityThreshold);
    m_msMinDurationSpinBox->setValue(m_settings->m_microsaccadeSettings->m_minDuration);
    m_msVelocityWindowSizeSpinBox->setValue(int(m_settings->m_microsaccadeSettings->m_velocityWindowSize));
    m_msIgnoreBeforeMissingDataSpinBox->setValue(int(m_settings->m_microsaccadeSettings->m_ignoreBeforeMissingData));
    m_msIgnoreAfterMissingDataSpinBox->setValue(int(m_settings->m_microsaccadeSettings->m_ignoreAfterMissingData));
    m_msBinocularCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_binocular);
    m_msMaxDurationSpinBox->setValue(m_settings->m_microsaccadeSettings->m_maxDuration);
    m_msMaxAmplitudeSpinBox->setValue(m_settings->m_microsaccadeSettings->m_maxAmplitude);
    m_msMinAmplitudeSpinBox->setValue(m_settings->m_microsaccadeSettings->m_minAmplitude);
    m_msMinIntersaccIntervalSpinBox->setValue(m_settings->m_microsaccadeSettings->m_minIntersaccInterval);
    m_msMinVelocitySpinBox->setValue(m_settings->m_microsaccadeSettings->m_minVelocity);
    m_msMaxVelocitySpinBox->setValue(m_settings->m_microsaccadeSettings->m_maxVelocity);
    m_msIgnoreAtFixationStartSpinBox->setValue(m_settings->m_microsaccadeSettings->m_ignoreAtStart);
    m_msIgnoreAtFixationEndSpinBox->setValue(m_settings->m_microsaccadeSettings->m_ignoreAtEnd);

    m_msMaxDurationCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMaxDuration);
    m_msMinAmplitudeCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMinAmplitude);
    m_msMaxAmplitudeCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMaxAmplitude);
    m_msMinIntersaccIntervalCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMinIntersaccInterval);
    m_msMinVelocityCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMinVelocity);
    m_msMaxVelocityCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useMaxVelocity);
    m_msIgnoreAtFixationStartCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useIgnoreAtStart);
    m_msIgnoreAtFixationEndCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useIgnoreAtEnd);
    m_msIgnoreBeforeMissingDataCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useIgnoreBeforeMissingData);
    m_msIgnoreAfterMissingDataCheckBox->setChecked(m_settings->m_microsaccadeSettings->m_useIgnoreAfterMissingData);

    // ** data plot **
    m_dataPlotBinsSpinBox->setValue(m_settings->m_dataPlotBins);
    m_dataPlotRadiusSpinBox->setValue(m_settings->m_dataPlotRadiusCount);
    m_aggregatePlotDataComboBox->setCurrentIndex(m_settings->m_overplot);
    m_testConditionColorsCheckbox->setChecked(m_settings->m_testConditionColors);

    m_dataPlotFixationsComboBox->setCurrentIndex(m_settings->m_dataPlotFixations);
    m_dataPlotDataComboBox->setCurrentIndex(m_settings->m_dataPlotData);
    m_dataPlotDirectionComboBox->setCurrentIndex(m_settings->m_dataPlotDirection);
    m_dataPlotTypeComboBox->setCurrentIndex(m_settings->m_dataPlotType > 8 ? 2 : (m_settings->m_dataPlotType > 6) ? 1 : 0);
    m_dataDirectionPlotVisTypeComboBox->setCurrentIndex(m_settings->m_dataPlotType);
    m_dataScatterplotVisTypeComboBox->setCurrentIndex(m_settings->m_dataPlotType == PT_RadialConnectedScatterPlot ? 1 : 0);

    // ** histogram **
    m_histogramYAxisCombooBox->setCurrentIndex(m_settings->m_histogramYAxis);
    m_dataComboBox->setCurrentIndex(m_settings->m_histogramAndScatterplotData);
    m_histogramStartTimeBinSizeSpinBox->setValue(m_settings->m_histogramStartTimeBinSize);
    m_histogramPeakVelocityBinSizeSpinBox->setValue(m_settings->m_histogramPeakVelocityBinSize);
    m_histogramAmplitudeBinSizeSpinBox->setValue(m_settings->m_histogramAmplitudeBinSize);
    m_histogramDurationBinSizeSpinBox->setValue(m_settings->m_histogramDurationBinSize);
    m_histogramMaxXSpinBox->setValue(m_settings->m_histogramMaxX);
    m_histogramMaxYSpinBox->setValue(m_settings->m_histogramMaxY);

    // ** scatterplot **
    m_scatterplotXAxisComboBox->setCurrentIndex(m_settings->m_scatterplotXAxis);
    m_scatterplotYAxisComboBox->setCurrentIndex(m_settings->m_scatterplotYAxis);
    m_scatterplotMaxXSpinBox->setValue(m_settings->m_scatterplotMaxX);
    m_scatterplotMaxYSpinBox->setValue(m_settings->m_scatterplotMaxY);
    m_logarithmicScale->setChecked(m_settings->m_logarithmicScale);

    // ** fixations detection **
    m_fixationsFromInputFileCheckbox->setChecked(m_settings->m_fixationSettings->m_fromInputFile);
    m_fixVelocityThresholdSpinBox->setValue(m_settings->m_fixationSettings->m_velocityThreshold);
    m_fixMinSaccadeDurationSpinBox->setValue(m_settings->m_fixationSettings->m_minDuration);
    m_fixBinocularCheckBox->setChecked(m_settings->m_fixationSettings->m_binocular);
    m_fixVelocityWindowSizeSpinBox->setValue(int(m_settings->m_fixationSettings->m_velocityWindowSize));
    m_fixIgnoreBeforeMissingDataSpinBox->setValue(int(m_settings->m_fixationSettings->m_ignoreBeforeMissingData));
    m_fixIgnoreAfterMissingDataSpinBox->setValue(int(m_settings->m_fixationSettings->m_ignoreAfterMissingData));
    m_fixMaxDurationSpinBox->setValue(m_settings->m_fixationSettings->m_maxDuration);
    m_fixMaxSaccadeAmplitudeSpinBox->setValue(m_settings->m_fixationSettings->m_maxAmplitude);
    m_fixMinSaccadeAmplitudeSpinBox->setValue(m_settings->m_fixationSettings->m_minAmplitude);
    m_fixMinSaccadeIntersaccIntervalSpinBox->setValue(m_settings->m_fixationSettings->m_minIntersaccInterval);
    m_fixMinVelocitySpinBox->setValue(m_settings->m_fixationSettings->m_minVelocity);
    m_fixMaxVelocitySpinBox->setValue(m_settings->m_fixationSettings->m_maxVelocity);
    m_fixIgnoreAtStartSpinBox->setValue(m_settings->m_fixationSettings->m_ignoreAtStart);
    m_fixIgnoreAtEndSpinBox->setValue(m_settings->m_fixationSettings->m_ignoreAtEnd);

    m_fixMaxDurationCheckBox->setChecked(m_settings->m_fixationSettings->m_useMaxDuration);
    m_fixMinAmplitudeCheckBox->setChecked(m_settings->m_fixationSettings->m_useMinAmplitude);
    m_fixMaxAmplitudeCheckBox->setChecked(m_settings->m_fixationSettings->m_useMaxAmplitude);
    m_fixMinIntersaccIntervalCheckBox->setChecked(m_settings->m_fixationSettings->m_useMinIntersaccInterval);
    m_fixMinVelocityCheckBox->setChecked(m_settings->m_fixationSettings->m_useMinVelocity);
    m_fixMaxVelocityCheckBox->setChecked(m_settings->m_fixationSettings->m_useMaxVelocity);
    m_fixIgnoreAtFixationStartCheckBox->setChecked(m_settings->m_fixationSettings->m_useIgnoreAtStart);
    m_fixIgnoreAtFixationEndCheckBox->setChecked(m_settings->m_fixationSettings->m_useIgnoreAtEnd);
    m_fixIgnoreBeforeMissingDataCheckBox->setChecked(m_settings->m_fixationSettings->m_useIgnoreBeforeMissingData);
    m_fixIgnoreAfterMissingDataCheckBox->setChecked(m_settings->m_fixationSettings->m_useIgnoreAfterMissingData);

    // further settings
    m_activateZoomToFixationAreaCheckBox->setChecked(m_settings->m_activateZoomToFixationArea);
}


void UserInterface::updateMicrosaccadeSettings(Trial& trial)
{
    if (trial.getMsFilterSettings() == nullptr)
    {
       m_microsaccadesFromInputFileCheckbox->setChecked(true);
    }
    else
    {
        SaccadeFilterSettings* settings = trial.getMsFilterSettings();
        m_microsaccadesFromInputFileCheckbox->setChecked(settings->m_fromInputFile);
        m_msVelocityThresholdSpinBox->setValue(settings->m_velocityThreshold);
        m_msMinDurationSpinBox->setValue(settings->m_minDuration);
        m_msBinocularCheckBox->setChecked(settings->m_binocular);
        m_msVelocityWindowSizeSpinBox->setValue(int(settings->m_velocityWindowSize));
        m_msIgnoreBeforeMissingDataSpinBox->setValue(settings->m_ignoreBeforeMissingData);
        m_msIgnoreAfterMissingDataSpinBox->setValue(settings->m_ignoreAfterMissingData);
        m_msMaxDurationSpinBox->setValue(settings->m_maxDuration);
        m_msMaxAmplitudeSpinBox->setValue(settings->m_maxAmplitude);
        m_msMinAmplitudeSpinBox->setValue(settings->m_minAmplitude);
        m_msMinIntersaccIntervalSpinBox->setValue(settings->m_minIntersaccInterval);
        m_msMinVelocitySpinBox->setValue(settings->m_minVelocity);
        m_msMaxVelocitySpinBox->setValue(settings->m_maxVelocity);
        m_msIgnoreAtFixationStartSpinBox->setValue(settings->m_ignoreAtStart);
        m_msIgnoreAtFixationEndSpinBox->setValue(settings->m_ignoreAtEnd);

        m_msMaxDurationCheckBox->setChecked(settings->m_useMaxDuration);
        m_msMinAmplitudeCheckBox->setChecked(settings->m_useMinAmplitude);
        m_msMaxAmplitudeCheckBox->setChecked(settings->m_useMaxAmplitude);
        m_msMinIntersaccIntervalCheckBox->setChecked(settings->m_useMinIntersaccInterval);
        m_msMinVelocityCheckBox->setChecked(settings->m_useMinVelocity);
        m_msMaxVelocityCheckBox->setChecked(settings->m_useMaxVelocity);
        m_msIgnoreAtFixationStartCheckBox->setChecked(settings->m_useIgnoreAtStart);
        m_msIgnoreAtFixationEndCheckBox->setChecked(settings->m_useIgnoreAtEnd);
        m_msIgnoreBeforeMissingDataCheckBox->setChecked(settings->m_useIgnoreBeforeMissingData);
        m_msIgnoreAfterMissingDataCheckBox->setChecked(settings->m_useIgnoreAfterMissingData);
    }
}


void UserInterface::updateFixationsSettings(Trial& trial)
{

   if (trial.getFixationFilterSettings() == nullptr)
   {
      m_fixationsFromInputFileCheckbox->setChecked(true);
   }
   else
   {
       SaccadeFilterSettings* settings = trial.getFixationFilterSettings();
       m_fixationsFromInputFileCheckbox->setChecked(settings->m_fromInputFile);
       m_fixVelocityThresholdSpinBox->setValue(settings->m_velocityThreshold);
       m_fixMinSaccadeDurationSpinBox->setValue(settings->m_minDuration);
       m_fixBinocularCheckBox->setChecked(settings->m_binocular);
       m_fixVelocityWindowSizeSpinBox->setValue(int(settings->m_velocityWindowSize));
       m_fixIgnoreBeforeMissingDataSpinBox->setValue(settings->m_ignoreBeforeMissingData);
       m_fixIgnoreAfterMissingDataSpinBox->setValue(settings->m_ignoreAfterMissingData);
       m_fixMaxDurationSpinBox->setValue(settings->m_maxDuration);
       m_fixMaxSaccadeAmplitudeSpinBox->setValue(settings->m_maxAmplitude);
       m_fixMinSaccadeAmplitudeSpinBox->setValue(settings->m_minAmplitude);
       m_fixMinSaccadeIntersaccIntervalSpinBox->setValue(settings->m_minIntersaccInterval);
       m_fixMinVelocitySpinBox->setValue(settings->m_minVelocity);
       m_fixMaxVelocitySpinBox->setValue(settings->m_maxVelocity);
       m_fixIgnoreAtStartSpinBox->setValue(settings->m_ignoreAtStart);
       m_fixIgnoreAtEndSpinBox->setValue(settings->m_ignoreAtEnd);

       m_fixMaxDurationCheckBox->setChecked(settings->m_useMaxDuration);
       m_fixMinAmplitudeCheckBox->setChecked(settings->m_useMinAmplitude);
       m_fixMaxAmplitudeCheckBox->setChecked(settings->m_useMaxAmplitude);
       m_fixMinIntersaccIntervalCheckBox->setChecked(settings->m_useMinIntersaccInterval);
       m_fixMinVelocityCheckBox->setChecked(settings->m_useMinVelocity);
       m_fixMaxVelocityCheckBox->setChecked(settings->m_useMaxVelocity);
       m_fixIgnoreAtFixationStartCheckBox->setChecked(settings->m_useIgnoreAtStart);
       m_fixIgnoreAtFixationEndCheckBox->setChecked(settings->m_useIgnoreAtEnd);
       m_fixIgnoreBeforeMissingDataCheckBox->setChecked(settings->m_useIgnoreBeforeMissingData);
       m_fixIgnoreAfterMissingDataCheckBox->setChecked(settings->m_useIgnoreAfterMissingData);
   }
}


QHBoxLayout* UserInterface::getCheckButtons(QListWidget* list)
{
    QPushButton* checkButton = new QPushButton(tr("Check All"), m_window);
    QPushButton* uncheckButton = new QPushButton(tr("Uncheck All"), m_window);
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(checkButton);
    hLayout->addWidget(uncheckButton);

    connect(checkButton, &QPushButton::clicked, [this, list](){ checkAllListEntries(static_cast<QWidget*>(list)); });
    connect(uncheckButton, &QPushButton::clicked, [this, list](){ uncheckAllListEntries(static_cast<QWidget*>(list)); });

    return hLayout;
}


void UserInterface::changeDisplayMode(bool single)
{
    if (single)
    {
        m_settings->m_displayMode = Individual;
        m_formWidgetMulti->hide();
        m_formWidgetSingle->show();
        m_fixationsDockWidget->show();
        m_microsaccadesDockWidget->show();
    }
    else
    {
        m_settings->m_displayMode = Multi;
        m_fixationsDockWidget->hide();
        m_microsaccadesDockWidget->hide();
        m_formWidgetSingle->hide();
        m_formWidgetMulti->show();
    }
}


void UserInterface::showDialog(const QString title, const QString fileName)
{

    QFile f(fileName);
    if (f.open(QFile::ReadOnly | QFile::Text))
    {
        QDialog* dialog = new QDialog(m_window);
        dialog->setWindowTitle(title);
        dialog->setGeometry(m_window->x() + m_window->width()/2 - 600/2,
                            m_window->y() + m_window->height()/2 - 600/2,
                            600, 600);

        QTextEdit* textBox = new QTextEdit();
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        textBox->setFont(font);

        QTextStream in(&f);

        textBox->setText(in.readAll());

        QVBoxLayout* layout = new QVBoxLayout();

        layout->addWidget(textBox);

        dialog->setLayout(layout);

        dialog->show();
    }
}


void UserInterface::showAboutDialog()
{
    QMessageBox::about(m_window, tr("About VisME - Visual Microsaccades Explorer"),
    tr("In this application eye tracking data can be visually explored. There is a focus on "
       "analysis of fixations especially considering microsaccades.\n"
       "\n"
       "This application was developed as part of a research project. "
       "It may contain errors and behave unexpected.\n"
       "\n"
       "Implemented in C++ with Qt 5.9.\n"
       "\n"
       "2019-07-26\n"
       "Tanja Munz\n"));
}


void UserInterface::showInputFormats()
{
    showDialog("VisME - Input Formats", ":readmeFiles/inputFormats.txt");
}


void UserInterface::showFeaturesDialog()
{
    showDialog("VisME - Features", ":readmeFiles/features.txt");
}


void UserInterface::checkAllListEntries(QWidget* list)
{
    QListWidget* l = dynamic_cast<QListWidget*>(list);

    for (int i = 0; i < l->count(); ++i)
    {
        l->item(i)->setCheckState(Qt::Checked);
    }
}


void UserInterface::uncheckAllListEntries(QWidget* list)
{
    QListWidget* l = dynamic_cast<QListWidget*>(list);

    for (int i = 0; i < l->count(); ++i)
    {
        l->item(i)->setCheckState(Qt::Unchecked);
    }
}


void UserInterface::setTestConditionsInUI(std::set<QString> testConditions)
{
    m_settings->m_testConditions.clear();

    ColorGenerator* colorGenerator = new ColorGenerator();

    std::vector<QString> sortedTestConditions(testConditions.begin(), testConditions.end());
    std::sort(sortedTestConditions.begin(), sortedTestConditions.end());

    if (!sortedTestConditions.empty())
    {
        foreach (const QString &value, sortedTestConditions)
        {
            QColor color = colorGenerator->nextGoldenRatioColor();
            // color.setHsv(int(qreal(count) / qreal(testConditions.size()) * 255 + 180) % 256, 255, 255, 255);

            m_settings->m_testConditions.insert(value, color);
        }
    }
    m_window->addTestConditionsInUI();
    m_window->updateMicrosaccadeStatBarsInUI();
}


void UserInterface::updateHistogramOptions()
{
    if (m_settings->m_histogramYAxis == MSStart)
    {
        m_histogramPeakVelocityBinsLabel->hide();
        m_histogramPeakVelocityBinSizeSpinBox->hide();
        m_histogramAmplitudeBinsLabel->hide();
        m_histogramAmplitudeBinSizeSpinBox->hide();
        m_histogramDurationBinsLabel->hide();
        m_histogramDurationBinSizeSpinBox->hide();

        m_histogramStartTimeBinsLabel->show();
        m_histogramStartTimeBinSizeSpinBox->show();
    }
    else if (m_settings->m_histogramYAxis == PeakVelocity)
    {
        m_histogramStartTimeBinsLabel->hide();
        m_histogramStartTimeBinSizeSpinBox->hide();
        m_histogramAmplitudeBinsLabel->hide();
        m_histogramAmplitudeBinSizeSpinBox->hide();
        m_histogramDurationBinsLabel->hide();
        m_histogramDurationBinSizeSpinBox->hide();

        m_histogramPeakVelocityBinsLabel->show();
        m_histogramPeakVelocityBinSizeSpinBox->show();
    }
    else if (m_settings->m_histogramYAxis == Amplitude)
    {
        m_histogramStartTimeBinsLabel->hide();
        m_histogramStartTimeBinSizeSpinBox->hide();
        m_histogramPeakVelocityBinsLabel->hide();
        m_histogramPeakVelocityBinSizeSpinBox->hide();
        m_histogramDurationBinsLabel->hide();
        m_histogramDurationBinSizeSpinBox->hide();

        m_histogramAmplitudeBinsLabel->show();
        m_histogramAmplitudeBinSizeSpinBox->show();
    }
    else if (m_settings->m_histogramYAxis == Duration)
    {
        m_histogramStartTimeBinsLabel->hide();
        m_histogramStartTimeBinSizeSpinBox->hide();
        m_histogramPeakVelocityBinsLabel->hide();
        m_histogramPeakVelocityBinSizeSpinBox->hide();
        m_histogramAmplitudeBinsLabel->hide();
        m_histogramAmplitudeBinSizeSpinBox->hide();

        m_histogramDurationBinsLabel->show();
        m_histogramDurationBinSizeSpinBox->show();
    }
}
