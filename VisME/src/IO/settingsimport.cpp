/***********************************************************************************************//**
 * @author Tanja Munz
 * @file SettingsImport.cpp
 **************************************************************************************************/
#include "settingsimport.h"
#include <QMessageBox>


SettingsImport::SettingsImport(QWidget* window, Settings* settings, UserInterface* ui) :
    m_parent(window),
    m_settings(settings),
    m_ui(ui)
{}


void SettingsImport::readMicrosaccadsSettingsFile(const QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(m_parent, tr("Error"),
                             (tr("File %1 can not be opened (3).\n %2").arg(fileName, file.errorString())));
        return;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.trimmed();

        QStringList elements = line.split("=");

        if (elements.size() == 2)
        {
            if (elements[0] == tr("UseMicrosaccadesFromInput"))
            {
                m_ui->m_microsaccadesFromInputFileCheckbox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("RelativeVelocityThreshold"))
            {
                m_ui->m_msVelocityThresholdSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MinDuration"))
            {
                m_ui->m_msMinDurationSpinBox->setValue(elements[1].toInt());
            }
            if (elements[0] == tr("Binocular"))
            {
                m_ui->m_msBinocularCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("VelocityWindowSize"))
            {
                m_ui->m_msVelocityWindowSizeSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MaxDuration"))
            {
                m_ui->m_msMaxDurationSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MaxAmplitude"))
            {
                m_ui->m_msMaxAmplitudeSpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MinAmplitude"))
            {
                m_ui->m_msMinAmplitudeSpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MinSaccadicInterval"))
            {
                m_ui->m_msMinIntersaccIntervalSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MinVelocity"))
            {
                m_ui->m_msMinVelocitySpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MaxVelocity"))
            {
                m_ui->m_msMaxVelocitySpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("IgnoreStartFixationTime"))
            {
                m_ui->m_msIgnoreAtFixationStartSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreEndFixationTime"))
            {
                m_ui->m_msIgnoreAtFixationEndSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreBeforeMissingData"))
            {
                m_ui->m_msIgnoreBeforeMissingDataSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreAfterMissingData"))
            {
                m_ui->m_msIgnoreAfterMissingDataSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("UseMsMaxDuration"))
            {
                m_ui->m_msMaxDurationCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMsMinAmplitude"))
            {
                m_ui->m_msMinAmplitudeCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMsMaxAmplitude"))
            {
                m_ui->m_msMaxAmplitudeCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMsMinIntersaccInterval"))
            {
                m_ui->m_msMinIntersaccIntervalCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMsMinVelocity"))
            {
                m_ui->m_msMinVelocityCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMsMaxVelocity"))
            {
                m_ui->m_msMaxVelocityCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreStartFixationTime"))
            {
                m_ui->m_msIgnoreAtFixationStartCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreEndFixationTime"))
            {
                m_ui->m_msIgnoreAtFixationEndCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreBeforeMissingData"))
            {
                m_ui->m_msIgnoreBeforeMissingDataCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreAfterMissingData"))
            {
                m_ui->m_msIgnoreAfterMissingDataCheckBox->setChecked(elements[1] == "1");
            }
        }
    }

    m_settings->m_recentDir = QFileInfo(file).path();
}


void SettingsImport::readFixationSettingsFile(const QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(m_parent, tr("Error"),
                             (tr("File %1 can not be opened (3).\n %2").arg(fileName, file.errorString())));
        return;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.trimmed();

        QStringList elements = line.split("=");

        if (elements.size() == 2)
        {
            if (elements[0] == tr("UseFixationsFromInput"))
            {
                m_ui->m_fixationsFromInputFileCheckbox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("RelativeVelocityThreshold"))
            {
                m_ui->m_fixVelocityThresholdSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MinDuration"))
            {
                m_ui->m_fixMinSaccadeDurationSpinBox->setValue(elements[1].toInt());
            }
            if (elements[0] == tr("Binocular"))
            {
                m_ui->m_fixBinocularCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("VelocityWindowSize"))
            {
                m_ui->m_fixVelocityWindowSizeSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MaxDuration"))
            {
                m_ui->m_fixMaxDurationSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MaxAmplitude"))
            {
                m_ui->m_fixMaxSaccadeAmplitudeSpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MinAmplitude"))
            {
                m_ui->m_fixMinSaccadeAmplitudeSpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MinSaccadicInterval"))
            {
                m_ui->m_fixMinSaccadeIntersaccIntervalSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("MinVelocity"))
            {
                m_ui->m_fixMinVelocitySpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("MaxVelocity"))
            {
                m_ui->m_fixMaxVelocitySpinBox->setValue(elements[1].toDouble());
            }
            else if (elements[0] == tr("IgnoreStartFixationTime"))
            {
                m_ui->m_fixIgnoreAtStartSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreEndFixationTime"))
            {
                m_ui->m_fixIgnoreAtEndSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreBeforeMissingData"))
            {
                m_ui->m_fixIgnoreBeforeMissingDataSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("IgnoreAfterMissingData"))
            {
                m_ui->m_fixIgnoreAfterMissingDataSpinBox->setValue(elements[1].toInt());
            }
            else if (elements[0] == tr("UseMaxDuration"))
            {
                m_ui->m_fixMaxDurationCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMinAmplitude"))
            {
                m_ui->m_fixMinAmplitudeCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMaxAmplitude"))
            {
                m_ui->m_fixMaxAmplitudeCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMinIntersaccInterval"))
            {
                m_ui->m_fixMinIntersaccIntervalCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMinVelocity"))
            {
                m_ui->m_fixMinVelocityCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseMaxVelocity"))
            {
                m_ui->m_fixMaxVelocityCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreStartFixationTime"))
            {
                m_ui->m_fixIgnoreAtFixationStartCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreEndFixationTime"))
            {
                m_ui->m_fixIgnoreAtFixationEndCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreBeforeMissingData"))
            {
                m_ui->m_fixIgnoreBeforeMissingDataCheckBox->setChecked(elements[1] == "1");
            }
            else if (elements[0] == tr("UseIgnoreAfterMissingData"))
            {
                m_ui->m_fixIgnoreAfterMissingDataCheckBox->setChecked(elements[1] == "1");
            }
        }
    }

    m_settings->m_recentDir = QFileInfo(file).path();
}


void SettingsImport::readColorSettingsFile(const QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(m_parent, tr("Error"),
                             (tr("File %1 can not be opened (3).\n %2").arg(fileName, file.errorString())));
        return;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.trimmed();

        QStringList elements = line.split("=");

        if (elements.size() == 2)
        {
            if (elements[0] == tr("sampleConnectionColor"))
            {
                m_settings->m_colors.sampleConnectionColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("sampleColor"))
            {
                m_settings->m_colors.sampleColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("fixationSamplesColor"))
            {
                m_settings->m_colors.fixationSamplesColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("fixationColor"))
            {
                m_settings->m_colors.fixationColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("fixationHighlightColor"))
            {
                m_settings->m_colors.fixationHighlightColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("saccadeColor"))
            {
                m_settings->m_colors.saccadeColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("saccadesDirectionStartColor"))
            {
                m_settings->m_colors.saccadesDirectionStartColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("microsaccadesColor"))
            {
                m_settings->m_colors.microsaccadesColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("microsaccadesDirectionEndColor"))
            {
                m_settings->m_colors.microsaccadesDirectionEndColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("missingDataColor"))
            {
                m_settings->m_colors.missingDataColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("yValueColor"))
            {
                m_settings->m_colors.yValueColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("xValueColor"))
            {
                m_settings->m_colors.xValueColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("velocityColor"))
            {
                m_settings->m_colors.velocityColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("dataPlotColor"))
            {
                m_settings->m_colors.dataPlotColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("dataPlotBaseColor"))
            {
                m_settings->m_colors.dataPlotBaseColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("sceneBorderColor"))
            {
                m_settings->m_colors.sceneBorderColor->setNamedColor(elements[1]);
            }
            else if (elements[0] == tr("meanValueColor"))
            {
                m_settings->m_colors.meanValueColor->setNamedColor(elements[1]);
            }
        }
    }

    m_settings->m_recentDir = QFileInfo(file).path();

    // update all colors in dock widget, timeline, ...
    m_ui->updateColorButtons();
}
