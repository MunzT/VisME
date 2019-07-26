/***********************************************************************************************//**
 * @author Tanja Munz
 * @file main.cpp
 **************************************************************************************************/
#include "darkstyle.h"
#include "mainwindow.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("VisME - Visual Microsaccades Explorer");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser cmd;
    cmd.addHelpOption();
    cmd.addVersionOption();
    cmd.addPositionalArgument("file", QApplication::tr("main", "File to open."), "[file]");
    cmd.process(app);
    const QStringList args = cmd.positionalArguments();

    // apply dark style
    CDarkStyle::assign();

    MainWindow window;
    window.show();

    if (args.length() > 0)
    {
        window.openFiles(args);
    }

    // for testing
    //window.openFiles(QStringList(QFileInfo("filepath.maf").absoluteFilePath()));

    return app.exec();
}
