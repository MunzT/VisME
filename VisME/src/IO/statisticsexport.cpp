/***********************************************************************************************//**
 * @author Tanja Munz
 * @file statisticsexport.cpp
 **************************************************************************************************/
#include "directioncalculations.h"
#include "mathhelpfunctions.h"
#include "statisticsexport.h"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextStream>


StatisticsExport::StatisticsExport(QWidget* parent, EyeTrackingData* eyeTrackingData, Settings* settings) :
    m_parent(parent),
    m_eyeTrackingData(eyeTrackingData),
    m_settings(settings)
{
    m_dirPath = QDir(qApp->applicationDirPath()).filePath("settings.ini");
}


void StatisticsExport::exportDataToFile()
{
    exportDirectionalData = false;

    m_exportDataCheckBoxes.clear();
    m_exportValueTypesCheckBoxes.clear();

    QDialog* myDialog = new QDialog(m_parent);
    myDialog->setWindowTitle(tr("Specify data for aggregated export"));

    QLabel* textLabel = new QLabel(tr("Files with aggregated participant-test condition "
                                      "information will be created for each specified data type. "
                                      "Each row of a file will contain information about a "
                                      "participant and each column represents a test condition. "
                                      "If no test conditions are available aggregation over all "
                                      "available trials is performed."));
    textLabel->setWordWrap(true);

    QLabel* dataLabel = new QLabel(tr("<b>Data Types:</b>"));
    dataLabel->setWordWrap(true);
    m_exportData = QStringList({"#Raw Data Samples in Trials",
                                "Duration of Trials [s]",
                                "#Fixations in Trials",
                                "#Fixations with Microsaccades in Trials",
                                "Fixations containing Microsaccades [%]",
                                "Fixation Duration [s]",
                                "Fixation (with Microsaccades) Duration [s]",
                                "#Microsaccades in Trials",
                                "#Microsaccades per Fixation",
                                "#Microsaccades per Fixation with Microsaccades",
                                "#Microsaccades per Second (for Trials)",
                                "#Microsaccades per Second in Fixations (for Trials)",
                                "Microsaccade Amplitude [°]",
                                "Inter-saccadic Interval [ms]",
                                "Microsaccade Duration [ms]",
                                "Microsaccade Peak Velocity [°/s]"});

    m_sumNotAvailable = {4, 10, 11, 12, 13, 14, 15};

    QLabel* exportValueTypesLabel = new QLabel(tr("<b>Values:</b>"));
    m_exportValueTypes = QStringList({"min", "max", "mean", "median", "sum", "single values"});

    // accept button and cancel button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

    QPushButton* button1 = new QPushButton(tr("Export"));

    buttonBox->addButton(button1, QDialogButtonBox::AcceptRole);

    // connection of the buttons to close the window and start the creation of the tree
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, myDialog, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, myDialog, &QDialog::accept);

    QObject::connect(button1, &QPushButton::clicked,
                     this, &StatisticsExport::exportAggregatedData);

    auto exportValueTypesLayout = new QVBoxLayout;

    exportValueTypesLayout->addWidget(exportValueTypesLabel);
    foreach (QString type, m_exportValueTypes)
    {
        auto checkbox = new QRadioButton(type);
        m_exportValueTypesCheckBoxes.push_back(checkbox);

        exportValueTypesLayout->addWidget(checkbox);
    }
    m_exportValueTypesCheckBoxes[2]->setChecked(true);

    auto exportValueTypesGroupBox = new QGroupBox(m_parent);
    exportValueTypesGroupBox->setLayout(exportValueTypesLayout);

    auto exportDataLayout = new QVBoxLayout;

    exportDataLayout->addWidget(dataLabel);
    foreach (QString dataType, m_exportData)
    {
        auto checkbox = new QCheckBox(dataType);
        m_exportDataCheckBoxes.push_back(checkbox);

        exportDataLayout->addWidget(checkbox);
        checkbox->setChecked(true);
    }

    auto exportDataGroupBox = new QGroupBox(m_parent);
    exportDataGroupBox->setLayout(exportDataLayout);

    // layout
    auto layout = new QVBoxLayout;

    layout->addWidget(textLabel);
    layout->addWidget(exportDataGroupBox);
    layout->addWidget(exportValueTypesGroupBox);

    layout->addSpacing(20);
    layout->addWidget(buttonBox);

    myDialog->setLayout(layout);

    myDialog->exec();
}


