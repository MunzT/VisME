/************************************************************************************************//**
 * @author Tanja Munz
 * @file mainwindow.cpp
 ***************************************************************************************************/
#include "eyehistogram.h"
#include "eyescatterplot.h"
#include "fileextensions.h"
#include "fixation.h"
#include "fixationitem.h"
#include "gazedata.h"
#include "keypressfilter.h"
#include "mainvisualization.h"
#include "mainwindow.h"
#include "mathhelpfunctions.h"
#include "pngexporter.h"
#include "radialscatterplot.h"
#include "saccadedetector.h"
#include "timelineview.h"
#include <iostream>
#include <numeric>
#include <QComboBox>
#include <QGuiApplication>
#include <QMap>
#include <QtWidgets>
#include <set>
#include <sstream>


MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    m_settings(new Settings()),
    m_eyeTrackingData(new EyeTrackingData()),
    m_mainView(new MainView(this)),
    m_timelineView(new TimelineView(this)),
    m_mainScene(nullptr),
    m_timelineScene(nullptr),
    m_ui(nullptr)
{

    m_mainVisualization = new MainVisualization(m_settings);
    m_timelineVisualization = new TimelineVisualization(m_settings);

    // location of settings file is in the directory of the executable file
    m_settingsPath = QDir(qApp->applicationDirPath()).filePath("settings.ini");

    auto keyPressFilter = new KeyPressFilter();
    QApplication::instance()->installEventFilter(keyPressFilter);
    QObject::connect(keyPressFilter, &KeyPressFilter::nextEvent,
                     this, &MainWindow::selectNextFixation);
    QObject::connect(keyPressFilter, &KeyPressFilter::previousEvent,
                     this, &MainWindow::selectPreviousFixation);

    setWindowIcon(QIcon(":images/icon.jpg"));

    // https://wiki.qt.io/How_to_Center_a_Window_on_the_Screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1800,800),
            QGuiApplication::screens().first()->geometry()
        )
    );

    setAcceptDrops(true);

    QObject::connect(m_mainVisualization, &MainVisualization::fixationSelected,
                     this, &MainWindow::fixationSelected);

    QObject::connect(m_timelineVisualization, &TimelineVisualization::fixationSelected,
                     this, &MainWindow::fixationSelected);

    QObject::connect(m_timelineView, &TimelineView::timelineSelection,
                     this, &MainWindow::limitTimeRangeToSelection);

    m_ui = new UserInterface(this, m_settings, m_mainView, m_timelineView);

    // initialize the scenes
    newScenes();   
}


MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_mainVisualization;
    delete m_timelineVisualization;
    delete m_ui;
    delete m_eyeTrackingData;
}


void MainWindow::openFiles(const QStringList& fileNames, bool add)
{
    qApp->processEvents();
    EyeDataImport eyeDataImport(this, m_eyeTrackingData, m_settings);
    connect(&eyeDataImport, &EyeDataImport::setTestConditionsInUI, m_ui, &UserInterface::setTestConditionsInUI);
    connect(&eyeDataImport, &EyeDataImport::updateParticipantsInUI, this, &MainWindow::addParticipantsInUI);
    eyeDataImport.openFiles(fileNames, add);
}


void MainWindow::showData(bool resetTimeline)
{
    if (m_ui == nullptr || m_eyeTrackingData->numberOfParticipants() == 0)
    {
        return;
    }

    int p = m_ui->m_participantCombobox->currentIndex();

    determineTrials();

    if (m_settings->m_currentTrials.empty())
    {
        m_mainVisualization->updateTrial();
        m_timelineVisualization->updateTrial();
        m_mainView->updateAll();
        m_timelineView->updateAll();
        m_ui->m_fixationsTable->clear();
        updateFixationInfo(nullptr);
        updateGeneralInfo(std::vector<Trial*>());
        updateStatisticsInfo(std::vector<Trial*>());

        // plots
        showDataPlotForTrials();
        showHistogramForTrials();
        showScatterplotForTrials();
        return;
    }

    setWindowTitle(QString("%1 - %2").arg(
                       QApplication::applicationName(),
                       QString(QFileInfo(QFile(m_eyeTrackingData->participantAt(p)->filePath()).fileName()).fileName())));

    m_ui->updateMicrosaccadeSettings(*m_settings->m_currentTrials[0]);
    m_ui->updateFixationsSettings(*m_settings->m_currentTrials[0]);

    m_mainVisualization->updateTrial();
    m_timelineVisualization->updateTrial();

    if (m_settings->m_displayMode == Individual)
    {
        m_ui->m_updateMicrosaccadesButton->setEnabled(true);
        m_ui->m_updateCurrentParticipantMicrosaccadesButton->setEnabled(true);
        m_ui->m_updateFixationsButton->setEnabled(true);
        m_ui->m_updateFixationsForParticipantButton->setEnabled(true);
        m_ui->m_viewGroupBoxTimeline->setEnabled(true);
        m_ui->m_viewGroupBoxMainView->setEnabled(true);
        m_ui->m_viewGroupBoxFixations->setEnabled(true);
        m_ui->m_dataPlotFixationsComboBox->setEnabled(true);
        fixationSelected(m_settings->m_currentTrials[0]->getFixationAt(m_settings->m_currentFixationIndex));
    }
    else
    {
        m_ui->m_fixationsTable->setRowCount(0);
        m_ui->m_fixationsTable->setColumnCount(0);
        m_ui->m_microsaccadesTable->setRowCount(0);
        m_ui->m_microsaccadesTable->setColumnCount(0);
        m_ui->m_updateMicrosaccadesButton->setEnabled(false);
        m_ui->m_updateCurrentParticipantMicrosaccadesButton->setEnabled(false);
        m_ui->m_updateFixationsButton->setEnabled(false);
        m_ui->m_updateFixationsForParticipantButton->setEnabled(false);
        m_ui->m_viewGroupBoxTimeline->setEnabled(false);
        m_ui->m_viewGroupBoxMainView->setEnabled(false);
        m_ui->m_viewGroupBoxFixations->setEnabled(false);
        m_ui->m_dataPlotFixationsComboBox->setEnabled(false);
        updateFixationInfo(nullptr);
    }

    updateUI(false);

    m_mainScene->updateSceneRectSize();
    if (resetTimeline)
    {
        m_timelineScene->updateSceneRectSize();
        m_timelineView->resetView();
    }
}


void MainWindow::newScenes()
{
    // delete the old scenes
    if (m_mainScene != nullptr)
    {
        m_mainScene->clear();
        delete m_mainScene;
    }
    if (m_timelineScene != nullptr)
    {
        m_timelineScene->clear();
        delete m_timelineScene;
    }

    // scene for the hierarchy
    m_mainScene = new MainScene(m_mainVisualization, m_settings, this);
    m_ui->m_mainView->setScene(m_mainScene);
    QObject::connect(m_mainScene, &MainScene::fixationSelected,
                     this, &MainWindow::fixationSelected);

    // scene for the WOI River
    m_timelineScene = new TimelineScene(m_settings, this);
    m_ui->m_timelineView->setScene(m_timelineScene);

    m_mainVisualization->setScene(m_mainScene);
    m_timelineVisualization->setScene(m_timelineScene);
}


