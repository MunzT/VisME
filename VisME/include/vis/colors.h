/***********************************************************************************************//**
 * @author Tanja Munz
 * @file colors.h
 **************************************************************************************************/
#ifndef COLORS_H
#define COLORS_H

#include <QColor>

// red
const QColor darkRedTransparent = QColor(128, 0, 60, 200);

const QColor pink = QColor(255, 0, 90, 170);

// blue
const QColor blue = QColor(65, 120, 255, 220);
const QColor lightBlue = QColor(90, 180, 230, 255);

// orange
const QColor orangeTransparent = QColor(255, 100, 0, 100);

// purple
const QColor purpleTransparent = QColor(100, 0, 250, 100);

// green
const QColor brightGreenTransparent = QColor(50, 255, 50, 200);
const QColor greenTransparent = QColor(100, 200, 100, 200);

// turquoise
const QColor turquoiseTransparent = QColor(50, 200, 200, 100);

// gray
const QColor black = QColor(0, 0, 0, 255);
const QColor blackLightTransparent = QColor(0, 0, 0, 200);
const QColor blackLightTransparent2 = QColor(0, 0, 0, 100);

// gray
const QColor gray = QColor(100, 100, 100, 255);
const QColor grayTransparent = QColor(150, 150, 150, 150);

// white
const QColor white = QColor(255, 255, 255, 255);
const QColor whiteLightTransparent = QColor(255, 255, 255, 200);

/***********************************************************************************************//**
 * Color values used for the application.
 **************************************************************************************************/
struct Colors
{
    Colors() :
        sampleConnectionColor(new QColor(brightGreenTransparent)),
        sampleColor(new QColor(greenTransparent)),
        fixationSamplesColor(new QColor(blackLightTransparent)),
        fixationColor(new QColor(blue)),
        fixationHighlightColor(new QColor(darkRedTransparent)),
        saccadeColor(new QColor(blue)),
        saccadesDirectionStartColor(new QColor(darkRedTransparent)),
        microsaccadesColor(new QColor(pink)),
        microsaccadesDirectionEndColor(new QColor(whiteLightTransparent)),
        missingDataColor(new QColor(purpleTransparent)),
        yValueColor(new QColor(turquoiseTransparent)),
        xValueColor(new QColor(orangeTransparent)),
        velocityColor(new QColor(grayTransparent)),
        dataPlotColor(new QColor(lightBlue)),
        dataPlotBaseColor(new QColor(gray)),
        sceneBorderColor(new QColor(grayTransparent)),
        meanValueColor(new QColor(black)),
        eventColor(new QColor(blackLightTransparent2)),
        alphaForPlots(60)
    {}

    std::string getColorString(QColor* c)
    {
      return "#" + (QString::number(c->alpha(), 16)).toStdString() + c->name().mid(1).toStdString();
    }

    // Colors used
    QColor* sampleConnectionColor;
    QColor* sampleColor;
    QColor* fixationSamplesColor;

    QColor* fixationColor;
    QColor* fixationHighlightColor;

    QColor* saccadeColor;
    QColor* saccadesDirectionStartColor;

    QColor* microsaccadesColor;
    QColor* microsaccadesDirectionEndColor;

    QColor* missingDataColor;

    QColor* yValueColor;
    QColor* xValueColor;
    QColor* velocityColor;

    QColor* dataPlotColor;

    QColor* dataPlotBaseColor;
    QColor* sceneBorderColor;

    QColor* meanValueColor;
    
    QColor* eventColor;

    int alphaForPlots;

};

#endif // COLORS_H
