/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedataexport.cpp
 **************************************************************************************************/
#include "eyedataexport.h"
#include "fileextensions.h"
#include "settings.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

EyeDataExport::EyeDataExport(QWidget* parent, EyeTrackingData* data, Settings* settings) :
    m_parent(parent),
    m_data(data),
    m_settings(settings)
{}


void EyeDataExport::exportTrialDataToFile()
{
    if (m_settings->m_currentTrials.empty())
    {
        QMessageBox::information(m_parent, tr("No data available"),
                                 tr("No data for export available"));

        return;
    }

    std::ofstream out;
    exportFile(out,
               tr("Save Current Trial"),
               tr("%2 (*.%1);;All Files (*)").arg(eyeTrackingDataFile,
                                                  eyeTrackingDataFileDescription));

    qApp->setOverrideCursor(Qt::WaitCursor);

    if (out.is_open())
    {
        writeHeader(*m_settings->m_currentTrials[0]->getParticipant(), out);
        writeTrial(*m_settings->m_currentTrials[0], out);
        out.close();
    }

    qApp->restoreOverrideCursor();
}


void EyeDataExport::exportCurrentParticipantDataToFile()
{
    if (m_settings->m_currentTrials.empty())
    {
        QMessageBox::information(m_parent, tr("No data available"),
                                 tr("No data for export available"));

        return;
    }

    std::ofstream out;
    exportFile(out,
               tr("Save Current Participant"),
               tr("%2 (*.%1);;All Files (*)").arg(eyeTrackingDataFile,
                                                  eyeTrackingDataFileDescription));

    exportParticipantDataToFile(m_settings->m_currentTrials[0]->getParticipant(), out);
}


void EyeDataExport::exportAllParticipantDataToFile()
{
    // get directory path
    QString dir = QFileDialog::getExistingDirectory(m_parent, tr("Select Directory"),
                                                    m_settings->m_recentDir,
                                                    QFileDialog::ShowDirsOnly |
                                                    QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
    {
        return;
    }

    for (size_t i = 0; i < m_data->numberOfParticipants(); ++i)
    {
        std::ofstream out;
        exportFile(out,
                   tr("Save Current Participant"),
                   eyeTrackingDataFile,
                   m_data->participantAt(int(i)),
                   dir);

        exportParticipantDataToFile(m_data->participantAt(int(i)), out);
    }
}


void EyeDataExport::writeHeader(Participant& participant, std::ofstream& out)
{
    out << "VisME - Visual Microsaccades Explorer" << std::endl;
    out << "PARTICIPANT " << participant.name().toStdString() << std::endl;
    out << "PIXELSPERDEGREE " << QString::number(participant.getPixelsPerDegree()).toStdString() << std::endl;
}


void EyeDataExport::writeTrial(Trial& trial, std::ofstream& out)
{
    if (out.is_open())
    {
        out << "TRIAL " << trial.name().toStdString() << std::endl;
        out << "COORDS " << trial.minX() << " " << trial.minY() << " " << trial.maxX() << " "
           << trial.maxY() << std::endl;
        out << "FREQ " << trial.frequencyRate() << std::endl;
        out << "STIMULUS " << trial.getStimulus().toStdString() << std::endl;


        std::vector<GazeType> gazeTypes = {GazeType::GT_Right, GazeType::GT_Left, GazeType::GT_Average};
        std::vector<std::string> gazeSigns = {"R", "L", "B"};
        for (size_t j = 0; j < gazeTypes.size(); ++j)
        {
            // fixations
            for (int i = 0; i < trial.fixationsCount(gazeTypes[j]); ++i)
            {
                Fixation* f = trial.getFixationAt(i, gazeTypes[j]);

                out << "F " << gazeSigns[j] << " " << QString::number(f->getStartIndex(), 'g', 10).toStdString() << " " <<
                       QString::number(f->getDuration(), 'g', 10).toStdString() << " " <<
                       f->getXPosition() << " " << f->getYPosition() << std::endl;
            }

            // microsaccades
            for (int i = 0; i < trial.fixationsCount(gazeTypes[j]); ++i)
            {
                for (size_t k = 0; k < trial.getFixationAt(i, gazeTypes[j])->getMicrosaccadesCount(trial.msFromInputFile()); ++k)
                {
                    Saccade* m = trial.getFixationAt(i, gazeTypes[j])->getMicrosaccadeAt(k, trial.msFromInputFile());

                    out << "M " << gazeSigns[j] << " " << QString::number(int(m->getOnsetIndex()), 'g', 10).toStdString() << " " <<
                           QString::number(int(m->getDuration()), 'g', 10).toStdString() << " " <<
                           m->getPeakVelocity() << " " <<
                           m->getHorizontalComponent() << " " << m->getVerticalComponent() << " " <<
                           m->getHorizontalAmplitude() << " " << m->getVerticalAmplitude() << std::endl;
                }
            }

            // gaze positions
            QMapIterator<int, GazeData> i(trial.gazeData(gazeTypes[j]));
            while (i.hasNext())
            {
                i.next();
                const GazeData& g = i.value();
                out << gazeSigns[j] << " " << QString::number(i.key(), 'g', 10).toStdString() << " " <<
                       g.getXPosition() << " " << g.getYPosition() << std::endl;
            }

            for (size_t i = 0; i < trial.eventCount(); ++i)
            {
                Event* e = trial.getEventAt(i);

                out << "E " << QString::number(int(e->getStartIndex()), 'g', 10).toStdString() << " " <<
                       QString::number(int(e->getDuration()), 'g', 10).toStdString() << " " <<
                       e->getName().toStdString() << " " << std::endl;
            }
        }

        out << "ENDTRIAL" << std::endl;
        out << std::endl;
    }
}


void EyeDataExport::exportParticipantDataToFile(Participant* participant, std::ofstream& out)
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    if (out.is_open())
    {
        writeHeader(*participant, out);
        for (size_t i = 0; i < participant->numberOfTrials(); ++i)
        {
            writeTrial(*participant->getTrialAt(int(i)), out);
        }

        out.close();
    }

    qApp->restoreOverrideCursor();
}


void EyeDataExport::exportFile(std::ofstream& out,
                               const QString& reason,
                               const QString& fileTypes,
                               Participant* p,
                               const QString dir) const
{
    QString fileSuggestion;
    if (p != nullptr)
    {
        fileSuggestion = p->modifiedName();
    }
    else if (m_settings->m_currentTrials[0] != nullptr)
    {
        fileSuggestion = m_settings->m_currentTrials[0]->getParticipant()->modifiedName();
    }

    QString fileName;
    if (dir == QString())
    {
        fileName= QFileDialog::getSaveFileName(m_parent, reason,
        QDir::cleanPath(m_settings->m_recentDir + QDir::separator() + fileSuggestion), fileTypes);
    }
    else
    {
        fileName = QDir::cleanPath(dir + QDir::separator() + fileSuggestion + "." + fileTypes);
    }

    if (!fileName.isEmpty())
    {
        // write to file
        out = std::ofstream(fileName.toStdString(), std::ios::out);

        QFile file(fileName);
        m_settings->m_recentDir = QFileInfo(file).path();
    }
}