void StatisticsExport::exportDirectionalDataToFile()
{
    exportDirectionalData = true;

    m_exportDataCheckBoxes.clear();
    m_exportValueTypesCheckBoxes.clear();

    QDialog* myDialog = new QDialog(m_parent);
    myDialog->setWindowTitle(tr("Specify data for aggregated export"));

    QLabel* textLabel = new QLabel(tr("Files with aggregated participant-test condition "
                                      "information will be created for each specified data type. "
                                      "Each row of a file will contain information about a "
                                      "participant and each column represents a test condition. "
                                      "If no test conditions are available aggregation over all "
                                      "available trials is performed. Note, the calculation of sum "
                                      "does not make sense for all values and might not be exported,"));
    textLabel->setWordWrap(true);

    QLabel* dataLabel = new QLabel(tr("<b>Data Types:</b>"));
    dataLabel->setWordWrap(true);
    m_exportData = QStringList({"Direction from Screen Coordinate System",
                                "Direction towards Next Fixation",
                                "Direction towards Previous Fixation"});

    m_sumNotAvailable = {0, 1, 2};

    QLabel* exportValueTypesLabel = new QLabel(tr("<b>Values:</b>"));
    m_exportValueTypes = QStringList({"mean", "single values"});

    // accept button and cancel button
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

    QPushButton* button1 = new QPushButton(tr("Export"));

    buttonBox->addButton(button1, QDialogButtonBox::AcceptRole);

    // connection of the buttons to close the window and start the creation of the tree
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, myDialog, &QDialog::reject);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, myDialog, &QDialog::accept);

    QObject::connect(button1, &QPushButton::clicked,
                     this, &StatisticsExport::exportAggregatedData);

    auto exportValueTypesLayout = new QVBoxLayout;

    exportValueTypesLayout->addWidget(exportValueTypesLabel);
    foreach (QString type, m_exportValueTypes)
    {
        auto checkbox = new QRadioButton(type);
        m_exportValueTypesCheckBoxes.push_back(checkbox);

        exportValueTypesLayout->addWidget(checkbox);
    }
    m_exportValueTypesCheckBoxes[0]->setChecked(true);

    auto exportValueTypesGroupBox = new QGroupBox(m_parent);
    exportValueTypesGroupBox->setLayout(exportValueTypesLayout);

    auto exportDataLayout = new QVBoxLayout;

    exportDataLayout->addWidget(dataLabel);
    foreach (QString dataType, m_exportData)
    {
        auto checkbox = new QCheckBox(dataType);
        m_exportDataCheckBoxes.push_back(checkbox);

        exportDataLayout->addWidget(checkbox);
        checkbox->setChecked(true);
    }

    auto exportDataGroupBox = new QGroupBox(m_parent);
    exportDataGroupBox->setLayout(exportDataLayout);

    // layout
    auto layout = new QVBoxLayout;

    layout->addWidget(textLabel);
    layout->addWidget(exportDataGroupBox);
    layout->addWidget(exportValueTypesGroupBox);

    layout->addSpacing(20);
    layout->addWidget(buttonBox);

    myDialog->setLayout(layout);

    myDialog->exec();
}


