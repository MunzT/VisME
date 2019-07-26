/***********************************************************************************************//**
 * @author Tanja Munz
 * @file pngexporter.cpp
 **************************************************************************************************/
#include "pngexporter.h"


PNGExporter::PNGExporter(QWidget* parent) :
    QWidget(parent),
    view(nullptr),
    exportFullScene(false)
{}


QString PNGExporter::openSaveDialog(const QString fileName, const QString exportTitle) const
{
    QString filePath = QDir::homePath() + "/" + fileName;

    return QFileDialog::getSaveFileName(parentWidget(),
           tr("Save the %1 as PNG file").arg(exportTitle), filePath,
           tr("PNG files (*.png)"));
}


bool PNGExporter::exportScene(QGraphicsView* visibleView, QString metaData,
                              bool fullScene, const QString filePath)
{
    view = visibleView;
    exportFullScene = fullScene;

    if (filePath.isEmpty())
    {
        return false;
    }
    else
    {
        saveToFile(filePath, metaData);
        return true;
    }
}


bool PNGExporter::exportScene(QWidget* widget, const QString metaData, const QString filePath)
{
    if (filePath.isEmpty())
        return false;
    else
    {
        QImage image;

        image = QImage(widget->size(),
                       QImage::Format_ARGB32);

        image.fill(Qt::transparent);
        QPainter painter(&image);
        widget->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);

        // "Comment"
        image.setText("Comment", metaData);

        // standard key for application
        image.setText("Software", tr("%1").arg(qApp->applicationName()));

        image.save(filePath);

        return true;
    }
}


void PNGExporter::saveToFile(const QString filePath, const QString metaData)
{
    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // http://stackoverflow.com/questions/7451183/how-to-create-image-file-from-
    // qgraphicsscene-qgraphicsview
    // http://www.qtcentre.org/threads/30892-QGraphicsView-Scene-to-png-image

    QImage image;

    // export the full scene
    if (exportFullScene)
    {
        image = QImage(view->mapFromScene(
                           view->scene()->sceneRect()).boundingRect().size(),
                       QImage::Format_ARGB32);
        QPainter painter(&image);

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        view->setForegroundBrush(Qt::NoBrush);
        view->scene()->render(&painter, QRect(), QRect(),
                              Qt::IgnoreAspectRatio);

    }
    // render just a part of the scene (the view)
    else
    {
        image = QImage(view->viewport()->size(), QImage::Format_ARGB32);
        QPainter painter(&image);

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        view->setForegroundBrush(Qt::NoBrush);

        view->render(&painter);
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // "Comment"
    image.setText("Comment", metaData);

    // standard key for application
    image.setText("Software", tr("%1").arg(qApp->applicationName()));

    image.save(filePath);

    // reset cursor
    qApp->restoreOverrideCursor();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}