void MainWindow::updateMicrosaccadesDetection(Trial* trial)
{
    if (trial == nullptr)
    {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    m_settings->m_microsaccadeSettings->m_fromInputFile = m_ui->m_microsaccadesFromInputFileCheckbox->isChecked();

    m_settings->m_microsaccadeSettings->m_velocityThreshold = m_ui->m_msVelocityThresholdSpinBox->value();
    m_settings->m_microsaccadeSettings->m_minDuration = m_ui->m_msMinDurationSpinBox->value();
    m_settings->m_microsaccadeSettings->m_binocular = m_ui->m_msBinocularCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_velocityWindowSize = size_t(m_ui->m_msVelocityWindowSizeSpinBox->value());
    m_settings->m_microsaccadeSettings->m_maxDuration = m_ui->m_msMaxDurationSpinBox->value();
    m_settings->m_microsaccadeSettings->m_maxAmplitude = m_ui->m_msMaxAmplitudeSpinBox->value();
    m_settings->m_microsaccadeSettings->m_minAmplitude = m_ui->m_msMinAmplitudeSpinBox->value();
    m_settings->m_microsaccadeSettings->m_minIntersaccInterval = m_ui->m_msMinIntersaccIntervalSpinBox->value();
    m_settings->m_microsaccadeSettings->m_minVelocity = m_ui->m_msMinVelocitySpinBox->value();
    m_settings->m_microsaccadeSettings->m_maxVelocity = m_ui->m_msMaxVelocitySpinBox->value();
    m_settings->m_microsaccadeSettings->m_ignoreAtStart = m_ui->m_msIgnoreAtFixationStartSpinBox->value();
    m_settings->m_microsaccadeSettings->m_ignoreAtEnd = m_ui->m_msIgnoreAtFixationEndSpinBox->value();
    m_settings->m_microsaccadeSettings->m_ignoreBeforeMissingData = m_ui->m_msIgnoreBeforeMissingDataSpinBox->value();
    m_settings->m_microsaccadeSettings->m_ignoreAfterMissingData = m_ui->m_msIgnoreAfterMissingDataSpinBox->value();

    m_settings->m_microsaccadeSettings->m_useMaxDuration = m_ui->m_msMaxDurationCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useMinAmplitude = m_ui->m_msMinAmplitudeCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useMaxAmplitude = m_ui->m_msMaxAmplitudeCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useMinIntersaccInterval = m_ui->m_msMinIntersaccIntervalCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useMinVelocity = m_ui->m_msMinVelocityCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useMaxVelocity = m_ui->m_msMaxVelocityCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useIgnoreAtStart = m_ui->m_msIgnoreAtFixationStartCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useIgnoreAtEnd = m_ui->m_msIgnoreAtFixationEndCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useIgnoreBeforeMissingData = m_ui->m_msIgnoreBeforeMissingDataCheckBox->isChecked();
    m_settings->m_microsaccadeSettings->m_useIgnoreAfterMissingData = m_ui->m_msIgnoreAfterMissingDataCheckBox->isChecked();

    trial->updateMicrosaccadesSettings();

    if (!m_settings->m_microsaccadeSettings->m_fromInputFile)
    {
        std::vector<std::vector<Saccade>> microsaccades;
        auto m = new SaccadeDetector();

        microsaccades = m->determineMicrosaccades(trial, m_settings);

        // update microsaccades for all gaze types
        std::vector<GazeType> gazeTypes = {GazeType::GT_Right, GazeType::GT_Left, GazeType::GT_Average};
        for (size_t i = 0; i < gazeTypes.size(); ++i)
        {
            // set for fixations and remove ms outside start/end
            trial->setMicrosaccadesForFixations(&microsaccades[i], gazeTypes[i]);

            if (m_settings->m_microsaccadeSettings->m_useMinIntersaccInterval)
            {
                // remove microsaccades inside inter-saccadic interval
                for (int j = 0; j < trial->fixationsCount(gazeTypes[i]); ++j)
                {
                    m->removeMicrosaccadesForIntersaccadicInterval(trial, m_settings, trial->getFixationAt(j, gazeTypes[i]));
                }
            }
        }
        updateMicrosaccadesInFixationTable();
    }

    qDebug() << QString("Participant %1, Trial %2, MS detection: %3 ms").arg(
                    trial->getParticipant()->name(), trial->name(), QString::number(timer.elapsed()));
}


void MainWindow::updateFixationsDetection(Trial* trial)
{
    if (trial == nullptr)
    {
        return;
    }

    m_settings->m_fixationSettings->m_fromInputFile = m_ui->m_fixationsFromInputFileCheckbox->isChecked();

    m_settings->m_fixationSettings->m_velocityThreshold = m_ui->m_fixVelocityThresholdSpinBox->value();
    m_settings->m_fixationSettings->m_minDuration = m_ui->m_fixMinSaccadeDurationSpinBox->value();
    m_settings->m_fixationSettings->m_binocular = m_ui->m_fixBinocularCheckBox->isChecked();
    m_settings->m_fixationSettings->m_velocityWindowSize = size_t(m_ui->m_fixVelocityWindowSizeSpinBox->value());
    m_settings->m_fixationSettings->m_ignoreBeforeMissingData = size_t(m_ui->m_fixIgnoreBeforeMissingDataSpinBox->value());
    m_settings->m_fixationSettings->m_ignoreAfterMissingData = size_t(m_ui->m_fixIgnoreAfterMissingDataSpinBox->value());
    m_settings->m_fixationSettings->m_maxDuration = m_ui->m_fixMaxDurationSpinBox->value();
    m_settings->m_fixationSettings->m_maxAmplitude = m_ui->m_fixMaxSaccadeAmplitudeSpinBox->value();
    m_settings->m_fixationSettings->m_minAmplitude = m_ui->m_fixMinSaccadeAmplitudeSpinBox->value();
    m_settings->m_fixationSettings->m_minIntersaccInterval = m_ui->m_fixMinSaccadeIntersaccIntervalSpinBox->value();
    m_settings->m_fixationSettings->m_minVelocity = m_ui->m_fixMinVelocitySpinBox->value();
    m_settings->m_fixationSettings->m_maxVelocity = m_ui->m_fixMaxVelocitySpinBox->value();
    m_settings->m_fixationSettings->m_ignoreAtStart = m_ui->m_fixIgnoreAtStartSpinBox->value();
    m_settings->m_fixationSettings->m_ignoreAtEnd = m_ui->m_fixIgnoreAtEndSpinBox->value();
    m_settings->m_fixationSettings->m_ignoreBeforeMissingData = m_ui->m_fixIgnoreBeforeMissingDataSpinBox->value();
    m_settings->m_fixationSettings->m_ignoreAfterMissingData = m_ui->m_fixIgnoreAfterMissingDataSpinBox->value();

    m_settings->m_fixationSettings->m_useMaxDuration = m_ui->m_fixMaxDurationCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useMinAmplitude = m_ui->m_fixMinAmplitudeCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useMaxAmplitude = m_ui->m_fixMaxAmplitudeCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useMinIntersaccInterval = m_ui->m_fixMinIntersaccIntervalCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useMinVelocity = m_ui->m_fixMinVelocityCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useMaxVelocity = m_ui->m_fixMaxVelocityCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useIgnoreAtStart = m_ui->m_fixIgnoreAtFixationStartCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useIgnoreAtEnd = m_ui->m_fixIgnoreAtFixationEndCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useIgnoreBeforeMissingData = m_ui->m_fixIgnoreBeforeMissingDataCheckBox->isChecked();
    m_settings->m_fixationSettings->m_useIgnoreAfterMissingData = m_ui->m_fixIgnoreAfterMissingDataCheckBox->isChecked();

    trial->updateFixationSettings();

    if (m_settings->m_fixationSettings->m_fromInputFile)
    {
        trial->setUseFixationsFromInput(true);
    }
    else
    {
        trial->setUseFixationsFromInput(false);

        // update microsaccades for all gaze types
        std::vector<std::vector<Saccade>> saccades;

        SaccadeDetector m = SaccadeDetector();
        saccades = m.determineRegularSaccades(trial, m_settings);

        std::vector<GazeType> gazeTypes = {GazeType::GT_Right, GazeType::GT_Left, GazeType::GT_Average};
        for (size_t k = 0; k < gazeTypes.size(); ++k)
        {
            const QMap<int, GazeData>& g = trial->gazeData(gazeTypes[k]);
            std::vector<Fixation> fixations;

            if (!g.empty())
            {
                int ignoreAtStart = m_settings->m_fixationSettings->m_useIgnoreAtStart ?
                                        int(trial->timeToSamples(m_settings->m_fixationSettings->m_ignoreAtStart)) : 0;
                int ignoreAtEnd = m_settings->m_fixationSettings->m_useIgnoreAtEnd ?
                                      int(trial->timeToSamples(m_settings->m_fixationSettings->m_ignoreAtEnd)) : 0;
                int startIndex = g.firstKey() + ignoreAtStart;
                int endIndex = g.lastKey() - ignoreAtEnd;

                if(startIndex < 0 || endIndex < 0)
                {
                    return;
                }

                int minimumFixLength = 0; // in samples

                // everything that is not a saccade is handled as fixation...
                for (size_t i = 0; i < saccades[k].size() + 1; ++i)
                {
                    // area before first saccade (or after ignore time) and after the last one is added as fixations
                    int start = i == 0 ? startIndex : int(saccades[k][i - 1].getEndIndex()) + 1;
                    int end = i < saccades[k].size() ? (saccades[k][i].getEndIndex() > size_t(endIndex) ?
                                                              endIndex :
                                                              int(saccades[k][i].getOnsetIndex()) - 1) : endIndex;

                    qreal x = 0;
                    qreal y = 0;
                    int count = 0;
                    int s = start;
                    int e = end;
                    for (int j = start; j < end; ++j)
                    {
                        if (g.contains(j))
                        {
                            x += g[j].getXPosition();
                            y += g[j].getYPosition();
                            e = j;
                            s == -1 ? s = j : s;
                            count++;
                        }
                        else
                        {
                            if (count > 0)
                            {
                                if (e - s + 1 >= minimumFixLength)
                                {
                                    x /= count;
                                    y /= count;

                                    Fixation fixation = Fixation(m_settings);
                                    fixation.setStartIndex(s);
                                    fixation.setDuration(e - s + 1);
                                    fixation.setXPosition(x);
                                    fixation.setYPosition(y);

                                    fixations.push_back(fixation);

                                    x = 0;
                                    y = 0;
                                }
                                count = 0;
                                s = -1;
                            }
                        }
                    }

                    if (count > 0)
                    {
                        if (e - s + 1 >= minimumFixLength)
                        {
                            x /= count;
                            y /= count;

                            Fixation fixation = Fixation(m_settings);
                            fixation.setStartIndex(s);
                            fixation.setDuration(e - s + 1);
                            fixation.setXPosition(x);
                            fixation.setYPosition(y);

                            fixations.push_back(fixation);
                        }
                    }

                }
            }

            trial->setSaccades(saccades[k], gazeTypes[k]);
            trial->setFixations(fixations, gazeTypes[k]);
        }
    }

    updateFixationTable();

    if (m_settings->m_displayMode == DisplayMode::Individual)
    {
        m_mainVisualization->updateTrial();
        m_timelineVisualization->updateTrial();
    }

    updateMicrosaccadesDetection(trial); // update ms as well for the new fixations

    if (!m_settings->m_currentTrials.empty() && trial == m_settings->m_currentTrials[0])
    {
        fixationSelected(trial->getFixationAt(m_settings->m_currentFixationIndex));
    }
}


void MainWindow::updateGeneralInfo(std::vector<Trial*> currentTrials)
{
    // update infos in dock widget
    if (currentTrials.empty())
    {
        m_ui->m_numberOfTrialsLabel->setText("-");
        m_ui->m_screenResolutionLabel->setText("-");
        m_ui->m_frequencyValueLabel->setText("-");
    }
    else
    {
        QSet<QString> screenResolutions;
        QString screenResolution;
        QSet<qreal> frequencies;
        QString frequency;

        for (size_t i = 0; i < currentTrials.size(); ++i)
        {
            screenResolutions.insert(QString::number(currentTrials[i]->maxX() -
                                                     currentTrials[i]->minX() + 1) +
                                     " x " +
                                     QString::number(currentTrials[i]->maxY() -
                                                     currentTrials[i]->minY() + 1));
            frequencies.insert(currentTrials[i]->frequencyRate());
        }

        int count = 0;
        foreach (const QString &value, screenResolutions)
        {
            if (count > 0)
            {
                screenResolution += ", ";
            }
            screenResolution += value;
            count++;
        }
        count = 0;
        foreach (const qreal &value, frequencies)
        {
            if (count > 0)
            {
                frequency += ", ";
            }
            frequency += QString::number(value, 'g', 10) + " Hz";
            count++;
        }

        m_ui->m_numberOfTrialsLabel->setText(QString::number(currentTrials.size()));
        m_ui->m_screenResolutionLabel->setText(screenResolution);
        m_ui->m_frequencyValueLabel->setText(frequency);
    }
}


void MainWindow::updateStatisticsInfo(std::vector<Trial*> currentTrials)
{
    // update infos in dock widget
    if (currentTrials.empty())
    {
        m_ui->m_inputSamplesNumberLabelMin->setText("-");
        m_ui->m_inputSamplesNumberLabelMax->setText("-");
        m_ui->m_inputSamplesNumberLabelMean->setText("-");
        m_ui->m_inputSamplesNumberLabelMedian->setText("-");
        m_ui->m_inputSamplesNumberLabelSum->setText("-");

        m_ui->m_durationValueLabelMin->setText("-");
        m_ui->m_durationValueLabelMax->setText("-");
        m_ui->m_durationValueLabelMean->setText("-");
        m_ui->m_durationValueLabelMedian->setText("-");
        m_ui->m_durationValueLabelSum->setText("-");

        m_ui->m_fixationCountLabelMin->setText("-");
        m_ui->m_fixationCountLabelMax->setText("-");
        m_ui->m_fixationCountLabelMean->setText("-");
        m_ui->m_fixationCountLabelMedian->setText("-");
        m_ui->m_fixationCountLabelSum->setText("-");

        m_ui->m_fixationsWithMicrosaccadesCountLabelMin->setText("-");
        m_ui->m_fixationsWithMicrosaccadesCountLabelMax->setText("-");
        m_ui->m_fixationsWithMicrosaccadesCountLabelMean->setText("-");
        m_ui->m_fixationsWithMicrosaccadesCountLabelMedian->setText("-");
        m_ui->m_fixationsWithMicrosaccadesCountLabelSum->setText("-");

        m_ui->m_fixationsWithMicrosaccadesRatioLabelMin->setText("-");
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMax->setText("-");
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMean->setText("-");
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMedian->setText("-");

        m_ui->m_fixationDurationLabelMin->setText("-");
        m_ui->m_fixationDurationLabelMax->setText("-");
        m_ui->m_fixationDurationLabelMean->setText("-");
        m_ui->m_fixationDurationLabelMedian->setText("-");
        m_ui->m_fixationDurationLabelSum->setText("-");

        m_ui->m_fixationDurationWithMSLabelMin->setText("-");
        m_ui->m_fixationDurationWithMSLabelMax->setText("-");
        m_ui->m_fixationDurationWithMSLabelMean->setText("-");
        m_ui->m_fixationDurationWithMSLabelMedian->setText("-");
        m_ui->m_fixationDurationWithMSLabelSum->setText("-");

        m_ui->m_microsaccadesCountLabelMin->setText("-");
        m_ui->m_microsaccadesCountLabelMax->setText("-");
        m_ui->m_microsaccadesCountLabelMean->setText("-");
        m_ui->m_microsaccadesCountLabelMedian->setText("-");
        m_ui->m_microsaccadesCountLabelSum->setText("-");

        m_ui->m_microsaccadesPerFixationLabelMin->setText("-");
        m_ui->m_microsaccadesPerFixationLabelMax->setText("-");
        m_ui->m_microsaccadesPerFixationLabelMean->setText("-");
        m_ui->m_microsaccadesPerFixationLabelMedian->setText("-");
        m_ui->m_microsaccadesPerFixationLabelSum->setText("-");

        m_ui->m_microsaccadesPerFixationWithMSLabelMin->setText("-");
        m_ui->m_microsaccadesPerFixationWithMSLabelMax->setText("-");
        m_ui->m_microsaccadesPerFixationWithMSLabelMean->setText("-");
        m_ui->m_microsaccadesPerFixationWithMSLabelMedian->setText("-");
        m_ui->m_microsaccadesPerFixationWithMSLabelSum->setText("-");

        m_ui->m_microsaccadesPerSecondLabelMin->setText("-");
        m_ui->m_microsaccadesPerSecondLabelMax->setText("-");
        m_ui->m_microsaccadesPerSecondLabelMean->setText("-");
        m_ui->m_microsaccadesPerSecondLabelMedian->setText("-");

        m_ui->m_microsaccadesPerSecondInFixationsLabelMin->setText("-");
        m_ui->m_microsaccadesPerSecondInFixationsLabelMax->setText("-");
        m_ui->m_microsaccadesPerSecondInFixationsLabelMean->setText("-");
        m_ui->m_microsaccadesPerSecondInFixationsLabelMedian->setText("-");

        m_ui->m_microsaccadeAmplitudeLabelMin->setText("-");
        m_ui->m_microsaccadeAmplitudeLabelMax->setText("-");
        m_ui->m_microsaccadeAmplitudeLabelMean->setText("-");
        m_ui->m_microsaccadeAmplitudeLabelMedian->setText("-");

        m_ui->m_microsaccadesIntersaccIntervalLabelMin->setText("-");
        m_ui->m_microsaccadesIntersaccIntervalLabelMax->setText("-");
        m_ui->m_microsaccadesIntersaccIntervalLabelMean->setText("-");
        m_ui->m_microsaccadesIntersaccIntervalLabelMedian->setText("-");

        m_ui->m_microsaccadeDurationLabelMin->setText("-");
        m_ui->m_microsaccadeDurationLabelMax->setText("-");
        m_ui->m_microsaccadeDurationLabelMean->setText("-");
        m_ui->m_microsaccadeDurationLabelMedian->setText("-");

        m_ui->m_microsaccadeVelocityLabelMin->setText("-");
        m_ui->m_microsaccadeVelocityLabelMax->setText("-");
        m_ui->m_microsaccadeVelocityLabelMean->setText("-");
        m_ui->m_microsaccadeVelocityLabelMedian->setText("-");
    }
    else
    {
        std::vector<qreal> rawDataSamples;
        qreal rawDataSamplesSum = 0;
        qreal rawDataSamplesMin = 0;
        qreal rawDataSamplesMax = 0;
        qreal rawDataSamplesMean = 0;
        qreal rawDataSamplesMedian = 0;

        std::vector<qreal> durationsInSec;
        qreal durationSum = 0;
        qreal durationMin = 0;
        qreal durationMax = 0;
        qreal durationMean = 0;
        qreal durationMedian = 0;

        std::vector<int> fixationCounts;
        int fixCountSum = 0;
        qreal fixCountMin = 0;
        qreal fixCountMax = 0;
        qreal fixCountMean = 0;
        qreal fixCountMedian = 0;

        std::vector<int> fixationWithMSCounts;
        int fixWithMSCountSum = 0;
        qreal fixWithMSCountMin = 0;
        qreal fixWithMSCountMax = 0;
        qreal fixWithMSCountMean = 0;
        qreal fixWithMSCountMedian = 0;

        std::vector<qreal> fixationWithMSRatio;
        qreal fixWithMSRatioMin = 0;
        qreal fixWithMSRatioMax = 0;
        qreal fixWithMSRatioMean = 0;
        qreal fixWithMSRatioMedian = 0;

        std::vector<qreal> fixDurationsInSec;
        qreal fixDurationSum = 0;
        qreal fixDurationMin = 0;
        qreal fixDurationMax = 0;
        qreal fixDurationMean = 0;
        qreal fixDurationMedian = 0;

        std::vector<qreal> fixDurationsWithMSInSec;
        qreal fixDurationWithMSSum = 0;
        qreal fixDurationWithMSMin = 0;
        qreal fixDurationWithMSMax = 0;
        qreal fixDurationWithMSMean = 0;
        qreal fixDurationWithMSMedian = 0;

        std::vector<int> msCounts;
        int msCountSum = 0;
        qreal msCountMin = 0;
        qreal msCountMax = 0;
        qreal msCountMean = 0;
        qreal msCountMedian = 0;

        std::vector<int> msPerFix;
        qreal msPerFixMin = 0;
        qreal msPerFixMax = 0;
        qreal msPerFixMean = 0;
        qreal msPerFixMedian = 0;
        qreal msPerFixSum = 0;

        std::vector<int> msPerFixWithMS;
        qreal msPerFixWithMSMin = 0;
        qreal msPerFixWithMSMax = 0;
        qreal msPerFixWithMSMean = 0;
        qreal msPerFixWithMSMedian = 0;
        qreal msPerFixWithMSSum = 0;

        std::vector<qreal> msPerSecond;
        qreal msPerSecondMin = 0;
        qreal msPerSecondMax = 0;
        qreal msPerSecondMean = 0;
        qreal msPerSecondMedian = 0;

        std::vector<qreal> msPerSecondInFix;
        qreal msPerSecondInFixMin = 0;
        qreal msPerSecondInFixMax = 0;
        qreal msPerSecondInFixMean = 0;
        qreal msPerSecondInFixMedian = 0;

        std::vector<qreal> msAmplitudes;
        qreal msAmplitudeMin = 0;
        qreal msAmplitudeMax = 0;
        qreal msAmplitudeMean = 0;
        qreal msAmplitudeMedian = 0;

        std::vector<qreal> msIntersaccIntervals;
        qreal msIntersaccIntervalMin = 0;
        qreal msIntersaccIntervalMax = 0;
        qreal msIntersaccIntervalMean = 0;
        qreal msIntersaccIntervalMedian = 0;

        std::vector<qreal> msDurations;
        qreal msDurationMin = 0;
        qreal msDurationMax = 0;
        qreal msDurationMean = 0;
        qreal msDurationMedian = 0;

        std::vector<qreal> msPeakVelocities;
        qreal msVelocityMin = 0;
        qreal msVelocityMax = 0;
        qreal msVelocityMean = 0;
        qreal msVelocityMedian = 0;

        for (size_t i = 0; i < currentTrials.size(); ++i)
        {
            int gazeDataSamples = 0;

            const QMap<int, GazeData>& g = currentTrials[i]->gazeData();
            if (!g.empty())
            {
                QMapIterator<int, GazeData> it(g);
                while(it.hasNext())
                {
                    it.next();
                    if (!m_settings->m_limitTimeRange ||
                            (it.key() >= currentTrials[i]->timeToSampleInTrial(m_settings->m_startTimeInMSec) &&
                            it.key() <= currentTrials[i]->timeToSampleInTrial(m_settings->m_endTimeInMSec)))
                        gazeDataSamples++;
                }
            }
            rawDataSamples.push_back(gazeDataSamples);

            int duration = 0;
            int fixWithMS = 0;
            int msCountSum = 0;
            int fixSum = 0;
            for (int j = 0; j < int(currentTrials[i]->fixationsCount()); ++j)
            {
                Fixation* fixation = currentTrials[i]->getFixationAt(j);
                if (!currentTrials[i]->outsideTimeLimitIfActivated(fixation->getStartIndex(), fixation->getEndIndex()))
                {
                    duration += fixation->getDuration();

                    bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                    int msCount = int(fixation->getMicrosaccadesCount(fromInput));
                    Saccade* prevMS = nullptr;
                    for (int k = 0; k < msCount; ++k)
                    {
                        Saccade* ms = fixation->getMicrosaccadeAt(size_t(k), fromInput);

                        msAmplitudes.push_back(ms->getAmplitude());
                        msDurations.push_back(currentTrials[i]->samplesToTime(int(ms->getDuration())));

                        msPeakVelocities.push_back(ms->getPeakVelocity());

                        if (k > 0)
                        {
                            msIntersaccIntervals.push_back(currentTrials[i]->samplesToTime(int(ms->getOnsetIndex() - prevMS->getEndIndex())));
                        }
                        prevMS = ms;
                    }

                    if (msCount > 0)
                    {
                        fixDurationsWithMSInSec.push_back(currentTrials[i]->samplesToTime(fixation->getDuration()) / 1000.0);
                        msPerFixWithMS.push_back(msCount);
                        fixWithMS++;
                    }
                    fixDurationsInSec.push_back(currentTrials[i]->samplesToTime(fixation->getDuration()) / 1000.0);

                    msPerFix.push_back(msCount);
                    msCountSum += msCount;
                    fixSum++;
                }
            }

            msCounts.push_back(msCountSum);
            fixationCounts.push_back(fixSum);

            fixationWithMSCounts.push_back(fixWithMS);
            fixationWithMSRatio.push_back(qreal(fixWithMS) / currentTrials[i]->fixationsCount());
            msPerSecondInFix.push_back(duration == 0 ? 0 : msCountSum / currentTrials[i]->samplesToTime(duration) * 1000.0);

            int trialDuration;
            if(m_settings->m_limitTimeRange)
            {
                trialDuration = std::min(currentTrials[i]->timeToSampleInTrial(m_settings->m_endTimeInMSec),
                                         currentTrials[i]->lastIndex()) -
                                std::max(currentTrials[i]->timeToSampleInTrial(m_settings->m_startTimeInMSec),
                                         currentTrials[i]->firstIndex()) + 1;
            }
            else
            {
                trialDuration = currentTrials[i]->duration();
            }
            durationsInSec.push_back(currentTrials[i]->samplesToTime(trialDuration) / 1000.0);
            msPerSecond.push_back(msCountSum / currentTrials[i]->samplesToTime(trialDuration) * 1000.0);
        }

        rawDataSamplesSum = std::accumulate(rawDataSamples.begin(), rawDataSamples.end(), 0.0);
        rawDataSamplesMin = *std::min_element(rawDataSamples.begin(), rawDataSamples.end());
        rawDataSamplesMax = *std::max_element(rawDataSamples.begin(), rawDataSamples.end());
        rawDataSamplesMedian = MathHelpFunctions::median<qreal>(rawDataSamples);
        rawDataSamplesMean = rawDataSamplesSum / currentTrials.size();

        durationSum = std::accumulate(durationsInSec.begin(), durationsInSec.end(), 0.0);
        durationMin = *std::min_element(durationsInSec.begin(), durationsInSec.end());
        durationMax = *std::max_element(durationsInSec.begin(), durationsInSec.end());
        durationMedian = MathHelpFunctions::median<qreal>(durationsInSec);
        durationMean = durationSum / currentTrials.size();

        fixCountSum = std::accumulate(fixationCounts.begin(), fixationCounts.end(), 0);
        fixCountMin = *std::min_element(fixationCounts.begin(), fixationCounts.end());
        fixCountMax = *std::max_element(fixationCounts.begin(), fixationCounts.end());
        fixCountMedian = MathHelpFunctions::median<int>(fixationCounts);
        fixCountMean = qreal(fixCountSum) / currentTrials.size();

        fixWithMSCountSum = std::accumulate(fixationWithMSCounts.begin(), fixationWithMSCounts.end(), 0);
        fixWithMSCountMin = *std::min_element(fixationWithMSCounts.begin(), fixationWithMSCounts.end());
        fixWithMSCountMax = *std::max_element(fixationWithMSCounts.begin(), fixationWithMSCounts.end());
        fixWithMSCountMedian = MathHelpFunctions::median<int>(fixationWithMSCounts);
        fixWithMSCountMean = qreal(fixWithMSCountSum) / currentTrials.size();

        qreal fixWithMSRatioSum = std::accumulate(fixationWithMSRatio.begin(), fixationWithMSRatio.end(), 0.0);
        fixWithMSRatioMin = *std::min_element(fixationWithMSRatio.begin(),fixationWithMSRatio.end()) * 100;
        fixWithMSRatioMax = *std::max_element(fixationWithMSRatio.begin(),fixationWithMSRatio.end()) * 100;
        fixWithMSRatioMedian = MathHelpFunctions::median<qreal>(fixationWithMSRatio) * 100;
        fixWithMSRatioMean = qreal(fixWithMSRatioSum) / fixationWithMSRatio.size() * 100;

        if (!fixDurationsInSec.empty())
        {
            fixDurationSum = std::accumulate(fixDurationsInSec.begin(), fixDurationsInSec.end(), 0.0);
            fixDurationMin = *std::min_element(fixDurationsInSec.begin(), fixDurationsInSec.end());
            fixDurationMax = *std::max_element(fixDurationsInSec.begin(), fixDurationsInSec.end());
            fixDurationMedian = MathHelpFunctions::median<qreal>(fixDurationsInSec);
            fixDurationMean = fixDurationSum / fixDurationsInSec.size();
        }

        if (!fixDurationsWithMSInSec.empty())
        {
            fixDurationWithMSSum = std::accumulate(fixDurationsWithMSInSec.begin(), fixDurationsWithMSInSec.end(), 0.0);
            fixDurationWithMSMin = *std::min_element(fixDurationsWithMSInSec.begin(), fixDurationsWithMSInSec.end());
            fixDurationWithMSMax = *std::max_element(fixDurationsWithMSInSec.begin(), fixDurationsWithMSInSec.end());
            fixDurationWithMSMedian = MathHelpFunctions::median<qreal>(fixDurationsWithMSInSec);
            fixDurationWithMSMean = fixDurationWithMSSum / fixDurationsWithMSInSec.size();
        }

        msCountSum = std::accumulate(msCounts.begin(), msCounts.end(), 0);
        msCountMin = *std::min_element(msCounts.begin(), msCounts.end());
        msCountMax = *std::max_element(msCounts.begin(), msCounts.end());
        msCountMedian = MathHelpFunctions::median<int>(msCounts);
        msCountMean = double(msCountSum) / currentTrials.size();

        if (!msPerFix.empty())
        {
            msPerFixSum = std::accumulate(msPerFix.begin(), msPerFix.end(), 0.0);
            msPerFixMin = *std::min_element(msPerFix.begin(), msPerFix.end());
            msPerFixMax = *std::max_element(msPerFix.begin(), msPerFix.end());
            msPerFixMedian = MathHelpFunctions::median<int>(msPerFix);
            msPerFixMean = msPerFixSum / msPerFix.size();
        }

        if (!msPerFixWithMS.empty())
        {
            msPerFixWithMSSum = std::accumulate(msPerFixWithMS.begin(), msPerFixWithMS.end(), 0.0);
            msPerFixWithMSMin = *std::min_element(msPerFixWithMS.begin(), msPerFixWithMS.end());
            msPerFixWithMSMax = *std::max_element(msPerFixWithMS.begin(), msPerFixWithMS.end());
            msPerFixWithMSMedian = MathHelpFunctions::median<int>(msPerFixWithMS);
            msPerFixWithMSMean = msPerFixWithMSSum / msPerFixWithMS.size();
        }

        if (!msPerSecond.empty())
        {
            qreal msPerSecondSum = std::accumulate(msPerSecond.begin(), msPerSecond.end(), 0.0);
            msPerSecondMin = *std::min_element(msPerSecond.begin(), msPerSecond.end());
            msPerSecondMax = *std::max_element(msPerSecond.begin(), msPerSecond.end());
            msPerSecondMedian = MathHelpFunctions::median<qreal>(msPerSecond);
            msPerSecondMean = msPerSecondSum / currentTrials.size();
        }

        if (!msPerSecondInFix.empty())
        {
            qreal msPerSecondInFixSum = std::accumulate(msPerSecondInFix.begin(), msPerSecondInFix.end(), 0.0);
            msPerSecondInFixMin = *std::min_element(msPerSecondInFix.begin(), msPerSecondInFix.end());
            msPerSecondInFixMax = *std::max_element(msPerSecondInFix.begin(), msPerSecondInFix.end());
            msPerSecondInFixMedian = MathHelpFunctions::median<qreal>(msPerSecondInFix);
            msPerSecondInFixMean = msPerSecondInFixSum / msPerSecondInFix.size();
        }

        if (!msAmplitudes.empty())
        {
            qreal msAmplitudeSum = std::accumulate(msAmplitudes.begin(), msAmplitudes.end(), 0.0);
            msAmplitudeMin = *std::min_element(msAmplitudes.begin(), msAmplitudes.end());
            msAmplitudeMax = *std::max_element(msAmplitudes.begin(), msAmplitudes.end());
            msAmplitudeMedian = MathHelpFunctions::median<qreal>(msAmplitudes);
            msAmplitudeMean = msAmplitudeSum / msAmplitudes.size();
        }

        if (!msDurations.empty())
        {
            qreal msDurationSum = std::accumulate(msDurations.begin(), msDurations.end(), 0.0);
            msDurationMin = *std::min_element(msDurations.begin(), msDurations.end());
            msDurationMax = *std::max_element(msDurations.begin(), msDurations.end());
            msDurationMedian = MathHelpFunctions::median<qreal>(msDurations);
            msDurationMean = msDurationSum / msDurations.size();
        }

        if (!msIntersaccIntervals.empty())
        {
            qreal msIntersaccInterrvalSum = std::accumulate(msIntersaccIntervals.begin(), msIntersaccIntervals.end(), 0.0);
            msIntersaccIntervalMin = *std::min_element(msIntersaccIntervals.begin(), msIntersaccIntervals.end());
            msIntersaccIntervalMax = *std::max_element(msIntersaccIntervals.begin(), msIntersaccIntervals.end());
            msIntersaccIntervalMedian = MathHelpFunctions::median<qreal>(msIntersaccIntervals);
            msIntersaccIntervalMean = msIntersaccInterrvalSum / msIntersaccIntervals.size();
        }

        if (!msPeakVelocities.empty())
        {
            qreal msVelocitySum = std::accumulate(msPeakVelocities.begin(), msPeakVelocities.end(), 0.0);
            msVelocityMin = *std::min_element(msPeakVelocities.begin(), msPeakVelocities.end());
            msVelocityMax = *std::max_element(msPeakVelocities.begin(), msPeakVelocities.end());
            msVelocityMedian = MathHelpFunctions::median<qreal>(msPeakVelocities);
            msVelocityMean = msVelocitySum / msPeakVelocities.size();
        }

        m_ui->m_inputSamplesNumberLabelMin->setText(QString::number(rawDataSamplesMin, 'g', 10));
        m_ui->m_inputSamplesNumberLabelMax->setText(QString::number(rawDataSamplesMax, 'g', 10));
        m_ui->m_inputSamplesNumberLabelMean->setText(QString::number(rawDataSamplesMean, 'f', 2));
        m_ui->m_inputSamplesNumberLabelMedian->setText(QString::number(rawDataSamplesMedian, 'g', 10));
        m_ui->m_inputSamplesNumberLabelSum->setText(QString::number(rawDataSamplesSum, 'g', 10));

        m_ui->m_durationValueLabelMin->setText(QString::number(durationMin, 'f', 2));
        m_ui->m_durationValueLabelMax->setText(QString::number(durationMax, 'f', 2));
        m_ui->m_durationValueLabelMean->setText(QString::number(durationMean, 'f', 2));
        m_ui->m_durationValueLabelMedian->setText(QString::number(durationMedian, 'f', 2));
        m_ui->m_durationValueLabelSum->setText(QString::number(durationSum, 'f', 2));

        m_ui->m_fixationCountLabelMin->setText(QString::number(fixCountMin, 'g', 10));
        m_ui->m_fixationCountLabelMax->setText(QString::number(fixCountMax, 'g', 10));
        m_ui->m_fixationCountLabelMean->setText(QString::number(fixCountMean, 'f', 2));
        m_ui->m_fixationCountLabelMedian->setText(QString::number(fixCountMedian, 'f', 2));
        m_ui->m_fixationCountLabelSum->setText(QString::number(fixCountSum, 'g', 10));

        m_ui->m_fixationsWithMicrosaccadesCountLabelMin->setText(QString::number(fixWithMSCountMin, 'g', 10));
        m_ui->m_fixationsWithMicrosaccadesCountLabelMax->setText(QString::number(fixWithMSCountMax, 'g', 10));
        m_ui->m_fixationsWithMicrosaccadesCountLabelMean->setText(QString::number(fixWithMSCountMean, 'f', 2));
        m_ui->m_fixationsWithMicrosaccadesCountLabelMedian->setText(QString::number(fixWithMSCountMedian, 'f', 2));
        m_ui->m_fixationsWithMicrosaccadesCountLabelSum->setText(QString::number(fixWithMSCountSum, 'g', 10));

        m_ui->m_fixationsWithMicrosaccadesRatioLabelMin->setText(QString::number(fixWithMSRatioMin, 'f', 2));
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMax->setText(QString::number(fixWithMSRatioMax, 'f', 2));
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMean->setText(QString::number(fixWithMSRatioMean, 'f', 2));
        m_ui->m_fixationsWithMicrosaccadesRatioLabelMedian->setText(QString::number(fixWithMSRatioMedian, 'f', 2));

        m_ui->m_fixationDurationLabelMin->setText(QString::number(fixDurationMin, 'f', 2));
        m_ui->m_fixationDurationLabelMax->setText(QString::number(fixDurationMax, 'f', 2));
        m_ui->m_fixationDurationLabelMean->setText(QString::number(fixDurationMean, 'f', 2));
        m_ui->m_fixationDurationLabelMedian->setText(QString::number(fixDurationMedian, 'f', 2));
        m_ui->m_fixationDurationLabelSum->setText(QString::number(fixDurationSum, 'f', 2));

        m_ui->m_fixationDurationWithMSLabelMin->setText(QString::number(fixDurationWithMSMin, 'f', 2));
        m_ui->m_fixationDurationWithMSLabelMax->setText(QString::number(fixDurationWithMSMax, 'f', 2));
        m_ui->m_fixationDurationWithMSLabelMean->setText(QString::number(fixDurationWithMSMean, 'f', 2));
        m_ui->m_fixationDurationWithMSLabelMedian->setText(QString::number(fixDurationWithMSMedian, 'f', 2));
        m_ui->m_fixationDurationWithMSLabelSum->setText(QString::number(fixDurationWithMSSum, 'f', 2));

        m_ui->m_microsaccadesCountLabelMin->setText(QString::number(msCountMin, 'g', 10));
        m_ui->m_microsaccadesCountLabelMax->setText(QString::number(msCountMax, 'g', 10));
        m_ui->m_microsaccadesCountLabelMean->setText(QString::number(msCountMean, 'f', 2));
        m_ui->m_microsaccadesCountLabelMedian->setText(QString::number(msCountMedian, 'f', 2));
        m_ui->m_microsaccadesCountLabelSum->setText(QString::number(msCountSum, 'g', 10));

        m_ui->m_microsaccadesPerFixationLabelMin->setText(QString::number(msPerFixMin, 'g', 10));
        m_ui->m_microsaccadesPerFixationLabelMax->setText(QString::number(msPerFixMax, 'g', 10));
        m_ui->m_microsaccadesPerFixationLabelMean->setText(QString::number(msPerFixMean, 'f', 2));
        m_ui->m_microsaccadesPerFixationLabelMedian->setText(QString::number(msPerFixMedian, 'f', 2));
        m_ui->m_microsaccadesPerFixationLabelSum->setText(QString::number(msPerFixSum, 'g', 10));

        m_ui->m_microsaccadesPerFixationWithMSLabelMin->setText(QString::number(msPerFixWithMSMin, 'g', 10));
        m_ui->m_microsaccadesPerFixationWithMSLabelMax->setText(QString::number(msPerFixWithMSMax, 'g', 10));
        m_ui->m_microsaccadesPerFixationWithMSLabelMean->setText(QString::number(msPerFixWithMSMean, 'f', 2));
        m_ui->m_microsaccadesPerFixationWithMSLabelMedian->setText(QString::number(msPerFixWithMSMedian, 'f', 2));
        m_ui->m_microsaccadesPerFixationWithMSLabelSum->setText(QString::number(msPerFixWithMSSum, 'g', 10));

        m_ui->m_microsaccadesPerSecondLabelMin->setText(QString::number(msPerSecondMin, 'f', 2));
        m_ui->m_microsaccadesPerSecondLabelMax->setText(QString::number(msPerSecondMax, 'f', 2));
        m_ui->m_microsaccadesPerSecondLabelMean->setText(QString::number(msPerSecondMean, 'f', 2));
        m_ui->m_microsaccadesPerSecondLabelMedian->setText(QString::number(msPerSecondMedian, 'f', 2));

        m_ui->m_microsaccadesPerSecondInFixationsLabelMin->setText(QString::number(msPerSecondInFixMin, 'f', 2));
        m_ui->m_microsaccadesPerSecondInFixationsLabelMax->setText(QString::number(msPerSecondInFixMax, 'f', 2));
        m_ui->m_microsaccadesPerSecondInFixationsLabelMean->setText(QString::number(msPerSecondInFixMean, 'f', 2));
        m_ui->m_microsaccadesPerSecondInFixationsLabelMedian->setText(QString::number(msPerSecondInFixMedian, 'f', 2));

        m_ui->m_microsaccadeAmplitudeLabelMin->setText(QString::number(msAmplitudeMin, 'f', 2));
        m_ui->m_microsaccadeAmplitudeLabelMax->setText(QString::number(msAmplitudeMax, 'f', 2));
        m_ui->m_microsaccadeAmplitudeLabelMean->setText(QString::number(msAmplitudeMean, 'f', 2));
        m_ui->m_microsaccadeAmplitudeLabelMedian->setText(QString::number(msAmplitudeMedian, 'f', 2));

        m_ui->m_microsaccadesIntersaccIntervalLabelMin->setText(QString::number(msIntersaccIntervalMin, 'f', 2));
        m_ui->m_microsaccadesIntersaccIntervalLabelMax->setText(QString::number(msIntersaccIntervalMax, 'f', 2));
        m_ui->m_microsaccadesIntersaccIntervalLabelMean->setText(QString::number(msIntersaccIntervalMean, 'f', 2));
        m_ui->m_microsaccadesIntersaccIntervalLabelMedian->setText(QString::number(msIntersaccIntervalMedian, 'f', 2));

        m_ui->m_microsaccadeDurationLabelMin->setText(QString::number(msDurationMin, 'f', 2));
        m_ui->m_microsaccadeDurationLabelMax->setText(QString::number(msDurationMax, 'f', 2));
        m_ui->m_microsaccadeDurationLabelMean->setText(QString::number(msDurationMean, 'f', 2));
        m_ui->m_microsaccadeDurationLabelMedian->setText(QString::number(msDurationMedian, 'f', 2));

        m_ui->m_microsaccadeVelocityLabelMin->setText(QString::number(msVelocityMin, 'f', 2));
        m_ui->m_microsaccadeVelocityLabelMax->setText(QString::number(msVelocityMax, 'f', 2));
        m_ui->m_microsaccadeVelocityLabelMean->setText(QString::number(msVelocityMean, 'f', 2));
        m_ui->m_microsaccadeVelocityLabelMedian->setText(QString::number(msVelocityMedian, 'f', 2));
    }
}


void MainWindow::updateMainAndTimelineView()
{
    updateMainView();
    updateTimelineView();
}


void MainWindow::updateMainView()
{
    m_mainVisualization->updateVisibility();
    m_mainView->updateAll();
}


void MainWindow::updateTimelineView()
{
    m_timelineVisualization->updateVisibility();
    m_timelineView->updateAll();
    m_timelineVisualization->updateSize();
}


void MainWindow::showDataPlotForTrials()
{
    if (m_ui == nullptr)
    {
        return;
    }

    std::vector<Trial*> trials = m_settings->m_currentTrials;

    PlotFixations fixationSelection = PF_All;
    switch (m_ui->m_dataPlotFixationsComboBox->currentIndex())
    {
        case 0:
            fixationSelection = PF_All;
            break;
        case 1:
            fixationSelection = PF_Current;
            break;
        case 2:
            fixationSelection = PF_Visible;
            break;
    }

    PlotData plotData = PD_Microsaccades;
    switch (m_ui->m_dataPlotDataComboBox->currentIndex())
    {
        case 0:
            plotData = PD_Microsaccades;
            break;
        case 1:
            plotData = PD_Saccades;
            break;
        case 2:
            plotData = PD_GazeRawData;
            break;
    }

    PlotDirection plotDirection = PDir_ToNextFixation;
    switch (m_ui->m_dataPlotDirectionComboBox->currentIndex())
    {
        case 0:
            plotDirection = PDir_FromView;
            break;
        case 1:
            plotDirection = PDir_ToNextFixation;
            break;
        case 2:
            plotDirection = PDir_ToPreviousFixation;
            break;
    }

    PlotAggregation plotAggregation = PA_NoAggregation;
    switch (m_ui->m_aggregatePlotDataComboBox->currentIndex())
    {
        case 0:
            plotAggregation = PA_NoAggregation;
            break;
        case 1:
            plotAggregation = PA_All;
            break;
        case 2:
            plotAggregation = PA_ForTestConditions;
            break;
        case 3:
            plotAggregation = PA_ForTrials;
            break;
        case 4:
            plotAggregation = PA_ForParticipants;
            break;
    }

    PlotType plotType = PT_RegularPolarPlot;
    switch (m_ui->m_dataPlotTypeComboBox->currentIndex())
    {
        case 0:
            switch (m_ui->m_dataDirectionPlotVisTypeComboBox->currentIndex())
            {
                case 0:
                    plotType = PT_RegularPolarPlot;
                    break;

                //case 1:
                //    plotType = PT_PolarPlotWithHole;
                //    break;
                //
                //case 2:
                //    plotType = PT_RosePlot;
                //    break;
                //
                //case 3:
                case 1:
                    plotType = PT_RosePlotWithHole;
                    break;

                //case 4:
                //    plotType = PT_BoxRosePlot;
                //    break;
                //
                //case 5:
                //    plotType = PT_BoxRosePlotWithHole;
                //    break;
                //
                //case 6:
                //    plotType = PT_HistogramBarPlot;
                //    break;
            }
            m_ui->m_dataScatterplotVisTypeComboBox->hide();
            m_ui->m_dataPlotGraphLabel2->hide();
            m_ui->m_dataDirectionPlotVisTypeComboBox->show();
            m_ui->m_dataPlotGraphLabel->show();
            m_ui->m_dataPlotAggregationBinsValueLabel->show();
            m_ui->m_dataPlotBinsSpinBox->show();
            m_ui->m_testConditionColorsCheckbox->show();

            !m_settings->m_testConditions.empty() ? m_ui->m_dataPlotLegend->show() : m_ui->m_dataPlotLegend->hide();

            break;

        case 1:
            switch (m_ui->m_dataScatterplotVisTypeComboBox->currentIndex())
            {
                case 0:
                    plotType = PT_RadialScatterPlot;
                    break;

                case 1:
                    plotType = PT_RadialConnectedScatterPlot;
                    break;
            }

            m_ui->m_dataPlotLegend->show();
            m_ui->m_dataPlotAggregationBinsValueLabel->hide();
            m_ui->m_dataPlotBinsSpinBox->hide();
            m_ui->m_testConditionColorsCheckbox->hide();
            m_ui->m_dataDirectionPlotVisTypeComboBox->hide();
            m_ui->m_dataPlotGraphLabel->hide();
            m_ui->m_dataScatterplotVisTypeComboBox->show();
            m_ui->m_dataPlotGraphLabel2->show();

            break;

        case 2:
            plotType = PT_RadialRawDataPlot;

            m_ui->m_dataPlotLegend->show();
            m_ui->m_dataPlotAggregationBinsValueLabel->hide();
            m_ui->m_dataPlotBinsSpinBox->hide();
            m_ui->m_testConditionColorsCheckbox->hide();
            m_ui->m_dataScatterplotVisTypeComboBox->hide();
            m_ui->m_dataPlotGraphLabel2->hide();
            m_ui->m_dataDirectionPlotVisTypeComboBox->hide();
            m_ui->m_dataPlotGraphLabel->hide();
            break;
    }

    if (plotType == PT_RadialRawDataPlot)
    {
        m_ui->m_dataPlotRadiusSpinBox->hide();
        m_ui->dataPlotRadiusLabel->hide();
        m_ui->m_dataPlotRadiusSpinBox_2->show();
        m_ui->dataPlotRadiusLabel_2->show();
    }
    else
    {
        m_ui->m_dataPlotRadiusSpinBox_2->hide();
        m_ui->dataPlotRadiusLabel_2->hide();
        m_ui->m_dataPlotRadiusSpinBox->show();
        m_ui->dataPlotRadiusLabel->show();
    }

    int aggregations = int(360.0 / m_ui->m_dataPlotBinsSpinBox->value());

    qreal plotRadiusCout = m_ui->m_dataPlotRadiusSpinBox->value();
    qreal plotRadiusDegree = m_ui->m_dataPlotRadiusSpinBox_2->value();

    qApp->setOverrideCursor(Qt::WaitCursor);

    m_settings->m_dataPlotData = plotData;
    m_settings->m_dataPlotDirection = plotDirection;
    m_settings->m_dataPlotType = plotType;
    m_settings->m_dataPlotBins = aggregations;
    m_settings->m_dataPlotRadiusCount = plotRadiusCout;
    m_settings->m_dataPlotRadiusDegree = plotRadiusDegree;
    m_settings->m_dataPlotFixations = fixationSelection;
    m_settings->m_overplot = plotAggregation;
    m_settings->m_testConditionColors = m_ui->m_testConditionColorsCheckbox->isChecked();

    m_ui->updateDataPlot(trials, getFixationsForTrials(trials, fixationSelection));

    qApp->restoreOverrideCursor();
}


void MainWindow::showHistogramForTrials()
{
    if (m_ui == nullptr)
    {
        return;
    }

    std::vector<Trial*> trials = m_settings->m_currentTrials;

    PlotData plotData = PD_Microsaccades;
    switch (m_ui->m_dataComboBox->currentIndex())
    {
        case 0:
        {
            plotData = PD_Microsaccades;
            auto  model = dynamic_cast<QStandardItemModel*>(m_ui->m_histogramYAxisCombooBox->model());
            QStandardItem* item = model->item(3, 0);
            item->setEnabled(true);
            break;
        }
        case 1:
        {
            plotData = PD_Saccades;

            // disable an entry if saccades is selected
            if (m_ui->m_histogramYAxisCombooBox->currentIndex() == 3)
            {
                m_ui->m_histogramYAxisCombooBox->setCurrentIndex(0);
                return;
            }

            auto  model = dynamic_cast<QStandardItemModel*>(m_ui->m_histogramYAxisCombooBox->model());
            QStandardItem* item = model->item(3, 0);
            item->setEnabled(false);
            break;
        }
    }

    PlotValues plotValues = PeakVelocity;
    switch (m_ui->m_histogramYAxisCombooBox->currentIndex())
    {
        case 0:
            plotValues = PeakVelocity;
            break;
        case 1:
            plotValues = Duration;
            break;
        case 2:
            plotValues = Amplitude;
            break;
        case 3:
            plotValues = MSStart;
            break;
    }

    m_settings->m_histogramAndScatterplotData = plotData;
    m_settings->m_histogramYAxis = plotValues;
    m_settings->m_histogramStartTimeBinSize = m_ui->m_histogramStartTimeBinSizeSpinBox->value();
    m_settings->m_histogramPeakVelocityBinSize = m_ui->m_histogramPeakVelocityBinSizeSpinBox->value();
    m_settings->m_histogramAmplitudeBinSize = m_ui->m_histogramAmplitudeBinSizeSpinBox->value();
    m_settings->m_histogramDurationBinSize = m_ui->m_histogramDurationBinSizeSpinBox->value();
    m_settings->m_histogramMaxX = m_ui->m_histogramMaxXSpinBox->value();
    m_settings->m_histogramMaxY = m_ui->m_histogramMaxYSpinBox->value();

    m_ui->m_histogramPlot->updatePlot(trials, getFixationsForTrials(trials, PlotFixations::PF_All),
                                      getSaccadesForTrial(trials));

    !m_settings->m_testConditions.empty() ? m_ui->m_diagramsLegend->show() : m_ui->m_diagramsLegend->hide();
}


void MainWindow::showScatterplotForTrials()
{
    if (m_ui == nullptr)
    {
        return;
    }

    std::vector<Trial*> trials = m_settings->m_currentTrials;

    PlotData plotData = PD_Microsaccades;
    switch (m_ui->m_dataComboBox->currentIndex())
    {
        case 0:
            plotData = PD_Microsaccades;
            break;
        case 1:
            plotData = PD_Saccades;
            break;
    }

    PlotValues plotXValues = PeakVelocity;
    switch (m_ui->m_scatterplotXAxisComboBox->currentIndex())
    {
        case 0:
            plotXValues = PeakVelocity;
            break;
        case 1:
            plotXValues = Duration;
            break;
        case 2:
            plotXValues = Amplitude;
            break;
    }

    PlotValues plotYValues = PeakVelocity;
    switch (m_ui->m_scatterplotYAxisComboBox->currentIndex())
    {
        case 0:
            plotYValues = PeakVelocity;
            break;
        case 1:
            plotYValues = Duration;
            break;
        case 2:
            plotYValues = Amplitude;
            break;
    }

    m_settings->m_histogramAndScatterplotData = plotData;
    m_settings->m_scatterplotXAxis = plotXValues;
    m_settings->m_scatterplotYAxis = plotYValues;
    m_settings->m_scatterplotMaxX = m_ui->m_scatterplotMaxXSpinBox->value();
    m_settings->m_scatterplotMaxY = m_ui->m_scatterplotMaxYSpinBox->value();
    m_settings->m_logarithmicScale = m_ui->m_logarithmicScale->isChecked();

    m_ui->m_scatterplot->updatePlot(trials, getFixationsForTrials(trials, PlotFixations::PF_All),
                                    getSaccadesForTrial(trials));

    !m_settings->m_testConditions.empty() ? m_ui->m_diagramsLegend->show() : m_ui->m_diagramsLegend->hide();
}


void MainWindow::updateFixationInfo(Fixation* fixation)
{
    if (m_settings->m_currentTrials.size() != 1)
    {
        m_ui->m_microsaccadesTable->clear();
        m_ui->m_selectionGroupBox->hide();
        return;
    }

    m_ui->m_microsaccadesTable->clear();
    m_ui->m_microsaccadesTable->scrollToTop();
    m_ui->m_microsaccadesTable->setColumnCount(9);

    QStringList tableHeader;
    tableHeader << tr("Onset [id]") << tr("End [id]") << tr("Start in Fix [ms]") << tr("Dur [ms]") << tr("Peak Vel [°/s]") << tr("V Comp [°]")
                << tr("H Comp [°]") << tr("V Ampl [°]") << tr("H Ampl [°]");
    m_ui->m_microsaccadesTable->setHorizontalHeaderLabels(tableHeader);

    Trial* trial = m_settings->m_currentTrials[0];
    if (fixation != nullptr)
    {
        m_ui->m_selectionGroupBox->show();
        m_ui->m_selectedFixationDurationLabel->
                setText(QString::number(trial->samplesToTime(fixation->getDuration()),
                        'g', 10) + " ms");
        m_ui->m_selectedFixationMicrosaccadesCountLabel->
                setText(QString::number(fixation->getMicrosaccadesCount(trial->msFromInputFile()),
                        'g', 10));
        m_ui->m_selectedFixationStartIndexLabel->
                setText(QString::number(fixation->getStartIndex(), 'g', 10));
        m_ui->m_selectedFixationStartTimeLabel->
                setText(QString::number((trial->samplesToTime(fixation->getStartIndex() -
                                         trial->gazeData().firstKey())),
                'g', 10) + " ms");
        m_ui->m_selectedFixationXLabel->setText(QString::number(fixation->getXPosition()));
        m_ui->m_selectedFixationYLabel->setText(QString::number(fixation->getYPosition()));

        // update microsaccades table
        m_ui->m_microsaccadesTable->setRowCount(int(fixation->getMicrosaccadesCount(trial->msFromInputFile())));

        int index = 0;
        QStringList indices;
        for (size_t j = 0; j < fixation->getMicrosaccadesCount(trial->msFromInputFile()); ++j)
        {
            indices << QString::number(index);
            addToMicrosaccadesTable(trial, fixation, fixation->getMicrosaccadeAt(j, trial->msFromInputFile()), index++);
        }
        m_ui->m_microsaccadesTable->setVerticalHeaderLabels(indices);
    }
    else
    {
        m_ui->m_selectionGroupBox->hide();

        m_ui->m_microsaccadesTable->setRowCount(int(trial->microsaccadesCount()));

        int index = 0;
        QStringList indices;
        for (int i = 0; i < trial->fixationsCount(); ++i)
        {
            for (size_t j = 0; j < trial->getFixationAt(i)->
                 getMicrosaccadesCount(trial->msFromInputFile()); ++j)
            {
                indices << QString::number(index);
                m_ui->m_microsaccadesTable->model()->setHeaderData(j, Qt::Vertical, int(j));
                addToMicrosaccadesTable(trial, trial->getFixationAt(i), trial->getFixationAt(i)->
                        getMicrosaccadeAt(j, trial->msFromInputFile()), index++);
            }
        }
        m_ui->m_microsaccadesTable->setVerticalHeaderLabels(indices);
    }

    m_ui->m_microsaccadesTable->resizeColumnsToContents();
}


Fixation* MainWindow::selectedFixation()
{
    if (m_settings->m_currentFixationIndex != -1 && !m_settings->m_currentTrials.empty())
    {
        Fixation* f =  m_settings->m_currentTrials[0]->getFixationAt(m_settings->m_currentFixationIndex);
        return f;
    }
    return nullptr;
}


std::vector<std::vector<Fixation*> > MainWindow::getFixationsForTrials(
        std::vector<Trial *>& trials, PlotFixations plotElements)
{
    std::vector<std::vector<Fixation*> > fixationsForTrials;

    for (size_t i = 0; i < trials.size(); ++i)
    {
        std::vector<Fixation*> fixations;

        if (trials.size() > 1 || plotElements == PF_All)
        {
            for (int j = 0; j < trials[i]->fixationsCount(); ++j)
            {
                Fixation* fixation = trials[i]->getFixationAt(j);
                if (!trials[i]->outsideTimeLimitIfActivated(fixation->getStartIndex(),
                                                            fixation->getEndIndex()))
                {
                    fixations.push_back(fixation);
                }
            }
        }
        else if (plotElements == PF_Current && m_settings->m_currentFixationIndex != -1 && i == 0)
        {
            Fixation* fixation = trials[i]->getFixationAt(m_settings->m_currentFixationIndex);
            if (!trials[i]->outsideTimeLimitIfActivated(fixation->getStartIndex(),
                                                        fixation->getEndIndex()))
            {
                fixations.push_back(fixation);
            }
        }
        else if (plotElements == PF_Visible && i == 0)
        {
            for (int j = 0; j < int(trials[i]->fixationsCount()); ++j)
            {
                if ((!m_settings->m_showNeighboringFixations ||
                     (m_settings->m_currentFixationIndex > -1 &&
                      j <= m_settings->m_currentFixationIndex +
                      m_settings->m_followingFixationsCount &&
                      j >= m_settings->m_currentFixationIndex -
                      m_settings->m_previousFixationsCount)))
                {
                    Fixation* fixation = trials[i]->getFixationAt(j);
                    if (!trials[i]->outsideTimeLimitIfActivated(fixation->getStartIndex(),
                                                                fixation->getEndIndex()))
                    {
                        fixations.push_back(fixation);
                    }
                }
            }
        }

        fixationsForTrials.push_back(fixations);
    }

    return fixationsForTrials;
}


std::vector<std::vector<Saccade*> > MainWindow::getSaccadesForTrial(
        std::vector<Trial *>& trials)
{
    std::vector<std::vector<Saccade*> > saccadesForTrials;

    for (size_t i = 0; i < trials.size(); ++i)
    {
        std::vector<Saccade*> saccades;
        for (size_t j = 0; j < trials[i]->saccadesCount(); ++j)
        {
            Saccade* saccade = trials[i]->getSaccadeAt(j);
            if (!trials[i]->outsideTimeLimitIfActivated(int(saccade->getOnsetIndex()),
                                                        int(saccade->getEndIndex())))
            {
                saccades.push_back(saccade);
            }
        }
        saccadesForTrials.push_back(saccades);
    }
    return saccadesForTrials;
}


std::vector<Trial*> MainWindow::getTrials(const std::set<int> participantIds,
                                          const std::set<QString> trialNames,
                                          const std::set<QString> testConditions)
{
    std::vector<Trial*> trials;
    for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
    {
        if (participantIds.count(int(i)) > 0)
        {
            for (size_t j = 0; j < m_eyeTrackingData->participantAt(int(i))->numberOfTrials(); ++j)
            {
                if (trialNames.count(m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j))->name()) > 0 &&
                    (m_ui->m_testConditionList->count() == 0 ||
                     (testConditions.empty() &&
                      m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j))->getTestConditionType().isEmpty()) ||
                      testConditions.count(m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j))->getTestConditionType()) > 0))
                {
                    trials.push_back(m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j)));
                }
            }
        }
    }

    return trials;
}


