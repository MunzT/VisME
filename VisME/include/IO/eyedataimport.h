/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedataimport.h
 **************************************************************************************************/
#ifndef EYEDATAIMPORT_H
#define EYEDATAIMPORT_H

#include "eyetrackingdata.h"
#include "mainwindow.h"
#include <QObject>

/***********************************************************************************************//**
 * This class is responsible for importing data such as trials, participants, parameters.
 **************************************************************************************************/
class EyeDataImport : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     **********************************************************************************************/
    EyeDataImport(QWidget* window, EyeTrackingData* data, Settings* settings);

    /*******************************************************************************************//**
     * Reads the given file and extracts relevant fixation/microsaccade data to visualize them.
     *
     * @param fileNames files that should be opened
     * @param add       if files shall be added to the already loaded files or if loaded ones
     *                  should be closed
     **********************************************************************************************/
    void openFiles(const QStringList& fileNames, bool add = false);

    /*******************************************************************************************//**
     * Opens the csv file with test condition specifications and updates the test condition
     * relationships for the specified trial within the file.
     *
     * @param fileName file name of the test condition file
     **********************************************************************************************/
    void readCSV(const QString& fileName);

signals:

    /*******************************************************************************************//**
     * Signal that all participants shall be updated.
     **********************************************************************************************/
    void updateParticipantsInUI();

    /*******************************************************************************************//**
     * Signal to set the given test conditions in the project.
     **********************************************************************************************/
    void setTestConditionsInUI(std::set<QString>);

private:

    /*******************************************************************************************//**
     * Checks if the given file was already loaded.
     *
     * @param filePath filep path
     * @return whether the given file was already loaded
     **********************************************************************************************/
    bool fileAlreadyLoaded(const QString& filePath);

    /// Main window.
    QWidget* m_parent;

    /// Eye tracking data containing all participant data.
    EyeTrackingData* m_data;

    /// Current settings for visualizations.
    Settings* m_settings;

};

#endif // EYEDATAIMPORT_H
