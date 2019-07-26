/***********************************************************************************************//**
 * @author Tanja Munz
 * @file statisticsexport.h
 **************************************************************************************************/
#ifndef STATISTICSEXPORT_H
#define STATISTICSEXPORT_H

#include "mainwindow.h"
#include <QFileDialog>

/***********************************************************************************************//**
 * This class is responsible for exporting aggregated statistics to text files.
 **************************************************************************************************/
class StatisticsExport : public QObject
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor.
     * Initializes values.
     *
     * Values used in this class:
     * data types:
     * if exportDirectionalData:
     *     0: #Raw Data Samples in Trials
     *     1: Duration of Trials [s]
     *     2: #Fixations in Trials
     *     3: #Fixations with Microsaccades in Trials
     *     4: Fixations containing Microsaccades [%]
     *     5: Fixation Duration [s]
     *     6: Fixation (with Microsaccades) Duration [s]
     *     7: #Microsaccades in Trials
     *     8: #Microsaccades per Fixation
     *     9: #Microsaccades per Fixation with Microsaccades
     *     10: #Microsaccades per Second (for Trials)
     *     11: #Microsaccades per Second in Fixations (for Trials)
     *     12: Microsaccade Amplitude [°]
     *     13: Inter-saccadic Interval [ms]
     *     14: Microsaccade Duration [ms]
     *     15: Microsaccade Peak Velocity [°/s]
     * else:
     *     0: DirectionFromView
     *     1: DirectionTowardsNextFixation
     *     2: DirectionTowardsPreviousFixation
     *
     * @param parent          parent widget
     * @param eyeTrackingData eye tracking data
     * @param settings        global settings
     **********************************************************************************************/
    StatisticsExport(QWidget* parent, EyeTrackingData* eyeTrackingData, Settings* settings);

public slots:

    /*******************************************************************************************//**
     * Shows the export dialog with check boxes to specify the export values.
     **********************************************************************************************/
    void exportDataToFile();

    /*******************************************************************************************//**
     * Shows another export dialog with check boxes to specify the export values for directional
     * data.
     **********************************************************************************************/
    void exportDirectionalDataToFile();

private slots:

    /*******************************************************************************************//**
     * Creates export files with aggregated data for participants/test conditions.
     *
     * Opens a directory picker dialog, creates a new subdirectory, and creates export files for
     * all selected options.
     *
     * All export filescontain the values valuexy:
     *
     *                participant1, participant2, participant3, ...
     * testCondition1 value11,      value21,      value31
     * testCondition2 value12,      value22,      value32
     * testCondition3 value13,      value23,      value33
     * ...
     **********************************************************************************************/
    void exportAggregatedData();

private:

    /*******************************************************************************************//**
     * Creates a new sub directory for all exports.
     *
     * @param path existing directory path.
     **********************************************************************************************/
    QString subDirPath(const QString path);

    /*******************************************************************************************//**
     * Returns a file nime depending on the data type.
     *
     * @param dataType data type, see above
     * @param dir      existing directory
     **********************************************************************************************/
    QString getFileName(int dataType, const QString dir);

    /*******************************************************************************************//**
     * Returns a list of datat samples for the given participant, test condition and the data type.
     *
     * @param p             current participant
     * @param testCondition current test condition
     * @param dataType      data type, see above
     * @param dataCount     value which has to be used to determine mean value
     * @return list of datat samples for the given participant, test condition and the data type
     **********************************************************************************************/
    std::vector<qreal> getDataSamples(Participant* p, const QString testCondition, int dataType,
                                      int& dataCount);

    /*******************************************************************************************//**
     * Returns the value for the given value type and data samples.
     *
     * if exportDirectionalData:
     *     valueType: 0: min, 1: max, 2: mean, 3: median, 4: sum, 5: single values
     * else:
     *     valueType: 0: mean, 1: single values
     *
     * @param dataSamples list of data samples
     * @param valueType   value type, representing for example min, max, sum, ...
     * @param dataCount   element/samples count
     * @return value as string
     **********************************************************************************************/
    QString getValue(std::vector<qreal> dataSamples, int valueType, int dataCount = -1);

    /// Main window.
    QWidget* m_parent;

    /// Eye tracking data.
    EyeTrackingData* m_eyeTrackingData;

    /// Settings.
    Settings* m_settings;

    /// Directory file path.
    QString m_dirPath;

    /// Checkboxes which contain information about data which will be exported
    std::vector<QCheckBox*> m_exportDataCheckBoxes;
    std::vector<QRadioButton*> m_exportValueTypesCheckBoxes;

    /// Data and value types.
    QStringList m_exportData;
    QStringList m_exportValueTypes;

    /// Indices of m_exportData for which sum can not be determined.
    std::vector<size_t> m_sumNotAvailable;

    /// Directional or other data will be exported
    bool exportDirectionalData;
};

#endif // STATISTICSEXPORT_H