void MainWindow::updateUI(bool updateTrials)
{
    if (m_eyeTrackingData->numberOfParticipants() == 0)
    {
        return;
    }

    if (updateTrials && m_settings->m_displayMode == DisplayMode::Individual)
    {
        m_mainVisualization->updateTrial();
        m_timelineVisualization->updateTrial();
    }

    updateGeneralInfo(m_settings->m_currentTrials);
    updateStatisticsInfo(m_settings->m_currentTrials);

    updateMicrosaccadeStatBarsInUI();

    updateMainAndTimelineView();

    showDataPlotForTrials();
    showHistogramForTrials();
    showScatterplotForTrials();

    updateFixationTable();
}


void MainWindow::addToMicrosaccadesTable(Trial* trial, Fixation* f, Saccade* m, int index)
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    auto item = new QTableWidgetItem;
    item->setData(Qt::EditRole, QString::number(m->getOnsetIndex(), 'g', 10));
    m_ui->m_microsaccadesTable->setItem(index, 0, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, QString::number(m->getEndIndex(), 'g', 10));
    m_ui->m_microsaccadesTable->setItem(index, 1, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, trial->samplesToTime(m->getOnsetIndex() - f->getStartIndex()));
    m_ui->m_microsaccadesTable->setItem(index, 2, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, trial->samplesToTime(m->getDuration()));
    m_ui->m_microsaccadesTable->setItem(index, 3, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, m->getPeakVelocity());
    m_ui->m_microsaccadesTable->setItem(index, 4, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, m->getHorizontalComponent());
    m_ui->m_microsaccadesTable->setItem(index, 5, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, m->getVerticalComponent());
    m_ui->m_microsaccadesTable->setItem(index, 6, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, m->getHorizontalAmplitude());
    m_ui->m_microsaccadesTable->setItem(index, 7, item);
    item = new QTableWidgetItem;
    item->setData(Qt::EditRole, m->getVerticalAmplitude());
    m_ui->m_microsaccadesTable->setItem(index, 8, item);
}


