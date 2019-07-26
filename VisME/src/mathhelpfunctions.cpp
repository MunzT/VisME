/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mathhelpfunctions.cpp
 **************************************************************************************************/
#define _USE_MATH_DEFINES
#include "mathhelpfunctions.h"
#include <iostream>
#include <cmath>
#include <numeric>
#include <QPair>
#include <vector>


std::vector<qreal> MathHelpFunctions::getColumn(std::vector<std::vector<qreal> > matrix,
                                                size_t columnIndex)
{
    std::vector<qreal> column;
    if (matrix.size() < 1 || matrix[0].size() <= columnIndex)
    {
        return column;
    }
    for (size_t i = 0; i < matrix.size(); ++i)
    {
        column.push_back(matrix[i][columnIndex]);
    }
    return column;
}


qreal MathHelpFunctions::mean(std::vector<qreal> vector)
{
    qreal sum = std::accumulate(vector.begin(), vector.end(), 0.0);
    qreal mean = sum / vector.size();

    return mean;
}


QPair<qreal, qreal> MathHelpFunctions::centroid(const std::vector<std::vector<qreal> > matrix)
{
    return QPair<qreal, qreal>(mean(getColumn(matrix, 0)), mean(getColumn(matrix, 1)));
}


qreal MathHelpFunctions::maxValue(std::vector<std::vector<qreal> > matrix)
{
    qreal max = 0;

    for (size_t j = 0; j < matrix.size(); ++j)
    {
        qreal tempMax = *std::max_element(std::begin(matrix[j]),
                                          std::end(matrix[j]));
        max = std::max(max, tempMax);
    }
    return max;
}


qreal MathHelpFunctions::maxValue(std::vector<qreal> vector)
{
    qreal max = 0;
    for (size_t i = 0; i < vector.size(); ++i)
    {
        if (i == 0)
        {
            max = vector[0];
        }
        else
        {
            max = std::max(max, vector[i]);
        }
    }
    return max;
}


qreal MathHelpFunctions::minValue(std::vector<qreal> vector)
{
    qreal min = 0;
    for (size_t i = 0; i < vector.size(); ++i)
    {
        if (i == 0)
        {
            min = vector[0];
        }
        else
        {
            min = std::min(min, vector[i]);
        }
    }
    return min;
}


int MathHelpFunctions::maxIndex(std::vector<qreal>& vector)
{
    int maxIndex = -1;
    qreal max = 0.0;

    for (size_t i = 0; i < vector.size(); ++i)
    {

        if (i == 0)
        {
            max = vector[i];
            maxIndex = int(i);
        }

        else if (max < vector[i])
        {
            max = vector[i];
            maxIndex = int(i);
        }
    }

    return maxIndex;
}


int MathHelpFunctions::minIndex(std::vector<qreal>& vector)
{
    int minIndex = -1;
    qreal min = 0.0;

    for (size_t i = 0; i < vector.size(); ++i)
    {

        if (i == 0)
        {
            min = vector[i];
            minIndex = int(i);
        }

        else if (min > vector[i])
        {
            min = vector[i];
            minIndex = int(i);
        }
    }

    return minIndex;
}


std::vector<size_t> MathHelpFunctions::indicesLargerThan(std::vector<qreal>& vector, qreal value)
{
    std::vector<size_t> newV;

    for (size_t i = 0; i < vector.size(); ++i)
    {
        if (vector[i] > value)
        {
            newV.push_back(i);
        }
    }

    return newV;
}


std::vector<qreal> MathHelpFunctions::add(std::vector<qreal> vector1, std::vector<qreal> vector2)
{
    for (size_t i = 0; i < vector1.size(); ++i)
    {
        vector1[i] += vector2[i];
    }
    return vector1;
}


std::vector<qreal> MathHelpFunctions::addToComponents(std::vector<qreal> vector, qreal value)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        vector[i] += value;
    }
    return vector;
}


std::vector<qreal> MathHelpFunctions::squareComponents(std::vector<qreal> vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        vector[i] = vector[i] * vector[i];
    }
    return vector;
}


std::vector<qreal> MathHelpFunctions::sqrtComponents(std::vector<qreal> vector)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        vector[i] = std::sqrt(vector[i]);
    }
    return vector;
}


std::vector<qreal> MathHelpFunctions::divideComponrntsBy(std::vector<qreal> vector, qreal value)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        vector[i] /= value;
    }
    return vector;
}


std::vector<int> MathHelpFunctions::differences(std::vector<int>& vector)
{
    std::vector<int> newVector;
    for (size_t i = 0; i < vector.size() - 1; ++i)
    {
        newVector.push_back(vector[i + 1] - vector[i]);
    }

    return newVector;
}


std::vector<qreal> MathHelpFunctions::toVector(std::vector<std::vector<qreal>> vector)
{
    std::vector<qreal> newVector;

    for (size_t i = 0; i < vector.size(); ++i)
    {
        newVector.insert(newVector.end(), vector[i].begin(), vector[i].end());
    }
    return newVector;
}


qreal MathHelpFunctions::angularMean(std::vector<qreal> vector)
{
    qreal cosSum = 0;
    qreal sinSum = 0;
    qreal mean = 0;

    for (size_t i = 0; i < vector.size(); ++i)
    {
        sinSum += sin(vector[i] / 180.0 * M_PI);
        cosSum += cos(vector[i] / 180.0 * M_PI);
    }
    cosSum /= vector.size();
    sinSum /= vector.size();

    if (sinSum > 0 && cosSum > 0)
    {
        mean = atan(sinSum/cosSum) * 180.0 / M_PI;
    }
    if (cosSum < 0)
    {
        mean = atan(sinSum/cosSum) * 180.0 / M_PI + 180.0;
    }
    if (sinSum < 0 && cosSum > 0)
    {
        mean = atan(sinSum/cosSum) * 180.0 / M_PI + 360.0;
    }
    return mean;
}


qreal MathHelpFunctions::angularDifference(qreal value1, qreal value2)
{
    qreal diff = fmod(value2 - value1, 360);
    qreal sign = -1;
    if ((diff >= 0 && diff <= 180) || (diff <= -180 && diff >= -360))
    {
        sign = 1;
    }

    if (diff > 180)
    {
        diff = 360 - diff;
    }
    else if (diff < -180)
    {
        diff = diff + 360;
    }

    diff = abs(diff) * sign;

    return diff;
}


qreal MathHelpFunctions::getAngle(const qreal x1, const qreal x2,
                                  const qreal y1, const qreal y2)
{
    // not: positixe x axis to the right; positive y axis downwards; angle related to the vertical
    // axis which has 0 degree at the top.

    // https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
    qreal x_1 = 0;
    qreal x_2 = x2 - x1;
    qreal y_1 = -1;
    qreal y_2 = y2 - y1;

    qreal dot = x_1 * x_2 + y_1 * y_2; // dot product between [x1, y1] and [x2, y2]
    qreal det = x_1 * y_2 - y_1 * x_2; // determinant
    qreal angle = std::atan2(qreal(det), qreal(dot)) * 180.0 / M_PI; // atan2(y, x) or atan2(sin, cos)

    return fmod(angle + 360, 360);
}
