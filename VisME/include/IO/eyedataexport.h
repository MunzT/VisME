/***********************************************************************************************//**
 * @author Tanja Munz
 * @file eyedataexport.h
 **************************************************************************************************/
#ifndef EYEDATAEXPORT_H
#define EYEDATAEXPORT_H

#include "eyetrackingdata.h"
#include "trial.h"
#include <fstream>
#include <QFileDialog>

/***********************************************************************************************//**
 * This class is responsible for exporting data such as trials, participants, parameters.
 **************************************************************************************************/
class EyeDataExport : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     *
     * @param parent   parent widget
     * @param data     eye tracking data
     * @param settings global settings
     **********************************************************************************************/
    EyeDataExport(QWidget* parent, EyeTrackingData* data, Settings* settings);

public slots:

    /*******************************************************************************************//**
     * Export data of the current trial (gaze positions, fixations and microsaccades) to a file.
     **********************************************************************************************/
    void exportTrialDataToFile();

    /*******************************************************************************************//**
     * Export data of the current participant (gaze positions, fixations and microsaccades) to a file.
     **********************************************************************************************/
    void exportCurrentParticipantDataToFile();

    /*******************************************************************************************//**
     * Export data of all participants to files.
     **********************************************************************************************/
    void exportAllParticipantDataToFile();

private:

    /*******************************************************************************************//**
     * Writes all data of the given trial to out.
     *
     * @param participant data of this participant will be written
     * @param out         text stream element to which data will be written
     **********************************************************************************************/
    void writeHeader(Participant& participant, std::ofstream& out);

    /*******************************************************************************************//**
     * Writes all data of the given trial to out.
     *
     * @param trial data of this trial will be written
     * @param out   text stream element to which data will be written
     **********************************************************************************************/
    void writeTrial(Trial& trial, std::ofstream& out);

    /*******************************************************************************************//**
     * Help function to export data of a participant to a file.
     *
     * @param participant participant
     * @param out         out text stream
     **********************************************************************************************/
    void exportParticipantDataToFile(Participant* participant, std::ofstream& out);

    /*******************************************************************************************//**
     * Shows a save file dialog and returns a std::ofstream element.
     *
     * @return reason    reason for the file save dialog
     * @return fileTypes file types for the save dialog or file extension if a dir is given
     * @return p         participant who is used for the file name
     * @return dir       directory which is used for the save file path
     **********************************************************************************************/
    void exportFile(std::ofstream& out,
                    const QString& reason,
                    const QString& fileTypes,
                    Participant* p = nullptr,
                    const QString dir = QString()) const;

    /// Main window.
    QWidget* m_parent;

    /// Eye tracking data containing all participant data.
    EyeTrackingData* m_data;

    /// Current settings for visualizations.
    Settings* m_settings;

};

#endif // EYEDATAEXPORT_H