void MainWindow::updateMicrosaccadesInFixationTable()
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }
    for (int i = 0; i < m_ui->m_fixationsTable->rowCount(); ++i)
    {
        m_ui->m_fixationsTable->item(i, 4)->
                setText(QString::number(m_settings->m_currentTrials[0]->getFixationAt(i)->
                                        getMicrosaccadesCount(m_settings->m_currentTrials[0]->
                                                              msFromInputFile())));
    }
}


void MainWindow::updateFixationTable()
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    // Extract important information from raw data
    int fixationsCount = int(m_settings->m_currentTrials[0]->fixationsCount());

    m_ui->m_fixationsTable->clear();
    m_ui->m_fixationsTable->scrollToTop();
    m_ui->m_fixationsTable->setRowCount(fixationsCount);
    m_ui->m_fixationsTable->setColumnCount(7);

    QStringList tableHeader;
    tableHeader << tr("Start [Id]") << tr("Start [ms]") << tr("Dur [Id]") << tr("Dur [ms]") << tr("X [px]") << tr("Y [px]") << tr("#Msacc");
    m_ui->m_fixationsTable->setHorizontalHeaderLabels(tableHeader);

    Trial* trial = m_settings->m_currentTrials[0];

    QStringList indices;
    for (int i = 0; i < fixationsCount; ++i)
    {
        indices.append(QString::number(i));
        m_ui->m_fixationsTable->setItem(i, 0,
            new QTableWidgetItem(QString::number(trial->getFixationAt(i)->
                                 getStartIndex(), 'g', 10)));
        m_ui->m_fixationsTable->setItem(i, 1,
            new QTableWidgetItem(QString::number(trial->samplesToTime(trial->getFixationAt(i)->
                                getStartIndex() - trial->gazeData().firstKey()), 'g', 10)));
        m_ui->m_fixationsTable->setItem(i, 2,
            new QTableWidgetItem(QString::number(trial->getFixationAt(i)->
                                 getDuration(), 'g', 10)));
        m_ui->m_fixationsTable->setItem(i, 3,
            new QTableWidgetItem(QString::number(trial->samplesToTime(trial->getFixationAt(i)->
                                 getDuration()), 'g', 10)));
        m_ui->m_fixationsTable->setItem(i, 4,
            new QTableWidgetItem(QString::number(trial->getFixationAt(i)->
                                 getXPosition())));
        m_ui->m_fixationsTable->setItem(i, 5,
            new QTableWidgetItem(QString::number(trial->getFixationAt(i)->
                                 getYPosition())));
        m_ui->m_fixationsTable->setItem(i, 6,
            new QTableWidgetItem(QString::number(trial->getFixationAt(i)->
                                 getMicrosaccadesCount(trial->msFromInputFile()))));
    }

    m_ui->m_fixationsTable->setVerticalHeaderLabels(indices);

    m_ui->m_fixationsTable->resizeColumnsToContents();

    if (m_settings->m_currentFixationIndex != -1)
    {
        m_ui->m_fixationsTable->selectRow(m_settings->m_currentFixationIndex);
    }

    updateFixationInfo(selectedFixation());
}


