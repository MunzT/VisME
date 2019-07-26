/***********************************************************************************************//**
 * This file contains the class PNGExporter.
 *
 * @author Tanja Munz
 * @file pngexporter.h
 **************************************************************************************************/
#ifndef PNGEXPORTER_H
#define PNGEXPORTER_H

#include <iostream>
#include <QApplication>
#include <QFileDialog>
#include <QGraphicsView>
#include <QString>

/***********************************************************************************************//**
 * This class provides functions to save the full scene or a part of the scene as a PNG file.
 **************************************************************************************************/
class PNGExporter : public QWidget
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     *
     * @param parent parent object of this object
     **********************************************************************************************/
    PNGExporter(QWidget* parent);

    /*******************************************************************************************//**
     * Opens a dialog to select the file path where the file shall be stored.
     *
     * @param fileName    default file name for the file that shall be exported
     * @param exportTitle the title specifies what is being exported
     * @return the file path where the file shall be stored
     **********************************************************************************************/
    QString openSaveDialog(const QString fileName, const QString exportTitle) const;

public slots:

    /*******************************************************************************************//**
     * Opens a dialog to select the file path where the file shall be stored.
     * If the file path is valid the PNG file is being created.
     *
     * @param visibleView the drawing area includes the scene that shall be
     *                    saved as a PNG-file
     * @param metaData    the meta data that shall be set for the file
     * @param fullScene   whether the full scene should be exported or the
     *                    visible section
     * @param filePath    file path where the file shall be saved
     **********************************************************************************************/
    bool exportScene(QGraphicsView* visibleView, QString metaData,
                     bool fullScene, const QString filePath);

    /*******************************************************************************************//**
     * Opens a dialog to select the file path where the file shall be stored.
     * If the file path is valid the PNG file for the given widget is being
     * created.
     *
     * @param widget    the widget contains the graphics that shall be saved as
     *                  a PNG-file
     * @param metaData  the meta data that shall be set for the file
     * @param filePath  file path where the file shall be saved
     **********************************************************************************************/
    bool exportScene(QWidget* widget, const QString metaData,  const QString filePath);

private:

    /*******************************************************************************************//**
     * Renders the scene in a PNG file.
     *
     * @param filePath where the file shall be stored
     * @param metaData    the meta data that shall be set for the file
     **********************************************************************************************/
    void saveToFile(const QString filePath, const QString metaData);

    /// the view that shows the content that should be saved as an image
    QGraphicsView* view;

    /// whether the full view should be exported or a section of it
    bool exportFullScene;
};

#endif // PNGEXPORTER_H