void StatisticsExport::exportAggregatedData()
{

    QString dir = QFileDialog::getExistingDirectory(m_parent, tr("Export Directory"),
                                                    m_dirPath,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
    {
        return;
    }

    m_dirPath = dir;

    int checkedValueType = -1;
    foreach (QRadioButton* p, m_exportValueTypesCheckBoxes)
    {
        checkedValueType++;
        if (p->isChecked())
        {
            break;
        }
    }

    // create sub directory
    QString dirName = subDirPath(dir);
    QDir().mkdir(dirName);

    //export for each participants and each test condition...; if no test condition: for each part...
    for (size_t i = 0; i < m_exportDataCheckBoxes.size(); ++i)
    {
        // skip sum for certain elements
        if (checkedValueType == 4 && std::find(m_sumNotAvailable.begin(), m_sumNotAvailable.end(), i) != m_sumNotAvailable.end())
        {
            continue;
        }
        if (m_exportDataCheckBoxes[i]->isChecked())
        {
            QString fileName = getFileName(int(i), dirName);

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly))
            {
                QTextStream out(&file);

                for (size_t j = 0; j < m_eyeTrackingData->numberOfParticipants(); ++j)
                {
                    Participant* participant = m_eyeTrackingData->participantAt(int(j));

                    QMapIterator<QString, QColor> it(m_settings->m_testConditions);
                    while (it.hasNext())
                    {
                        it.next();
                        QString testCondition = it.key();
                        int dataCount;
                        std::vector<qreal> dataSamples = getDataSamples(participant, testCondition, int(i), dataCount);
                        QString value = getValue(dataSamples, checkedValueType, dataCount);

                        out << value;
                        if (it.hasNext())
                        {
                            out << ",";
                        }
                    }

                    if (m_settings->m_testConditions.empty())
                    {
                        QString testCondition = "";
                        int dataCount;
                        std::vector<qreal> dataSamples = getDataSamples(participant, testCondition, int(i), dataCount);
                        QString value = getValue(dataSamples, checkedValueType, dataCount);

                        out << value;
                    }

                    out << endl;
                }

                file.close();
            }
        }
    }
}


QString StatisticsExport::subDirPath(const QString path)
{
    QString dirName = "VisME_Statistics";

    QString subPath = QDir::cleanPath(path + QDir::separator() + dirName);
    int count = 1;
    while (QDir(subPath).exists())
    {
        subPath = QDir::cleanPath(path + QDir::separator() + dirName + "_" + QString::number(count));
        count++;
    }
    return subPath;
}


QString StatisticsExport::getFileName(int dataType, const QString dir)
{
    QStringList fileNames;

    if (!exportDirectionalData)
    {
        fileNames = QStringList({
            "NumberRawDataSamplesInTrials.csv",
            "DurationOfTrials.csv",
            "NumberFixationsInTrials.csv",
            "NumberFixationsWithMicrosaccadesInTrial.csv",
            "PercentageFixationsContainingMicrosaccades.csv",
            "FixationDuration.csv",
            "FixationWithMicrosaccadesDuration.csv",
            "NumberMicrosaccadeInTrials.csv",
            "NumberMicrosaccadesPerFixation.csv",
            "NumberMicrosaccadesPerFixationWithMicrosaccades.csv",
            "NumberMicrosaccadesPerSecond.csv",
            "NumberMicrosaccadesPerSecondinFixations.csv",
            "MicrosaccadeAmplitude.csv",
            "IntersaccadicInterval.csv",
            "MicrosaccadeDuration.csv",
            "MicrosaccadePeakVelocity.csv"
        });
    }
    else
    {
        fileNames = QStringList({
            "DirectionFromView.csv",
            "DirectionTowardsNextFixation.csv",
            "DirectionTowardsPreviousFixation.csv"
        });
    }

    return QDir::cleanPath(dir + QDir::separator() + fileNames[dataType]);
}


