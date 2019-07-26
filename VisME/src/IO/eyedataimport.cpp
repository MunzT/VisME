/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedataimport.cpp
 **************************************************************************************************/
#include "event.h"
#include "eyedataimport.h"
#include "eyetrackingdata.h"
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QTextStream>


EyeDataImport::EyeDataImport(QWidget* window, EyeTrackingData* data, Settings* settings) :
    m_parent(window),
    m_data(data),
    m_settings(settings)
{}


void EyeDataImport::openFiles(const QStringList& fileNames, bool add)
{
    QElapsedTimer timer;
    timer.start();

    if (fileNames.size() < 1)
    {
        return ;
    }

    // show wait cursor
    qApp->processEvents();
    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();

    if (!add)
    {
        m_settings->m_currentTrials.clear();
        m_data->clearAllParticipants();
    }

    QString returnPath = "";

    foreach (QString filePath, fileNames)
    {
        if (filePath.isEmpty() || fileAlreadyLoaded(filePath))
        {
            continue;
        }

        returnPath = QFileInfo(filePath).path();

        bool success = true;

        // open the file
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(m_parent, tr("Error"),
                                 tr("File %1 can not be opened (1).\n %2").arg(filePath,
                                                                               file.errorString()));
            continue;
        }

        // read the file
        QTextStream in(&file);

        // UTF-8 codec for the input file expected
        in.setCodec("UTF-8");

        // read first line
        QString line;

        std::vector<Fixation> fixationsLeft;
        std::vector<Fixation> fixationsRight;
        std::vector<Fixation> fixationsAverage;
        std::vector<Saccade> microsaccadesLeft;
        std::vector<Saccade> microsaccadesRight;
        std::vector<Saccade> microsaccadesAverage;
        QMap<int, GazeData> leftGazeData;
        QMap<int, GazeData> rightGazeData;
        QMap<int, GazeData> averageGazeData;
        std::vector<Event> events;

        qreal minX = 0;
        qreal minY = 0;
        qreal maxX = 0;
        qreal maxY = 0;

        QString imagePath = QString();
        qreal frequencyRate = 0;

        Participant* currentParticipant = new Participant(filePath);
        Trial currentTrial = Trial(currentParticipant, m_settings);

        QString participantName;
        QString trialID;
        qreal pixelsPerDegree = 0;

        // our own input file format
        /*
        PARTICIPANT <participantName>
        PIXELSPERDEGREE <pixelsPerDegree>

        TRIAL <trialName>
        COORDS <xMin> <yMin> <xMax> <yMax>
        FREQ <frequency>
        STIMULUS <filePath>

        # gaze points
        [R|L|B] <index> <xPos> <yPos>
        ...

        # fixations
        F [R|L|B] <startIndex> <durationIds> <xPos> <yPos>
        ...

        # microsaccades
        M [R|L|B] <startIndex> <durationIds> <peakVelocity(deg/s)> <horizontalComp(deg)> <verticalComp(deg)> <horizontalAmpl(deg)> <verticalAmpl(deg)>
        ...

        # events
        E <startIndex> <durationIds> <name/description)>
        ...

        ENDTRIAL
        ...
        */

        line = in.readLine();

        do
        {
            if (!line.isNull())
            {
                line = line.trimmed();
            }
            if (!(line.isNull() || line.isEmpty()))
            {
                QStringList elementParts = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

                if (elementParts.count() > 0)
                {
                    // general eye-tracking data
                    if (elementParts[0] == "COORDS" && elementParts.count() >= 5)
                    {
                        minX = elementParts[1].toDouble();
                        minY = elementParts[2].toDouble();
                        maxX = elementParts[3].toDouble();
                        maxY = elementParts[4].toDouble();
                    }

                    else if (elementParts[0] == "FREQ" && elementParts.count() >= 2)
                    {
                        frequencyRate = elementParts[1].toDouble();
                    }

                    else if (elementParts[0] == "PARTICIPANT" && elementParts.count() >= 2)
                    {
                        participantName = elementParts[1];
                        currentParticipant->setName(participantName);
                    }

                    else if (elementParts[0] == "PIXELSPERDEGREE" && elementParts.count() >= 2)
                    {
                        pixelsPerDegree = elementParts[1].toDouble();
                        currentParticipant->setPixelsPerDegree(pixelsPerDegree);
                    }

                    // trial data
                    else if (elementParts[0] == "TRIAL" && elementParts.count() >= 2)
                    {
                        trialID = elementParts[1];
                    }

                    else if (elementParts[0] == "STIMULUS" && elementParts.count() >= 2)
                    {
                        imagePath = elementParts[1];
                    }

                    // fixations
                    else if (elementParts[0] == "F" && elementParts.count() >= 6)
                    {
                        Fixation fixation = Fixation(m_settings);
                        fixation.setStartIndex(elementParts[2].toInt());
                        fixation.setDuration(elementParts[3].toInt());
                        fixation.setXPosition(elementParts[4].toDouble());
                        fixation.setYPosition(elementParts[5].toDouble());

                        if (elementParts[1] == "R")
                        {
                            fixationsRight.push_back(fixation);
                        }
                        else if (elementParts[1] == "L")
                        {
                            fixationsLeft.push_back(fixation);
                        }
                        else if (elementParts[1] == "B")
                        {
                            fixationsAverage.push_back(fixation);
                        }
                    }
                    else if (elementParts[0] == "E" && elementParts.count() >= 4)
                    {

                        events.push_back(Event(elementParts[1].toInt(),
                                               elementParts[2].toInt(),
                                               elementParts.mid(3).join(" ")));
                    }

                    // Microsaccades
                    else if (elementParts[0] == "M" && elementParts.count() >= 9)
                    {
                        Saccade microsaccade(
                                    elementParts[2].toInt(),
                                    elementParts[3].toInt() + elementParts[2].toInt() - 1, // duration -> end
                                    elementParts[4].toDouble(),
                                    elementParts[5].toDouble(),
                                    elementParts[6].toDouble(),
                                    elementParts[7].toDouble(),
                                    elementParts[8].toDouble());

                        if (elementParts[1] == "R")
                        {
                            microsaccadesRight.push_back(microsaccade);
                        }
                        else if (elementParts[1] == "L")
                        {
                            microsaccadesLeft.push_back(microsaccade);
                        }
                        else if (elementParts[1] == "B")
                        {
                            microsaccadesAverage.push_back(microsaccade);
                        }
                    }

                    // gaze samples
                    else if ((elementParts[0] == "L" || elementParts[0] == "R" || elementParts[0] == "B") &&
                             elementParts.count() >= 4)
                    {
                        GazeData gaze;
                        gaze.setXPosition(elementParts[2].toDouble());
                        gaze.setYPosition(elementParts[3].toDouble());

                        if (elementParts[0] == "R")
                        {
                            rightGazeData.insert(elementParts[1].toDouble(), gaze);
                        }
                        else if (elementParts[0] == "L")
                        {
                            leftGazeData.insert(elementParts[1].toDouble(), gaze);
                        }
                        else if (elementParts[0] == "B")
                        {
                            averageGazeData.insert(elementParts[1].toDouble(), gaze);
                        }
                    }
                    else if (elementParts[0] == "ENDTRIAL")
                    {
                        currentTrial.setName(trialID);
                        currentTrial.setGazeData(leftGazeData, GazeType::GT_Left);
                        currentTrial.setGazeData(rightGazeData, GazeType::GT_Right);
                        currentTrial.setGazeData(averageGazeData, GazeType::GT_Average);
                        currentTrial.setFixations(fixationsLeft, GazeType::GT_Left);
                        currentTrial.setFixations(fixationsRight, GazeType::GT_Right);
                        currentTrial.setFixations(fixationsAverage, GazeType::GT_Average);
                        currentTrial.setMicrosaccadesForFixations(&microsaccadesLeft, GazeType::GT_Left);
                        currentTrial.setMicrosaccadesForFixations(&microsaccadesRight, GazeType::GT_Right);
                        currentTrial.setMicrosaccadesForFixations(&microsaccadesAverage, GazeType::GT_Average);
                        currentTrial.setStimulus(QDir(QFileInfo(QFile(currentParticipant->filePath()).fileName()).absoluteDir()).filePath(imagePath));
                        currentTrial.setEvents(events);

                        currentTrial.setFrequencyRate(frequencyRate);
                        currentTrial.setMinMax(minX, maxX, minY, maxY);

                        currentParticipant->addTrial(currentTrial);
                        currentTrial = Trial(currentParticipant, m_settings);

                        microsaccadesRight.clear();
                        microsaccadesLeft.clear();
                        microsaccadesAverage.clear();
                        leftGazeData.clear();
                        rightGazeData.clear();
                        averageGazeData.clear();
                        fixationsRight.clear();
                        fixationsLeft.clear();
                        fixationsAverage.clear();
                        events.clear();
                    }
                }
            }

            // read next line
            line = in.readLine();

        } while (!line.isNull());

        if (pixelsPerDegree == 0)
        {
            success = false; // value was missing
        }

        if (!success)
        {
            QMessageBox::warning(m_parent, "Error",
                tr("Loading file failed %1").arg(filePath));
            continue;
        }
        m_data->addParticipant(currentParticipant);

        file.close();
    }

    emit updateParticipantsInUI();

    qApp->restoreOverrideCursor();
    qApp->processEvents();

    m_settings->m_recentDir = returnPath;

    qDebug() << QString("Import files: %1 ms").arg(timer.elapsed());
}


