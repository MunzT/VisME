/***********************************************************************************************//**
 * This file contains the class ColorGenerator.
 *
 *@author Tanja Munz
 * @file colorgenerator.h
 **************************************************************************************************/
#ifndef COLORGENERATOR_H
#define COLORGENERATOR_H

#include <QColor>
#include <cmath>
#include <QVector>

/***********************************************************************************************//**
 * This class contains functions that generate colors for the WOIs.
 **************************************************************************************************/
class ColorGenerator
{

public:

    /// types for color assignment
    enum ColorType {listColor, goldenRatioColor};

    /*******************************************************************************************//**
     * Constructor.
     * Creates a list of color values.
     **********************************************************************************************/
    ColorGenerator();

    /*******************************************************************************************//**
     * Returns the next color calculated with the golden ratio approach.
     *
     * @return next color calculated with the golden ratio approach
     **********************************************************************************************/
    QColor nextGoldenRatioColor();

    /*******************************************************************************************//**
     * Returns the next color from the list.
     *
     * @return next color from the list
     **********************************************************************************************/
    QColor nextListColor();

private:

    /// list of predefined color substitution
    QVector<QColor> colorList;

    /// number of colors that have been set until now
    int colorNumber;

};

#endif // COLORGENERATOR_H