std::vector<qreal> StatisticsExport::getDataSamples(Participant* p, const QString testCondition,
                                                    int dataType, int& dataCount)
{
    std::vector<qreal> values;
    dataCount = -1;

    for (size_t i = 0; i < p->numberOfTrials(); ++i)
    {
        Trial* trial = p->getTrialAt(int(i));
        if (trial->getTestConditionType() == testCondition)
        {
            const QMap<int, GazeData>& g = trial->gazeData();

            if (!exportDirectionalData)
            {
                switch(dataType)
                {
                    case 0: // "#Raw Data Samples in Trials"
                    {
                        int gazeDataSamples = 0;
                        if (!g.empty())
                        {
                            QMapIterator<int, GazeData> it(g);
                            while(it.hasNext())
                            {
                                it.next();
                                gazeDataSamples++;
                            }
                        }
                        values.push_back(gazeDataSamples);
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 1: // "Duration of Trials [s]"
                    {
                        int trialDuration = trial->duration();

                        values.push_back(trial->samplesToTime(trialDuration) / 1000.0);
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 2: // "#Fixations in Trials"
                    {
                        values.push_back(trial->fixationsCount());
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 3: // "#Fixations with Microsaccades in Trials"
                    {
                        int fixWithMS = 0;
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            if (msCount > 0)
                            {
                                fixWithMS++;
                            }
                        }
                        values.push_back(fixWithMS);
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 4: // "Fixations containing Microsaccades [%]"
                    {
                        int fixWithMS = 0;
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            if (msCount > 0)
                            {
                                fixWithMS++;
                            }
                        }

                        values.push_back(qreal(fixWithMS) / qreal(trial->fixationsCount()) * 100);
                        dataCount = int(values.size());
                        break;
                    }
                    case 5: // "Fixation Duration [s]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            values.push_back(trial->samplesToTime(fixation->getDuration()) / 1000.0);
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 6: // "Fixation (with Microsaccades) Duration [s]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            if (msCount > 0)
                            {
                                values.push_back(trial->samplesToTime(fixation->getDuration()) / 1000.0);
                            }
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 7: // "#Microsaccades in Trials"
                    {
                        int msCountSum = 0;
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            msCountSum += msCount;
                        }

                        values.push_back(msCountSum);
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 8: // "#Microsaccades per Fixation"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            values.push_back(msCount);
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 9: // "#Microsaccades per Fixation with Microsaccades"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));

                            if (msCount > 0)
                            {
                                values.push_back(msCount);
                            }
                        }

                        dataCount = int(values.size());
                        break;
                    }
                    case 10: // "#Microsaccades per Second (for Trials)"
                    {
                        int msCountSum = 0;
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            msCountSum += msCount;
                        }

                        int trialDuration = trial->duration();

                        values.push_back(msCountSum / trial->samplesToTime(trialDuration) * 1000.0);
                        dataCount = int(p->numberOfTrials());
                        break;
                    }
                    case 11: // "#Microsaccades per Second in Fixations (for Trials)"
                    {
                        int duration = 0;
                        int msCountSum = 0;
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            duration += fixation->getDuration();

                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            msCountSum += msCount;
                        }

                        values.push_back(duration <= 0 ? 0 : msCountSum / trial->samplesToTime(duration) * 1000.0);

                        dataCount = int(values.size());
                        break;
                    }
                    case 12: // "Microsaccade Amplitude [°]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            size_t msCount = fixation->getMicrosaccadesCount(fromInput);
                            for (size_t k = 0; k < msCount; ++k)
                            {
                                Saccade* ms = fixation->getMicrosaccadeAt(k, fromInput);
                                values.push_back(ms->getAmplitude());
                            }
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 13: // "Inter-saccadic Interval [ms]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            size_t msCount = fixation->getMicrosaccadesCount(fromInput);
                            Saccade* prevMS = nullptr;
                            for (size_t k = 0; k < msCount; ++k)
                            {
                                Saccade* ms = fixation->getMicrosaccadeAt(k, fromInput);
                                if (k > 0)
                                {
                                    values.push_back(trial->samplesToTime(int(ms->getOnsetIndex() - prevMS->getEndIndex())));
                                }
                                prevMS = ms;
                            }
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 14: // "Microsaccade Duration [ms]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            size_t msCount = fixation->getMicrosaccadesCount(fromInput);
                            for (size_t k = 0; k < msCount; ++k)
                            {
                                Saccade* ms = fixation->getMicrosaccadeAt(k, fromInput);
                                values.push_back(trial->samplesToTime(int(ms->getDuration())));
                            }
                        }
                        dataCount = int(values.size());
                        break;
                    }
                    case 15: // "Microsaccade Peak Velocity [°/s]"
                    {
                        for (int j = 0; j < trial->fixationsCount(); ++j)
                        {
                            Fixation* fixation = trial->getFixationAt(j);
                            bool fromInput = m_settings->m_microsaccadeSettings->m_fromInputFile;
                            auto msCount = int(fixation->getMicrosaccadesCount(fromInput));
                            for (int k = 0; k < msCount; ++k)
                            {
                                Saccade* ms = fixation->getMicrosaccadeAt(size_t(k), fromInput);
                                values.push_back(ms->getPeakVelocity());
                            }
                        }
                        dataCount = int(values.size());
                        break;
                    }
                }
            }
            else
            {
                PlotDirection direction = PDir_FromView;
                switch(dataType)
                {
                    case 0: // DirectionFromView",
                    {
                        direction = PDir_FromView;
                        break;
                    }
                    case 1: // "DirectionTowardsNextFixation",
                    {
                        direction = PDir_ToNextFixation;
                        break;
                    }
                    case 2: // "DirectionTowardsPreviousFixation",
                    {
                        direction = PDir_ToPreviousFixation;
                        break;
                    }
                }

                std::vector<qreal> anglesForTrial;
                auto dirCalc = new DirectionCalculations();
                dirCalc->getAngularDataForTrial(trial, direction,
                                                PD_Microsaccades, anglesForTrial,
                                                trial->fixationPointers());

                if (!anglesForTrial.empty())
                {
                    values.insert(values.end(), anglesForTrial.begin(), anglesForTrial.end());
                }
            }
        }
    }

    return values;
}