void EyeDataImport::readCSV(const QString& fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(m_parent, tr("Error"),
                             tr("File %1 can not be opened (2).\n %2").arg(fileName, file.errorString()));
        return;
    }

    std::set<QString> testConditions;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.trimmed();
        QStringList words;
        words.append(line.split(","));

        if (words.size() >= 3)
        {
            for (size_t i = 0; i < m_data->numberOfParticipants(); ++i)
            {
                if (m_data->participantAt(int(i))->name() == words[0])
                {
                    for (size_t j = 0; j < m_data->participantAt(int(i))->numberOfTrials(); ++j)
                    {
                        if (m_data->participantAt(int(i))->getTrialAt(int(j))->name() == words[1])
                        {
                            m_data->participantAt(int(i))->getTrialAt(int(j))->setTestConditionType(words[2]);
                            break;
                        }
                    }
                    break;
                }
            }
            testConditions.insert(words[2]);
        }
    }

    emit setTestConditionsInUI(testConditions);

    file.close();

    m_settings->m_recentDir = QFileInfo(file).path();
}


bool EyeDataImport::fileAlreadyLoaded(const QString& filePath)
{
    for (size_t i = 0; i < m_data->numberOfParticipants(); ++i)
    {
        if (m_data->participantAt(int(i))->filePath() == filePath)
        {
            return true;
        }
    }
    return false;
}
