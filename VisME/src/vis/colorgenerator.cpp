/***********************************************************************************************//**
 * @author Tanja Munz
 * @file colorgenerator.cpp
 **************************************************************************************************/
#include "colorgenerator.h"


ColorGenerator::ColorGenerator() :
    colorNumber(0)
{
    colorList << QColor(166, 206, 227) << QColor(31, 120, 180)
              << QColor(178, 223, 138) << QColor(51, 160, 44)
              << QColor(251, 154, 153) << QColor(227, 26, 28)
              << QColor(253, 191, 111) << QColor(255, 127, 0)
              << QColor(202, 178, 214) << QColor(106, 61, 154)
              << QColor(255, 255, 153) << QColor(240, 240, 10);
}


QColor ColorGenerator::nextGoldenRatioColor()
{
    // http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-
    // programmatically/
    // use golden ratio
    qreal goldenRatioConjugate = 0.618033988749895;
    //qreal s = 0.7;
    qreal s = 0.4;
    qreal v = 0.8;
    qreal h = (colorNumber++ + 6) * goldenRatioConjugate;
    h = fmod(h, 1);
    return QColor::fromHsv(int(h * 359), int(s * 255), int(v * 255));
}


QColor ColorGenerator::nextListColor()
{
    return colorList[colorNumber++ % colorList.size()];
}
