/***********************************************************************************************//**
 * @author Tanja Munz
 * @file settingsexport.h
 **************************************************************************************************/
#ifndef SETTINGSEXPORT_H
#define SETTINGSEXPORT_H

#include "mainwindow.h"
#include <fstream>
#include <QFileDialog>

/***********************************************************************************************//**
 * This class is responsible for exporting data such as trials, participants, parameters.
 **************************************************************************************************/
class SettingsExport : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    SettingsExport(QWidget* parent, Settings* settings);

    /*******************************************************************************************//**
     * Shows a save file dialog and returns a std::ofstream element.
     *
     * @return reason    reason for the file save dialog
     * @return fileTypes file type of the saved file
     **********************************************************************************************/
    void exportFile(std::ofstream& out,
                    const QString& reason,
                    const QString& fileTypes,
                    QString fileName = "") const;

    /*******************************************************************************************//**
     * Export the current microseccades filter settings from the ui to the given file name.
     **********************************************************************************************/
    void exportMicroseccadesSettingsToGivenFile(QString fileName);

public slots:

    /*******************************************************************************************//**
     * Export the current microseccades filter settings from the ui.
     **********************************************************************************************/
    void exportMicroseccadesSettingsToFile();

    /*******************************************************************************************//**
     * Export the current fixation filter settings from the ui.
     **********************************************************************************************/
    void exportFixationSettingsToFile();

    /*******************************************************************************************//**
     * Export the current color settings.
     **********************************************************************************************/
    void exportColorSettingsToFile();

private:

    /// Main window.
    QWidget* m_parent;

    /// Current settings for visualizations.
    Settings* m_settings;

};

#endif // SETTINGSEXPORT_H
