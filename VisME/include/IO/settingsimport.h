/***********************************************************************************************//**
 * @author Tanja Munz
 * @file settingsimport.h
 **************************************************************************************************/
#ifndef SETTINGSIMPORT_H
#define SETTINGSIMPORT_H

#include "mainwindow.h"
#include "userinterface.h"

class UserInterface;

/***********************************************************************************************//**
 * This class is responsible for importing data such as trials, participants, parameters.
 **************************************************************************************************/
class SettingsImport : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     *
     * @param window   parent window
     * @param settings settings for the visualizations
     * @param ui       user interface container
     **********************************************************************************************/
    SettingsImport(QWidget* window, Settings* settings, UserInterface* ui);

public slots:

    /*******************************************************************************************//**
     * Opens the given file, extracts microsaccade detection parameters and updates the values in
     * the UI.
     *
     * @param fileName file name of the settings file
     **********************************************************************************************/
    void readMicrosaccadsSettingsFile(const QString fileName);

    /*******************************************************************************************//**
     * Opens the given file, extracts fixation detection parameters and updates the values in
     * the UI.
     *
     * @param fileName file name of the settings file
     **********************************************************************************************/
    void readFixationSettingsFile(const QString fileName);

    /*******************************************************************************************//**
     * Opens the given file, extracts color settings and updates the values in
     * the UI.
     *
     * @param fileName file name of the settings file
     **********************************************************************************************/
    void readColorSettingsFile(const QString fileName);

private:

    /// Main Window.
    QWidget* m_parent;

    /// Eye tracking data containing all participant data.
    EyeTrackingData* m_data;

    /// Current settings for eye tracking data and visualization.
    Settings* m_settings;

    /// User interface.
    UserInterface* m_ui;
};

#endif // SETTINGSIMPORT_H
