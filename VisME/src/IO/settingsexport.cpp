/***********************************************************************************************//**
 * @author Tanja Munz
 * @file settingsexport.cpp
 **************************************************************************************************/
#include "fileextensions.h"
#include "settingsexport.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>


SettingsExport::SettingsExport(QWidget* parent, Settings* settings) :
    m_parent(parent),
    m_settings(settings)
{}

void SettingsExport::exportMicroseccadesSettingsToFile()
{
    exportMicroseccadesSettingsToGivenFile("");
}

void SettingsExport::exportMicroseccadesSettingsToGivenFile(QString fileName)
{
    std::ofstream out;
    exportFile(out,
               tr("Save Microsaccades Parameter"),
               tr("%2 (*.%1);;All Files (*)").arg(MicrosaccadesParameterFile,
                                                  MicrosaccadesParameterFileDescription),
               fileName);

    qApp->setOverrideCursor(Qt::WaitCursor);

    if (out.is_open())
    {
        out << "UseMicrosaccadesFromInput=" << m_settings->m_microsaccadeSettings->m_fromInputFile << std::endl;
        out << "RelativeVelocityThreshold=" << m_settings->m_microsaccadeSettings->m_velocityThreshold << std::endl;
        out << "MinDuration=" << m_settings->m_microsaccadeSettings->m_minDuration << std::endl;
        out << "Binocular=" << m_settings->m_microsaccadeSettings->m_binocular << std::endl;
        out << "MaxDuration=" << m_settings->m_microsaccadeSettings->m_maxDuration << std::endl;
        out << "MaxAmplitude=" << m_settings->m_microsaccadeSettings->m_maxAmplitude << std::endl;
        out << "MinAmplitude=" << m_settings->m_microsaccadeSettings->m_minAmplitude << std::endl;
        out << "MinSaccadicInterval=" << m_settings->m_microsaccadeSettings->m_minIntersaccInterval << std::endl;
        out << "MinVelocity=" << m_settings->m_microsaccadeSettings->m_minVelocity << std::endl;
        out << "MaxVelocity=" << m_settings->m_microsaccadeSettings->m_maxVelocity << std::endl;
        out << "IgnoreStartFixationTime=" << m_settings->m_microsaccadeSettings->m_ignoreAtStart << std::endl;
        out << "IgnoreEndFixationTime=" << m_settings->m_microsaccadeSettings->m_ignoreAtEnd << std::endl;
        out << "VelocityWindowSize=" << m_settings->m_microsaccadeSettings->m_velocityWindowSize << std::endl;
        out << "IgnoreBeforeMissingData=" << m_settings->m_microsaccadeSettings->m_ignoreBeforeMissingData << std::endl;
        out << "IgnoreAfterMissingData=" << m_settings->m_microsaccadeSettings->m_ignoreAfterMissingData << std::endl;
        out << "UseMaxDuration=" << m_settings->m_microsaccadeSettings->m_useMaxDuration << std::endl;
        out << "UseMinAmplitude=" << m_settings->m_microsaccadeSettings->m_useMinAmplitude << std::endl;
        out << "UseMaxAmplitude=" << m_settings->m_microsaccadeSettings->m_useMaxAmplitude << std::endl;
        out << "UseMinIntersaccInterval=" << m_settings->m_microsaccadeSettings->m_useMinIntersaccInterval << std::endl;
        out << "UseMinVelocity=" << m_settings->m_microsaccadeSettings->m_useMinVelocity << std::endl;
        out << "UseMaxVelocity=" << m_settings->m_microsaccadeSettings->m_useMaxVelocity << std::endl;
        out << "UseIgnoreStartFixationTime=" << m_settings->m_microsaccadeSettings->m_useIgnoreAtStart << std::endl;
        out << "UseIgnoreEndFixationTime=" << m_settings->m_microsaccadeSettings->m_useIgnoreAtEnd << std::endl;
        out << "UseIgnoreBeforeMissingData=" << m_settings->m_microsaccadeSettings->m_useIgnoreBeforeMissingData << std::endl;
        out << "UseIgnoreAfterMissingData=" << m_settings->m_microsaccadeSettings->m_useIgnoreAfterMissingData << std::endl;

        out.close();
    }

    qApp->restoreOverrideCursor();
}