void MainWindow::exportAllViews()
{
    // get directory
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);

    // get directory path
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    m_settings->m_recentDir,
                                                    QFileDialog::ShowDirsOnly |
                                                    QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
    {
        return;
    }

    m_settings->m_recentDir = dir;

    // stimulus view
    QString path0 = QDir::cleanPath(dir + QDir::separator() +
                                    QString("stimulus.png"));
    screenshotSceneToPath(m_mainView, path0);

    // timeline
    QString path1 = QDir::cleanPath(newPath(dir + QDir::separator() +
                                    QString("timeline.png")));
    screenshotSceneToPath(m_timelineView, path1);

    // histogram
    QString path2 = QDir::cleanPath(newPath(dir + QDir::separator() +
                                    QString("histogram.png")));
    startPNGExport(m_ui->m_histogramPlot, path2, false);

    // scatterplot
    QString path3 = QDir::cleanPath(newPath(dir + QDir::separator() +
                                    QString("scaterplot.png")));
    startPNGExport(m_ui->m_scatterplot, path3, false);

    // data plot
    QString path4 = QDir::cleanPath(newPath(dir + QDir::separator() +
                                    QString("dataPlot.png")));
    screenshotDataPlotToPath(path4);

    qApp->restoreOverrideCursor();
}


void MainWindow::startPNGExport(bool fullScene, QGraphicsView* view, QString filePath,
                                bool modifyPath)
{
    auto pngExporter = new PNGExporter(this);

    QString propertiesString;
    QString fileNameSuggestion;

    if (fullScene)
    {
        propertiesString = tr("scene");
    }
    else
    {
        propertiesString = tr("view");
    }

    QString file;
    if (m_settings->m_displayMode == Individual)
    {
        if (!m_settings->m_currentTrials.empty())
        {
            file = m_settings->m_currentTrials[0]->getParticipant()->modifiedName();
        }
    }
    else
    {
        file = "GroupExport";
    }

    fileNameSuggestion = tr("%1/%2_%3.png").arg(m_settings->m_recentDir, file, propertiesString);

    if (filePath.isEmpty())
    {
        filePath = pngExporter->openSaveDialog(fileNameSuggestion,
                                               tr("images"));
    }
    else if (modifyPath)
    {
        filePath.append(QDir::separator() + fileNameSuggestion);
    }
    if (pngExporter->exportScene(view,
                                 getFilterSettingString() +
                                 getMicrosaccadesDetectionSettingString() +
                                 getFixationDetectionSettingString() +
                                 getCreationTimeString(),
                                 fullScene, filePath))
    {
        m_settings->m_recentDir = QFileInfo(filePath).path();
    }
}


void MainWindow::startPNGExport(QWidget* widget, QString filePath, bool modifyPath)
{
    auto pngExporter = new PNGExporter(this);

    QString fileNameSuggestion;

    QString file;
    if (m_settings->m_displayMode == Individual)
    {
        if (!m_settings->m_currentTrials.empty())
        {
            file = m_settings->m_currentTrials[0]->getParticipant()->modifiedName();
        }
    }
    else
    {
        if (!m_settings->m_currentTrials.empty() && m_settings->m_currentTrials[0] != nullptr)
        {
            file = "GroupExport";
        }
    }

    fileNameSuggestion = tr("%1/%2.png").arg(m_settings->m_recentDir, file);

    if (filePath.isEmpty())
    {
        filePath = pngExporter->openSaveDialog(fileNameSuggestion,
                                               tr("images"));
    }
    else if (modifyPath)
    {
        filePath.append(QDir::separator() + fileNameSuggestion);
    }

    if (pngExporter->exportScene(widget,
                                 getFilterSettingString() +
                                 getPlotSettingString() +
                                 getMicrosaccadesDetectionSettingString() +
                                 getFixationDetectionSettingString() +
                                 getCreationTimeString(),
                                 filePath))
    {
        m_settings->m_recentDir = QFileInfo(filePath).path();
    }
}