QString StatisticsExport::getValue(std::vector<qreal> dataSamples, int valueType, int dataCount)
{
    QString value = "";

    if (!exportDirectionalData)
    {
        qreal sum = std::accumulate(dataSamples.begin(), dataSamples.end(), 0.0);
        qreal elementCount;

        if (dataCount != -1)
        {
            elementCount = dataSamples.size();
        }
        else
        {
            elementCount = dataCount;//currentTrials.size();
        }


        if (valueType == 0) // min
        {
            value = QString::number(*std::min_element(dataSamples.begin(), dataSamples.end()));
        }
        else if (valueType == 1) // max
        {
            value = QString::number(*std::max_element(dataSamples.begin(), dataSamples.end()));
        }
        else if (valueType == 2) // mean
        {
            if (elementCount > 0)
            {
                value = QString::number(sum / elementCount);
            }
        }
        else if (valueType == 3) // median
        {
            value = QString::number(MathHelpFunctions::median<qreal>(dataSamples));
        }
        else if (valueType == 4) // sum
        {
            value = QString::number(sum);
        }
        else if (valueType == 5) // single values
        {
            value = "[";
            for (size_t i = 0; i < dataSamples.size(); ++i)
            {
                value += QString::number(dataSamples[i]);
                if (i != dataSamples.size() - 1)
                {
                    value += ",";
                }
            }
            value += "]";
        }
    }
    else
    {
        if (valueType == 0) // mean
        {
            value = QString::number(MathHelpFunctions::angularMean(dataSamples));
        }
        else if (valueType == 1) // single values
        {
            value = "[";
            for (size_t i = 0; i < dataSamples.size(); ++i)
            {
                value += QString::number(dataSamples[i]);
                if (i != dataSamples.size() - 1)
                {
                    value += ",";
                }
            }
            value += "]";
        }
    }

    return value;
}