void SettingsExport::exportFixationSettingsToFile()
{
    std::ofstream out;
    exportFile(out,
               tr("Save Fixation Filter Parameter"),
               tr("%2 (*.%1);;All Files (*)").arg(FixationParameterFile, FixationParameterFileDescription));

    qApp->setOverrideCursor(Qt::WaitCursor);

    if (out.is_open())
    {
        out << "UseMicrosaccadesFromInput=" << m_settings->m_fixationSettings->m_fromInputFile << std::endl;
        out << "RelativeVelocityThreshold=" << m_settings->m_fixationSettings->m_velocityThreshold << std::endl;
        out << "MinDuration=" << m_settings->m_fixationSettings->m_minDuration << std::endl;
        out << "Binocular=" << m_settings->m_fixationSettings->m_binocular << std::endl;
        out << "MaxDuration=" << m_settings->m_fixationSettings->m_maxDuration << std::endl;
        out << "MaxAmplitude=" << m_settings->m_fixationSettings->m_maxAmplitude << std::endl;
        out << "MinAmplitude=" << m_settings->m_fixationSettings->m_minAmplitude << std::endl;
        out << "MinSaccadicInterval=" << m_settings->m_fixationSettings->m_minIntersaccInterval << std::endl;
        out << "MinVelocity=" << m_settings->m_fixationSettings->m_minVelocity << std::endl;
        out << "MaxVelocity=" << m_settings->m_fixationSettings->m_maxVelocity << std::endl;
        out << "IgnoreStartFixationTime=" << m_settings->m_fixationSettings->m_ignoreAtStart << std::endl;
        out << "IgnoreEndFixationTime=" << m_settings->m_fixationSettings->m_ignoreAtEnd << std::endl;
        out << "VelocityWindowSize=" << m_settings->m_fixationSettings->m_velocityWindowSize << std::endl;
        out << "IgnoreBeforeMissingData=" << m_settings->m_fixationSettings->m_ignoreBeforeMissingData << std::endl;
        out << "IgnoreAfterMissingData=" << m_settings->m_fixationSettings->m_ignoreAfterMissingData << std::endl;
        out << "UseMaxDuration=" << m_settings->m_fixationSettings->m_useMaxDuration << std::endl;
        out << "UseMinAmplitude=" << m_settings->m_fixationSettings->m_useMinAmplitude << std::endl;
        out << "UseMaxAmplitude=" << m_settings->m_fixationSettings->m_useMaxAmplitude << std::endl;
        out << "UseMinIntersaccInterval=" << m_settings->m_fixationSettings->m_useMinIntersaccInterval << std::endl;
        out << "UseMinVelocity=" << m_settings->m_fixationSettings->m_useMinVelocity << std::endl;
        out << "UseMaxVelocity=" << m_settings->m_fixationSettings->m_useMaxVelocity << std::endl;
        out << "UseIgnoreStartFixationTime=" << m_settings->m_fixationSettings->m_useIgnoreAtStart << std::endl;
        out << "UseIgnoreEndFixationTime=" << m_settings->m_fixationSettings->m_useIgnoreAtEnd << std::endl;
        out << "UseIgnoreBeforeMissingData=" << m_settings->m_fixationSettings->m_useIgnoreBeforeMissingData << std::endl;
        out << "UseIgnoreAfterMissingData=" << m_settings->m_fixationSettings->m_useIgnoreAfterMissingData << std::endl;

        out.close();
    }

    qApp->restoreOverrideCursor();
}


void SettingsExport::exportColorSettingsToFile()
{
    std::ofstream out;
    exportFile(out,
               tr("Save Color Settings"),
               tr("%2 (*.%1);;All Files (*)").arg(ColorSettingsFile, ColorSettingsFileDescription));

    qApp->setOverrideCursor(Qt::WaitCursor);

    if (out.is_open())
    {
        out << "sampleConnectionColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.sampleConnectionColor)
            << std::endl;
        out << "sampleColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.sampleColor)
            << std::endl;
        out << "fixationSamplesColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.fixationSamplesColor)
            << std::endl;
        out << "fixationColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.fixationColor)
            << std::endl;
        out << "fixationHighlightColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.fixationHighlightColor)
            << std::endl;
        out << "saccadeColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.saccadeColor)
            << std::endl;
        out << "saccadesDirectionStartColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.saccadesDirectionStartColor)
            << std::endl;
        out << "microsaccadesColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.microsaccadesColor)
            << std::endl;
        out << "microsaccadesDirectionEndColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.microsaccadesDirectionEndColor)
            << std::endl;
        out << "missingDataColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.missingDataColor)
            << std::endl;
        out << "yValueColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.yValueColor)
            << std::endl;
        out << "xValueColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.xValueColor)
            << std::endl;
        out << "velocityColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.velocityColor)
            << std::endl;
        out << "dataPlotColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.dataPlotColor)
            << std::endl;
        out << "dataPlotBaseColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.dataPlotBaseColor)
            << std::endl;
        out << "sceneBorderColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.sceneBorderColor)
            << std::endl;
        out << "meanValueColor="
            << m_settings->m_colors.getColorString(m_settings->m_colors.meanValueColor)
            << std::endl;

        out.close();
    }

    qApp->restoreOverrideCursor();
}


void SettingsExport::exportFile(std::ofstream& out, const QString& reason, const QString& fileTypes, QString fileName) const
{
    if (fileName.isEmpty())
    {
        QString fileSuggestion;
        if (!m_settings->m_currentTrials.empty())
        {
            fileSuggestion = m_settings->m_currentTrials[0]->getParticipant()->modifiedName() + "_" +
                             m_settings->m_currentTrials[0]->modifiedName();
        }

        fileName= QFileDialog::getSaveFileName(m_parent, reason,
        QDir::cleanPath(m_settings->m_recentDir + QDir::separator() + fileSuggestion), fileTypes);
    }

    if (!fileName.isEmpty())
    {
        // write to file
        out = std::ofstream(fileName.toStdString(), std::ios::out);

        QFile file(fileName);
        m_settings->m_recentDir = QFileInfo(file).path();
    }
}