void MainWindow::screenshotSceneToPath(QWidget* viewObject, QString fielPath)
{
    auto view = dynamic_cast<QGraphicsView*>(viewObject);
    startPNGExport(true, view, fielPath, false);
}


void MainWindow::screenshotDataPlotToPath(QString fielPath)
{
    startPNGExport(m_ui->m_dataPlot, fielPath, false);
}


QString MainWindow::getFilterSettingString() const
{
    // list with properties of the WOI River
    QStringList settingsList;
    QString tempString;

    tempString = tr("DisplayMode");
    switch (m_settings->m_displayMode)
    {
        case Individual:
            settingsList << tr("%1 : Individual").arg(tempString);
            if (!m_settings->m_currentTrials.empty() &&
                m_settings->m_currentTrials[0]->getParticipant() != nullptr)
            {
                settingsList << tr("CurrentTrial : %1").
                                arg(m_settings->m_currentTrials[0]->name());
                settingsList << tr("CurrentParticipant : %1").
                                arg(m_settings->m_currentTrials[0]->getParticipant()->name());
            }
            break;
        case Multi:
            settingsList << tr("%1 : Multi").arg(tempString);

            QStringList currentTrialsStrings;
            for (size_t i = 0; i < m_settings->m_currentTrials.size(); ++i)
            {
                currentTrialsStrings.append(tr("%1 - %2").
                                            arg(m_settings->m_currentTrials[i]->name(),
                                                m_settings->m_currentTrials[i]->getParticipant()->name()));
            }
            settingsList << currentTrialsStrings.join(" | ");
            break;
    }

    // eye
    settingsList << tr("EyeType: %1").arg(m_settings->m_gazeType == GT_Right ? "Right" :
                                          m_settings->m_gazeType == GT_Left ? "Left" :
                                          m_settings->m_gazeType == GT_Average ? "Average": "None");

    // time range
    settingsList << tr("LimitTimeRange: %1").arg(QString::number(m_settings->m_limitTimeRange));
    settingsList << tr("TimeRange: %1 - %2").arg(QString::number(m_settings->m_startTimeInMSec),
                                                 QString::number(m_settings->m_endTimeInMSec));

    // visibility
    // main view
    settingsList << tr("Stimulus_BackgroundOpacity : %1").
                    arg(QString::number(m_settings->m_backgroundOpacity));
    settingsList << tr("Stimulus_ShowSamples : %1").
                    arg(QString::number(m_settings->m_showSamples));
    settingsList << tr("Stimulus_ShowSampleConnections : %1").
                    arg(QString::number(m_settings->m_showSampleConnections));
    settingsList << tr("Stimulus_ShowSaccadeDirections : %1").
                    arg(QString::number(m_settings->m_showSaccadeDirections));
    settingsList << tr("Stimulus_ShowScanpath : %1").
                    arg(QString::number(m_settings->m_showScanpath));

    tempString = "Stimulus_FixationSize";
    switch (m_settings->m_fixationSize)
    {
        case FromDuration:
            settingsList << tr("%1 : Duration").arg(tempString);
            break;
        case Equal:
            settingsList << tr("%1 : Equal").arg(tempString);
            break;
        case FromMicrosaccadesCount:
            settingsList << tr("%1 : MicrosaccadesCount").arg(tempString);
            break;
    }

    settingsList << tr("Stimulus_FixationScale : %1").
                    arg(QString::number(m_settings->m_fixationScale));
    settingsList << tr("Stimulus_HighlightCurrentFixation : %1").
                    arg(QString::number(m_settings->m_highlightCurrentFixation));
    settingsList << tr("Stimulus_HighlightFixations : %1").
                    arg(QString::number(m_settings->m_highlightFixations));
    settingsList << tr("Stimulus_HighlightMicrosaccades : %1").
                    arg(QString::number(m_settings->m_showMicrosaccades));
    settingsList << tr("Stimulus_HighlightMicrosaccadeDirections : %1").
                    arg(QString::number(m_settings->m_showMicrosaccadeDirections));

    // timeline view
    settingsList << tr("Timeline_ShowVelocity : %1").
                    arg(QString::number(m_settings->m_showVelocityValuesInTimeline));
    settingsList << tr("Timeline_ShowY : %1").
                    arg(QString::number(m_settings->m_showXValuesInTimeline));
    settingsList << tr("Timeline_ShowX : %1").
                    arg(QString::number(m_settings->m_showYValuesInTimeline));
    settingsList << tr("Timeline_ShowFixations : %1").
                    arg(QString::number(m_settings->m_showFixationsInTimeline));
    settingsList << tr("Timeline_ShowEvents : %1").
                    arg(QString::number(m_settings->m_showEventsInTimeline));
    settingsList << tr("Timeline_ShowMicrosaccades : %1").
                    arg(QString::number(m_settings->m_showMicrosaccadesInTimeline));

    settingsList << tr("ShowNeighboringFixations : %1").
                    arg(QString::number(m_settings->m_showNeighboringFixations));
    settingsList << tr("FollowingFixationsCount : %1").
                    arg(QString::number(m_settings->m_followingFixationsCount));
    settingsList << tr("PreviousFixationsCount : %1").
                    arg(QString::number(m_settings->m_previousFixationsCount));

    settingsList << tr("ZoomToVisibleFixationArea : %1").
                    arg(QString::number(m_settings->m_activateZoomToFixationArea));

    QString settingsString = settingsList.join("\n");
    settingsString.append("\n");
    return settingsString;
}


QString MainWindow::getPlotSettingString() const
{
    // list with properties of the WOI River
    QStringList settingsList;

    QString tempString;

    tempString = "DataPlotFixations";
    switch (m_settings->m_dataPlotFixations)
    {
        case PF_All:
            settingsList << tr("%1 : All").arg(tempString);
            break;
        case PF_Visible:
            settingsList << tr("%1 : Visible").arg(tempString);
            break;
        case PF_Current:
            settingsList << tr("%1 : CurrentFixation").arg(tempString);
            break;
    }

    tempString = "DataPlotData";
    switch (m_settings->m_dataPlotData)
    {
        case PD_Microsaccades:
            settingsList << tr("%1 : Microsaccades").arg(tempString);
            break;
        case PD_GazeRawData:
            settingsList << tr("%1 : GazeRawData").arg(tempString);
            break;
        case PD_Saccades:
            settingsList << tr("%1 : Saccades").arg(tempString);
            break;
    }

    tempString = "DataPlotDirection";
    switch (m_settings->m_dataPlotDirection)
    {
        case PDir_ToNextFixation:
            settingsList << tr("%1 : ToNextFixation").arg(tempString);
            break;
        case PDir_ToPreviousFixation:
            settingsList << tr("%1 : ToPreviousFixation").arg(tempString);
            break;
        case PDir_FromView:
            settingsList << tr("%1 : FromView").arg(tempString);
            break;
    }

    tempString = "DataPlotType";
    switch (m_settings->m_dataPlotType)
    {
        case PT_RegularPolarPlot:
            settingsList << tr("%1 : RegularPolarPlot").arg(tempString);
            break;
        case PT_PolarPlotWithHole:
            settingsList << tr("%1 : PolarPlotWithHole").arg(tempString);
            break;
        case PT_BoxRosePlot:
            settingsList << tr("%1 : BoxRosePlot").arg(tempString);
            break;
        case PT_RosePlot:
            settingsList << tr("%1 : RosePlot").arg(tempString);
            break;
        case PT_BoxRosePlotWithHole:
            settingsList << tr("%1 : BoxRosePlotWithHole").arg(tempString);
            break;
        case PT_RosePlotWithHole:
            settingsList << tr("%1 : RosePlotWithHole").arg(tempString);
            break;
        case PT_RadialScatterPlot:
            settingsList << tr("%1 : ScatterPlot").arg(tempString);
            break;
        case PT_RadialConnectedScatterPlot:
            settingsList << tr("%1 : ConnectedScatterPlot").arg(tempString);
            break;
        case PT_RadialRawDataPlot:
            settingsList << tr("%1 : RawDataPlot").arg(tempString);
            break;
        case PT_HistogramBarPlot:
            settingsList << tr("%1 : HistogramBarPlot").arg(tempString);
            break;
        case PT_None:
            settingsList << tr("%1 : None").arg(tempString);
            break;
    }

    tempString = "Overplot";
    switch (m_settings->m_overplot)
    {
        case PA_NoAggregation:
            settingsList << tr("%1 : PA_NoAggregation").arg(tempString);
            break;
        case PA_All:
            settingsList << tr("%1 : PA_All").arg(tempString);
            break;
        case PA_ForTestConditions:
            settingsList << tr("%1 : PA_ForTestConditions").arg(tempString);
            break;
        case PA_ForTrials:
            settingsList << tr("%1 : PA_ForTrials").arg(tempString);
            break;
        case PA_ForParticipants:
            settingsList << tr("%1 : PA_ForParticipants").arg(tempString);
            break;
    }

    settingsList << tr("DataPlotBins : %1").arg(QString::number(m_settings->m_dataPlotBins));
    settingsList << tr("DataPlotRadius : %1").arg(QString::number(m_settings->m_dataPlotRadiusCount));
    settingsList << tr("TestConditionColors : %1").arg(QString::number(m_settings->m_testConditionColors));

    QString settingsString = settingsList.join("\n");
    settingsString.append("\n");
    return settingsString;
}


QString MainWindow::getMicrosaccadesDetectionSettingString() const
{
    // list with properties of the WOI River
    QStringList settingsList;

    settingsList << tr("MS_UseMicrosaccadesFromInput : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_fromInputFile));
    settingsList << tr("MS_RelativeVelocityThreshold : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_velocityThreshold));
    settingsList << tr("MS_MinDuration : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_minDuration));
    settingsList << tr("MS_Binocular : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_binocular));
    settingsList << tr("MS_MaxDuration : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_maxDuration));
    settingsList << tr("MS_MaxAmplitude : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_maxAmplitude));
    settingsList << tr("MS_MinAmplitude : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_minAmplitude));
    settingsList << tr("MS_MinSaccadicInterval : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_minIntersaccInterval));
    settingsList << tr("MS_MinVelocity : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_minVelocity));
    settingsList << tr("MS_MaxVelocity : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_maxVelocity));
    settingsList << tr("MS_IgnoreStartFixationTime : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_ignoreAtStart));
    settingsList << tr("MS_IgnoreEndFixationTime : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_ignoreAtEnd));
    settingsList << tr("MS_VelocityWindowSize : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_velocityWindowSize));
    settingsList << tr("MS_IgnoreBeforeMissingData : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_ignoreBeforeMissingData));
    settingsList << tr("MS_IgnoreAfterMissingData : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_ignoreAfterMissingData));

    settingsList << tr("MS_UseMaxDuration : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMaxDuration));
    settingsList << tr("MS_UseMinAmplitude : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMinAmplitude));
    settingsList << tr("MS_UseMaxAmplitude : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMaxAmplitude));
    settingsList << tr("MS_UseMinIntersaccInterval : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMinIntersaccInterval));
    settingsList << tr("MS_UseMinVelocity : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMinVelocity));
    settingsList << tr("MS_UseMaxVelocity : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useMaxVelocity));
    settingsList << tr("MS_UseIgnoreStartFixationTime : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useIgnoreAtStart));
    settingsList << tr("MS_UseIgnoreEndFixationTime : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useIgnoreAtEnd));
    settingsList << tr("MS_UseIgnoreBeforeMissingData : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useIgnoreBeforeMissingData));
    settingsList << tr("MS_UseIgnoreAfterMissingData : %1").arg(QString::number(m_settings->m_microsaccadeSettings->m_useIgnoreAfterMissingData));

    QString settingsString = settingsList.join("\n");
    settingsString.append("\n");
    return settingsString;
}


QString MainWindow::getFixationDetectionSettingString() const
{
    // list with properties of the WOI River
    QStringList settingsList;

    settingsList << tr("Fix_UseFixationsFromInput : %1").arg(QString::number(m_settings->m_fixationSettings->m_fromInputFile));
    settingsList << tr("Fix_RelativeVelocityThreshold : %1").arg(QString::number(m_settings->m_fixationSettings->m_velocityThreshold));
    settingsList << tr("Fix_MinDuration : %1").arg(QString::number(m_settings->m_fixationSettings->m_minDuration));
    settingsList << tr("Fix_Binocular : %1").arg(QString::number(m_settings->m_fixationSettings->m_binocular));
    settingsList << tr("Fix_MaxDuration : %1").arg(QString::number(m_settings->m_fixationSettings->m_maxDuration));
    settingsList << tr("Fix_MaxAmplitude : %1").arg(QString::number(m_settings->m_fixationSettings->m_maxAmplitude));
    settingsList << tr("Fix_MinAmplitude : %1").arg(QString::number(m_settings->m_fixationSettings->m_minAmplitude));
    settingsList << tr("Fix_MinSaccadicInterval : %1").arg(QString::number(m_settings->m_fixationSettings->m_minIntersaccInterval));
    settingsList << tr("Fix_MinVelocity : %1").arg(QString::number(m_settings->m_fixationSettings->m_minVelocity));
    settingsList << tr("Fix_MaxVelocity : %1").arg(QString::number(m_settings->m_fixationSettings->m_maxVelocity));
    settingsList << tr("Fix_IgnoreStartTime : %1").arg(QString::number(m_settings->m_fixationSettings->m_ignoreAtStart));
    settingsList << tr("Fix_IgnoreEndTime : %1").arg(QString::number(m_settings->m_fixationSettings->m_ignoreAtEnd));
    settingsList << tr("Fix_VelocityWindowSize : %1").arg(QString::number(m_settings->m_fixationSettings->m_velocityWindowSize));
    settingsList << tr("Fix_IgnoreBeforeMissingData : %1").arg(QString::number(m_settings->m_fixationSettings->m_ignoreBeforeMissingData));
    settingsList << tr("Fix_IgnoreAfterMissingData : %1").arg(QString::number(m_settings->m_fixationSettings->m_ignoreAfterMissingData));

    settingsList << tr("Fix_UseMaxDuration : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMaxDuration));
    settingsList << tr("Fix_UseMinAmplitude : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMinAmplitude));
    settingsList << tr("Fix_UseMaxAmplitude : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMaxAmplitude));
    settingsList << tr("Fix_UseMinIntersaccInterval : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMinIntersaccInterval));
    settingsList << tr("Fix_UseMinVelocity : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMinVelocity));
    settingsList << tr("Fix_UseMaxVelocity : %1").arg(QString::number(m_settings->m_fixationSettings->m_useMaxVelocity));
    settingsList << tr("Fix_UseIgnoreStartTime : %1").arg(QString::number(m_settings->m_fixationSettings->m_useIgnoreAtStart));
    settingsList << tr("Fix_UseIgnoreEndTime : %1").arg(QString::number(m_settings->m_fixationSettings->m_useIgnoreAtEnd));
    settingsList << tr("Fix_useIgnoreBeforeMissingData : %1").arg(QString::number(m_settings->m_fixationSettings->m_useIgnoreBeforeMissingData));
    settingsList << tr("Fix_useIgnoreAfterMissingData : %1").arg(QString::number(m_settings->m_fixationSettings->m_useIgnoreAfterMissingData));

    QString settingsString = settingsList.join("\n");
    settingsString.append("\n");
    return settingsString;
}

/************************************************************************************************
 * QString MainWindow::getCreationTimeString() const
 ************************************************************************************************/
QString MainWindow::getCreationTimeString() const
{
    return tr("CreationTime: %1\n").arg(
              dateToString(QDateTime::currentDateTime()));
}

/*************************************************************************************************
 * QString MainWindow::dateToString(const QDateTime date, bool withSpace) const
 ************************************************************************************************/
QString MainWindow::dateToString(const QDateTime date, bool withSpace) const
{
    if (withSpace)
    {
        return date.toString(tr("dd.MM.yyyy hh:mm:ss"));
    }
    else
    {
        return date.toString(tr("yyyyMMddThhmmss"));
    }
}


int MainWindow::showFileImportDialog()
{
    QDialog* myDialog = new QDialog(this);
    myDialog->setWindowTitle(tr("Drop file"));

    QLabel* textLabel = new QLabel(tr("Do you want to add new participants or delete and replace "
                                      "all existing ones?"));

    // buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

    QPushButton* button1 = new QPushButton(tr("Add New Ones"));
    QPushButton* button2 = new QPushButton(tr("Replace Existing Ones"));

    buttonBox->addButton(button1, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(button2, QDialogButtonBox::AcceptRole);

    // connection of the buttons to close the window and start the creation of the tree
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, myDialog, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, myDialog, &QDialog::accept);

    QObject::connect(button1, &QPushButton::clicked, this, &MainWindow::addDragDropFiles);
    QObject::connect(button2, &QPushButton::clicked, this, &MainWindow::openDragDropFiles);

    // layout
    auto layout = new QVBoxLayout;
    layout->addWidget(textLabel);
    layout->addSpacing(20);
    layout->addWidget(buttonBox);

    myDialog->setLayout(layout);

    myDialog->exec();

    return 0;
}


QString MainWindow::getFileName(const QString& reason, const QString& fileTypes,
                                const QString& recentFile) const
{
    return QFileDialog::getOpenFileName(nullptr, reason, recentFile, fileTypes);
}


std::vector<qreal> MainWindow::createSample(const qreal time, const qreal rightX, const qreal rightY)
{
    std::vector<qreal> sample;

    sample.push_back(time);
    sample.push_back(0); // left
    sample.push_back(0); // left
    sample.push_back(0); // left
    sample.push_back(rightX); // right
    sample.push_back(rightY); // right
    sample.push_back(0); // right
    sample.push_back(0); // both
    sample.push_back(0); // both
    sample.push_back(0); // both

    return sample;
}


QString MainWindow::newPath(const QString path)
{
    QFileInfo check_file(path);
    QString newPath = path;
    int count = 1;
    while (check_file.exists())
    {
        QStringList split = path.split(".");
        newPath = split.mid(0, split.size() - 1).join(".") + "_" + QString::number(count) + "." + split.last();
        check_file = QFileInfo(newPath);
        count++;
    }
    return newPath;
}


void MainWindow::openDragDropFiles()
{
    openFiles(m_dragDropFiles);
    m_dragDropFiles.clear();
}


void MainWindow::addDragDropFiles()
{
    openFiles(m_dragDropFiles, true);
    m_dragDropFiles.clear();
}


void MainWindow::open()
{
    QStringList fileName = QFileDialog::getOpenFileNames(
        this, tr("Open file"), m_settings->m_recentDir, tr("%2 (*.%1)").arg(eyeTrackingDataFile,
                                                                            eyeTrackingDataFileDescription));
    openFiles(fileName);
}


void MainWindow::add()
{
    QStringList fileName = QFileDialog::getOpenFileNames(
        this, tr("Open file"), m_settings->m_recentDir, tr("%2 (*.%1)").arg(eyeTrackingDataFile,
                                                                            eyeTrackingDataFileDescription));
    openFiles(fileName, true);
}



void MainWindow::openCSV()
{
    if (m_eyeTrackingData->numberOfParticipants() == 0)
    {
        QMessageBox::warning(this, tr("No Participants Loaded"),
                             tr("No participants/trials available. "
                                "You need to load a %1 file first.").arg(eyeTrackingDataFile));
        return;
    }

    QString fileName = getFileName(tr("Open file"), tr("E%2 (*.%1)").arg(eyeTrackingTestConditions,
                                                                         eyeTrackingTestConditionsDescription),
                                   m_settings->m_recentDir);

    EyeDataImport eyeDataImport(this, m_eyeTrackingData, m_settings);
    connect(&eyeDataImport, &EyeDataImport::updateParticipantsInUI, this, &MainWindow::addParticipantsInUI);
    connect(&eyeDataImport, &EyeDataImport::setTestConditionsInUI, m_ui, &UserInterface::setTestConditionsInUI);
    eyeDataImport.readCSV(fileName);
}


void MainWindow::loadMicrosaccadsSettingsFile()
{
    QString fileName = getFileName(tr("Open Microsaccades Parameters"),
                                   tr("%2 (*.%1)").arg(MicrosaccadesParameterFile,
                                                       MicrosaccadesParameterFileDescription));

    SettingsImport settingsImport(this, m_settings, m_ui);
    settingsImport.readMicrosaccadsSettingsFile(fileName);
}


void MainWindow::loadFixationSettingsFile()
{
    QString fileName = getFileName(tr("Open Fixation Parameters"),
                                   tr("%2 (*.%1)").arg(FixationParameterFile,
                                                       FixationParameterFileDescription));

    SettingsImport settingsImport(this, m_settings, m_ui);
    settingsImport.readFixationSettingsFile(fileName);
}


void MainWindow::loadColorSettingsFile()
{
    QString fileName = getFileName(tr("Open Color Settings"),
                                   tr("%2 (*.%1)").arg(ColorSettingsFile,
                                                       ColorSettingsFileDescription));

    SettingsImport settingsImport(this, m_settings, m_ui);
    settingsImport.readColorSettingsFile(fileName);
}


void MainWindow::updateCurrentMicrosaccadesDetection()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    if (!m_settings->m_currentTrials.empty())
    {
        updateMicrosaccadesDetection(m_settings->m_currentTrials[0]);
    }
    updateUI();

    qApp->restoreOverrideCursor();
}


void MainWindow::updateMicrosaccadesForParticipant()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    QElapsedTimer timer;
    timer.start();

    if (!m_settings->m_currentTrials.empty())
    {
        Participant* p = m_settings->m_currentTrials[0]->getParticipant();
        for (size_t j = 0; j < p->numberOfTrials(); ++j)
        {
            updateMicrosaccadesDetection(p->getTrialAt(int(j)));
        }
    }

    qDebug() << QString("updateMicrosaccadesForParticipant: %1 ms").arg(timer.elapsed());

    updateUI();

    qApp->restoreOverrideCursor();
}


void MainWindow::updateMicrosaccadesForAllDetection()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    QElapsedTimer timer;
    timer.start();

    for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
    {
        for (size_t j = 0; j < m_eyeTrackingData->participantAt(int(i))->numberOfTrials(); ++j)
        {
            updateMicrosaccadesDetection(m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j)));
        }
    }

    qDebug() << QString("updateMicrosaccadesForAllDetection: %1 ms").arg(timer.elapsed());

    updateUI();

    qApp->restoreOverrideCursor();
}


void MainWindow::updateCurrentFixationsDetection()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    if (!m_settings->m_currentTrials.empty())
    {
        updateFixationsDetection(m_settings->m_currentTrials[0]);

        updateUI();
    }

    qApp->restoreOverrideCursor();
}


void MainWindow::updateFixationsForCurrentParticipantDetection()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    if (!m_settings->m_currentTrials.empty())
    {
        for (size_t j = 0; j < m_settings->m_currentTrials[0]->getParticipant()->numberOfTrials(); ++j)
        {
            updateFixationsDetection(m_settings->m_currentTrials[0]->getParticipant()->getTrialAt(int(j)));
        }

        updateUI();
    }

    qApp->restoreOverrideCursor();
}


void MainWindow::updateFixationsForAllDetection()
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
    {
        for (size_t j = 0; j < m_eyeTrackingData->participantAt(int(i))->numberOfTrials(); ++j)
        {
            updateFixationsDetection(m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j)));
        }
    }

    updateUI();

    qApp->restoreOverrideCursor();
}


void MainWindow::resetMainViewZoom()
{
    m_mainView->resetView();
}


void MainWindow::zoomInMainView()
{
    m_mainView->zoomIn();
}


void MainWindow::zoomOutMainView()
{
    m_mainView->zoomOut();
}


void MainWindow::zoomInTimeline()
{
    m_timelineView->zoomIn();
}


void MainWindow::zoomOutTimeline()
{
    m_timelineView->zoomOut();
}


void MainWindow::resetTimelineZoom()
{
    m_timelineView->resetView();
    m_timelineView->updateAll();
}


void MainWindow::determineTrials()
{
    qApp->processEvents();
    if (m_settings == nullptr || m_ui == nullptr)
    {
        return;
    }

    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();

    if (m_settings->m_displayMode == Multi)
    {
        std::set<int> checkedParticipants;
        for (int i = 0; i < m_ui->m_participantList->count(); ++i)
        {
            if (m_ui->m_participantList->item(i)->checkState() == Qt::Checked)
            {
                checkedParticipants.insert(i);
            }
        }

        std::set<QString> checkedTrials;
        for (int i = 0; i < m_ui->m_trialList->count(); ++i)
        {
            if (m_ui->m_trialList->item(i)->checkState() == Qt::Checked)
            {
                checkedTrials.insert(m_ui->m_trialList->item(i)->text());
            }
        }

        std::set<QString> checkedTestConditions;
        for (int i = 0; i < m_ui->m_testConditionList->count(); ++i)
        {
            if (m_ui->m_testConditionList->item(i)->checkState() == Qt::Checked)
            {
                checkedTestConditions.insert(m_ui->m_testConditionList->item(i)->text());
            }
        }

        m_settings->m_currentTrials = getTrials(checkedParticipants, checkedTrials, checkedTestConditions);
    }
    else
    {
        int p = m_ui->m_participantCombobox->currentIndex();
        int t = m_ui->m_trialCombobox->currentIndex();
        m_settings->m_currentTrials.clear();
        m_settings->m_currentTrials.push_back(m_eyeTrackingData->participantAt(p)->getTrialAt(t));
    }

    qApp->restoreOverrideCursor();
}


void MainWindow::screenshotView(QWidget* viewObject)
{
    auto view = dynamic_cast<QGraphicsView*>(viewObject);
    startPNGExport(false, view);
}


void MainWindow::screenshotScene(QWidget* viewObject)
{
    auto view = dynamic_cast<QGraphicsView*>(viewObject);
    startPNGExport(true, view);
}


void MainWindow::screenshotDataPlot()
{
    startPNGExport(m_ui->m_dataPlot);
}


void MainWindow::screenshotHistogram()
{
    startPNGExport(m_ui->m_histogramPlot);
}


void MainWindow::screenshotScatterplot()
{
    startPNGExport(m_ui->m_scatterplot);
}


void MainWindow::fixationSelected(Fixation* fixation)
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }

    if (fixation == nullptr)
    {
        m_settings->m_currentFixationIndex = -1;
    }
    else
    {
        int index = m_settings->m_currentTrials[0]->getFixationIndex(fixation);
        m_settings->m_currentFixationIndex = index;
    }

    for (int i = 0; i < int(m_settings->m_currentTrials[0]->fixationsCount()); ++i)
    {
        m_settings->m_currentTrials[0]->getFixationAt(i)->setMarked(false);
    }

    if (fixation != nullptr)
    {
        fixation->setMarked(true);
    }

    if (m_settings->m_dataPlotFixations == PF_Current)
    {
        showDataPlotForTrials();
    }
    updateMainAndTimelineView();

    // show information on fixation/update others...
    updateFixationInfo(selectedFixation());

    selectFixationInTable(fixation);
}


void MainWindow::fixationIndexSelected(int index)
{
    if (index != -1 && !m_settings->m_currentTrials.empty())
    {
        fixationSelected(m_settings->m_currentTrials[0]->getFixationAt(index));
    }
}


void MainWindow::selectFixationInTable(Fixation* fixation)
{
    if (m_settings->m_currentTrials.empty())
    {
        return;
    }
    if ((!m_ui->m_fixationsTable->selectedItems().empty() &&
            m_ui->m_fixationsTable->currentRow() == m_settings->m_currentTrials[0]->getFixationIndex(fixation)) ||
            (m_ui->m_fixationsTable->selectedItems().empty() &&
            m_settings->m_currentTrials[0]->getFixationIndex(fixation) == -1))
    {
        return;
    }


    int index = m_settings->m_currentTrials[0]->getFixationIndex(fixation);
    if (index >= 0 && index < m_ui->m_fixationsTable->rowCount())
    {
        m_ui->m_fixationsTable->selectRow(index);
        m_ui->m_fixationsTable->scrollToItem(m_ui->m_fixationsTable->selectedItems()[0],
                                             QAbstractItemView::EnsureVisible);
    }
    else
    {
        m_ui->m_fixationsTable->clearSelection();
        m_ui->m_fixationsTable->clearFocus();
        fixationSelected(nullptr);
    }

    showDataPlotForTrials();
}


void MainWindow::selectNextFixation()
{
    if (!m_settings->m_currentTrials.empty() &&
            m_settings->m_currentFixationIndex + 1 < m_ui->m_fixationsTable->rowCount())
    {
        fixationSelected(m_settings->m_currentTrials[0]->getFixationAt(m_settings->m_currentFixationIndex + 1));
    }
}


void MainWindow::selectPreviousFixation()
{
    if (!m_settings->m_currentTrials.empty() && m_settings->m_currentFixationIndex >= 1)
    {
        fixationSelected(m_settings->m_currentTrials[0]->getFixationAt(m_settings->m_currentFixationIndex - 1));
    }
}


void MainWindow::participantChanged(int index)
{
    if (index >= 0)
    {
        m_ui->selectParticipant(m_eyeTrackingData->participantAt(index));
    }
}


void MainWindow::trialChanged(int index)
{
    if (!m_settings->m_currentTrials.empty() &&
            m_settings->m_currentTrials[0]->getParticipant() != nullptr &&
            m_settings->m_currentTrials[0] != m_settings->m_currentTrials[0]->getParticipant()->getTrialAt(index))
    {
        showData();
    }
}


void MainWindow::gazeTypeChanged(int index)
{
    m_settings->m_gazeType = static_cast<GazeType>(index);

    updateMicrosaccadeStatBarsInUI();

    showData(false);
}


void MainWindow::showMicrosaccades(bool show)
{
    if (m_settings->m_showMicrosaccades != show)
    {
        m_settings->m_showMicrosaccades = show;

        updateMainView();
    }
}


void MainWindow::showSamples(bool show)
{
    if (m_settings->m_showSamples != show)
    {
        m_settings->m_showSamples = show;

        updateMainView();
    }
}


void MainWindow::showSampleConnections(bool show)
{
    if (m_settings->m_showSampleConnections != show)
    {
        m_settings->m_showSampleConnections = show;

        updateMainView();
    }
}


void MainWindow::showMicrosaccadesDirection(bool show)
{
    if (m_settings->m_showMicrosaccadeDirections != show)
    {
        m_settings->m_showMicrosaccadeDirections = show;

        updateMainView();
    }
}


void MainWindow::highlightFixations(bool highlight)
{
    if (m_settings->m_highlightFixations != highlight)
    {
        m_settings->m_highlightFixations = highlight;

        updateMainView();
    }
}


void MainWindow::highlightCurrentFixation(bool highlight)
{
    if (m_settings->m_highlightCurrentFixation != highlight)
    {
        m_settings->m_highlightCurrentFixation = highlight;

        updateMainView();
    }
}


void MainWindow::showSaccadeDirections(bool show)
{
    if (m_settings->m_showSaccadeDirections != show)
    {
        m_settings->m_showSaccadeDirections = show;

        updateMainView();
    }
}


void MainWindow::showScanPath(bool show)
{
    if (m_settings->m_showScanpath != show)
    {
        m_settings->m_showScanpath = show;
        updateMainView();
    }
}


void MainWindow::backgroundOpacityChanged(int value)
{
    if (m_settings->m_backgroundOpacity != value)
    {
        m_settings->m_backgroundOpacity = value;
        m_mainView->updateAll();
    }
}


void MainWindow::showMicrosaccadesInTimeline(bool show)
{
    if (m_settings->m_showMicrosaccadesInTimeline != show)
    {
        m_settings->m_showMicrosaccadesInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showFixationsInTimeline(bool show)
{
    if (m_settings->m_showFixationsInTimeline != show)
    {
        m_settings->m_showFixationsInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showEventsInTimeline(bool show)
{
    if (m_settings->m_showEventsInTimeline != show)
    {
        m_settings->m_showEventsInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showXValuesInTimeline(bool show)
{
    if (m_settings->m_showXValuesInTimeline != show)
    {
        m_settings->m_showXValuesInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showYValuesInTimeline(bool show)
{
    if (m_settings->m_showYValuesInTimeline != show)
    {
        m_settings->m_showYValuesInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showVelocityValuesInTimeline(bool show)
{
    if (m_settings->m_showVelocityValuesInTimeline != show)
    {
        m_settings->m_showVelocityValuesInTimeline = show;

        updateTimelineView();
    }
}


void MainWindow::showNeighboringFixations(bool show)
{
    if (m_settings->m_showNeighboringFixations != show)
    {
        m_settings->m_showNeighboringFixations = show;

        showDataPlotForTrials();
        updateMainAndTimelineView();
    }
}


void MainWindow::followingFixationsCountChanged(int value)
{
    if (m_settings->m_followingFixationsCount != value)
    {
        m_settings->m_followingFixationsCount = value;

        showDataPlotForTrials();
        updateMainAndTimelineView();
    }
}


void MainWindow::previousFixationsCountChanged(int value)
{
    if (m_settings->m_previousFixationsCount != value)
    {
        m_settings->m_previousFixationsCount = value;

        showDataPlotForTrials();
        updateMainAndTimelineView();
    }
}


void MainWindow::fixationSizeChanged(int index)
{
    FixationSize size = FromDuration;
    switch (index)
    {
        case 0:
            size = FromDuration;
            break;
        case 1:
            size = Equal;
            break;
        case 2:
            size = FromMicrosaccadesCount;
            break;
    }

    if (m_settings->m_fixationSize != size)
    {
        m_settings->m_fixationSize = size;
        m_mainView->updateAll();
    }
}


void MainWindow::fixationScaleValueChanged(int value)
{
    m_settings->m_fixationScale = value;
    m_mainView->updateAll();
}


void MainWindow::activateZoomToFixationArea(bool use)
{
    if (m_settings->m_activateZoomToFixationArea != use)
    {
        m_settings->m_activateZoomToFixationArea = use;

        updateMainAndTimelineView();
    }
}


void MainWindow::activateDataTimeLimit(bool use)
{
    if (m_settings->m_limitTimeRange != use)
    {
        m_settings->m_limitTimeRange = use;
        updateDataTimeLimit();
    }
}


void MainWindow::updateDataTimeLimit()
{
    if (m_ui->m_endTimeLimitSpinBox->value() < m_ui->m_startTimeLimitSpinBox->value())
    {
        m_ui->m_endTimeLimitSpinBox->setValue(m_ui->m_startTimeLimitSpinBox->value());
    }

    m_settings->m_startTimeInMSec = m_ui->m_startTimeLimitSpinBox->value();
    m_settings->m_endTimeInMSec = m_ui->m_endTimeLimitSpinBox->value();

    updateUI();
}


void MainWindow::limitTimeRangeToSelection(int min, int max)
{
    if (m_settings->m_currentTrials[0] != nullptr)
    {
        m_ui->m_startTimeLimitSpinBox->setValue(m_settings->m_currentTrials[0]->samplesToTime(min));
        m_ui->m_endTimeLimitSpinBox->setValue(m_settings->m_currentTrials[0]->samplesToTime(max));
    }
}


void MainWindow::zoomToTimeLimit()
{
    m_timelineVisualization->zoomToRange(
                int(m_settings->m_currentTrials[0]->timeToSamples(m_settings->m_startTimeInMSec)),
                int(m_settings->m_currentTrials[0]->timeToSamples(m_settings->m_endTimeInMSec)));
}


void MainWindow::addParticipantsInUI()
{
    QList<QString> trialList;
    if (m_eyeTrackingData->numberOfParticipants() > 0)
    {
        int currentIndex = std::max(0, m_ui->m_participantCombobox->currentIndex());
        m_ui->m_participantCombobox->clear();

        std::set<int> checkedParticipants;
        std::set<int> checkedTrials;
        for (int i = 0; i < m_ui->m_participantList->count(); ++i)
        {
            if (m_ui->m_participantList->item(i)->checkState() == Qt::Checked)
            {
                checkedParticipants.insert(i);
            }
        }
        for (int i = 0; i < m_ui->m_trialList->count(); ++i)
        {
            if (m_ui->m_trialList->item(i)->checkState() == Qt::Checked)
            {
                checkedTrials.insert(i);
            }
        }
        m_ui->m_participantList->clear();
        m_ui->m_trialList->clear();

        m_ui->m_participantList->setIconSize(QSize(100, 15));
        m_ui->m_trialList->setIconSize(QSize(100, 15));

        for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
        {
            m_ui->m_participantCombobox->addItem(m_eyeTrackingData->participantAt(int(i))->name());

            QListWidgetItem* item = new QListWidgetItem(QIcon(), m_eyeTrackingData->participantAt(int(i))->name(),
                                                        m_ui->m_participantList, QListWidgetItem::UserType);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);

            for (size_t j = 0; j < m_eyeTrackingData->participantAt(int(i))->numberOfTrials(); ++j)
            {
                QString name = m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j))->name();
                if (!trialList.contains(name))
                {
                    trialList.append(name);
                }
            }
        }
        m_ui->m_participantCombobox->setCurrentIndex(currentIndex);

        for (int i = 0; i < trialList.size(); ++i)
        {
            QListWidgetItem* item = new QListWidgetItem(QIcon(), trialList[i], m_ui->m_trialList,
                                                        QListWidgetItem::UserType);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }

        if (!m_settings->trialsLoaded && !m_settings->participantsLoaded)
        {
            m_ui->m_participantList->item(0)->setCheckState(Qt::Checked);
            m_ui->m_trialList->item(0)->setCheckState(Qt::Checked);
        }

        if (m_eyeTrackingData->numberOfParticipants() > 0)
        {
            m_ui->selectParticipant(m_eyeTrackingData->participantAt(0));
        }

        for (int i = 0; i < m_ui->m_participantList->count(); ++i)
        {
            if (checkedParticipants.find(i) != checkedParticipants.end())
            {
                m_ui->m_participantList->item(i)->setCheckState(Qt::Checked);
            }
        }
        for (int i = 0; i < m_ui->m_trialList->count(); ++i)
        {
            if (checkedTrials.find(i) != checkedTrials.end())
            {
                m_ui->m_trialList->item(i)->setCheckState(Qt::Checked);
            }
        }

        m_settings->trialsLoaded = true;
        m_settings->participantsLoaded = true;
    }
}


void MainWindow::addTestConditionsInUI()
{
    std::set<int> checkedTestConditions;
    for (int i = 0; i < m_ui->m_testConditionList->count(); ++i)
    {
        if (m_ui->m_testConditionList->item(i)->checkState() == Qt::Checked)
        {
            checkedTestConditions.insert(i);
        }
    }

    m_ui->m_testConditionList->clear();
    m_ui->m_testConditionList->setIconSize(QSize(100, 15));

    if (!m_settings->m_testConditions.empty())
    {
        QHash<QString, int> testConditionHash;
        int max = 0;
        for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
        {
            Participant* p = m_eyeTrackingData->participantAt(int(i));
            for (size_t j = 0; j < p->numberOfTrials(); ++j)
            {
                size_t ms = p->getTrialAt(int(j))->microsaccadesCount();

                testConditionHash[p->getTrialAt(int(j))->getTestConditionType()] =
                        testConditionHash[p->getTrialAt(int(j))->getTestConditionType()] + int(ms);
                max = std::max(max, testConditionHash[p->getTrialAt(int(j))->getTestConditionType()]);
            }
        }

        foreach (QString name, m_settings->m_testConditions.keys())
        {
            QListWidgetItem* item = new QListWidgetItem(QIcon(), name,
                                                        m_ui->m_testConditionList, QListWidgetItem::UserType);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);

            if (!m_settings->testConditionsLoaded)
            {
                item->setCheckState(Qt::Checked);
            }
        }

        for (int i = 0; i < m_ui->m_testConditionList->count(); ++i)
        {
            if (checkedTestConditions.find(i) != checkedTestConditions.end())
            {
                m_ui->m_testConditionList->item(i)->setCheckState(Qt::Checked);
            }
        }
    }

    updateUI();

    m_settings->testConditionsLoaded = true;
}


void MainWindow::updateMicrosaccadeStatBarsInUI()
{
    QList<QString> trialList;
    if (m_eyeTrackingData->numberOfParticipants() > 0)
    {
        // dtermine number of microsaccades in trials/for participants
        QMap<QString, int> participantMap;
        QMap<QString, int> trialMap;
        QMap<QString, int> testConditionMap;
        int maxP = 0;
        int maxT = 0;
        int maxTC = 0;
        for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
        {
            Participant* p = m_eyeTrackingData->participantAt(int(i));

            for (size_t j = 0; j < p->numberOfTrials(); ++j)
            {
                size_t ms = p->getTrialAt(int(j))->microsaccadesCount();
                participantMap[p->name()] = participantMap[p->name()] + int(ms);
                trialMap[p->getTrialAt(int(j))->name()] = trialMap[p->getTrialAt(int(j))->name()] + int(ms);
                testConditionMap[p->getTrialAt(int(j))->getTestConditionType()] =
                        testConditionMap[p->getTrialAt(int(j))->getTestConditionType()] + int(ms);
                maxP = std::max(maxP, participantMap[p->name()]);
                maxT = std::max(maxT, trialMap[p->getTrialAt(int(j))->name()]);
                maxTC = std::max(maxTC, testConditionMap[p->getTrialAt(int(j))->getTestConditionType()]);
            }
        }

        for (size_t i = 0; i < m_eyeTrackingData->numberOfParticipants(); ++i)
        {
            if (size_t(m_ui->m_participantList->count()) <= i)
            {
                break;
            }
            QPixmap p = QPixmap(maxP == 0 ? 0 : int(qreal(participantMap[m_eyeTrackingData->participantAt(int(i))->name()]) / qreal(maxP) * 100.0), 5);
            p.fill(*m_settings->m_colors.microsaccadesColor);
            m_ui->m_participantList->item(int(i))->setIcon(QIcon(p));

            for (size_t j = 0; j < m_eyeTrackingData->participantAt(int(i))->numberOfTrials(); ++j)
            {
                QString name = m_eyeTrackingData->participantAt(int(i))->getTrialAt(int(j))->name();
                if (!trialList.contains(name))
                {
                    trialList.append(name);
                }
            }
        }

        for (int i = 0; i < trialList.size(); ++i)
        {
            if (m_ui->m_trialList->count() <= i)
            {
                break;
            }
            QPixmap p = QPixmap(maxT == 0 ? 0 : int(qreal(trialMap[trialList[i]]) / qreal(maxT) * 100.0), 5);
            p.fill(*m_settings->m_colors.microsaccadesColor);
            m_ui->m_trialList->item(i)->setIcon(QIcon(p));
        }

        int i = 0;
        foreach (QString name, m_settings->m_testConditions.keys())
        {
            if (m_ui->m_testConditionList->count() <= i)
            {
                break;
            }
            QPixmap p = QPixmap(maxTC == 0 ? 0 : int(qreal(testConditionMap[name]) / qreal(maxTC) * 100.0), 5);
            p.fill(*m_settings->m_colors.microsaccadesColor);
            m_ui->m_testConditionList->item(i)->setIcon(QIcon(p));
            i++;
        }
    }
}


void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    m_settings->m_recentDir = settings.value("recentDir").toString();
}


void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings(m_settingsPath, QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("recentDir", m_settings->m_recentDir);
    QMainWindow::closeEvent(event);
}


void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}


void MainWindow::dropEvent(QDropEvent* e)
{
    // show wait cursor
    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();

    std::vector<QString> files;
    QString testConditionFile;
    QString msParameterFile;
    QString fixParameterFile;
    QString colorSettingsFile;

    foreach (const QUrl& url, e->mimeData()->urls())
    {
        QString fileName = url.toLocalFile();
        if (QFileInfo(fileName).suffix() == eyeTrackingDataFile)
        {
            files.push_back(fileName);
        }
        else if (QFileInfo(fileName).suffix() == eyeTrackingTestConditions)
        {
            testConditionFile = fileName;
        }
        else if (QFileInfo(fileName).suffix() == MicrosaccadesParameterFile)
        {
            msParameterFile = fileName;
        }
        else if (QFileInfo(fileName).suffix() == FixationParameterFile)
        {
            fixParameterFile = fileName;
        }
        else if (QFileInfo(fileName).suffix() == ColorSettingsFile)
        {
            colorSettingsFile = fileName;
        }
    }

    if (!files.empty())
    {
        std::sort(files.begin(), files.end());
        m_dragDropFiles = QStringList::fromVector(QVector<QString>::fromStdVector(files));
        if (m_eyeTrackingData->numberOfParticipants() == 0)
        {
            openDragDropFiles();
        }
        else
        {
            // ask if participants shall be added or replaced
            showFileImportDialog();
        }
    }

    if (!testConditionFile.isEmpty())
    {
        EyeDataImport eyeDataImport(this, m_eyeTrackingData, m_settings);
        connect(&eyeDataImport, &EyeDataImport::updateParticipantsInUI,
                this, &MainWindow::addParticipantsInUI);
        connect(&eyeDataImport, &EyeDataImport::setTestConditionsInUI, m_ui, &UserInterface::setTestConditionsInUI);
        eyeDataImport.readCSV(testConditionFile);
    }

    if (!msParameterFile.isEmpty())
    {
        SettingsImport settingsImport(this, m_settings, m_ui);
        settingsImport.readMicrosaccadsSettingsFile(msParameterFile);
    }

    if (!fixParameterFile.isEmpty())
    {
        SettingsImport settingsImport(this, m_settings, m_ui);
        settingsImport.readFixationSettingsFile(fixParameterFile);
    }

    if (!colorSettingsFile.isEmpty())
    {
        SettingsImport settingsImport(this, m_settings, m_ui);
        settingsImport.readColorSettingsFile(colorSettingsFile);
    }

    qApp->restoreOverrideCursor();
}
