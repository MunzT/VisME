/***********************************************************************************************//**
 * @author Tanja Munz
 * @file mathhelpfunctions.h
 **************************************************************************************************/
#ifndef MATHHELPFUNCTIONS_H
#define MATHHELPFUNCTIONS_H

#include <cmath>
#include <QPair>
#include <vector>

/***********************************************************************************************//**
 * This namespace provides functions for mathematical and geometric operations with vectors and
 * matrices.
 **************************************************************************************************/
namespace MathHelpFunctions
{

    /*******************************************************************************************//**
     * Returns the coulmn with the given index of the matrix.
     *
     * @return matrix input matrix
     * @return column column index
     * @return column of matrix
     **********************************************************************************************/
    std::vector<qreal> getColumn(std::vector<std::vector<qreal> > matrix, size_t columnIndex);

    /*******************************************************************************************//**
     * Calculates the median of the elements given in the vector.
     *
     * @return median
     **********************************************************************************************/
    template<class T>
    T median(std::vector<T> vector)
    {
        T result;

        std::sort(vector.data(), vector.data() + vector.size());

        if (vector.size() % 2 == 0)
        {
            result= vector[int(vector.size() / 2) - 1];
        }
        else
        {
            result = vector[int(vector.size() / 2)];
        }

        return result;
    }

    /*******************************************************************************************//**
     * Calculates the p-quantil of the elements given in the vector.
     *
     * @return median
     **********************************************************************************************/
    template<class T>
    T quantile(std::vector<T> vector, qreal p)
    {
        if (vector.empty())
        {
            return 0;
        }

        T result;

        std::sort(vector.data(), vector.data() + vector.size());

        qreal np = vector.size() * p;
        if (ceil(np) == np)
        {
            result = 0.5 * (vector[ceil(np) - 1] + vector[ceil(np)]);
        }
        else
        {
            result = vector[ceil(np) - 1];
        }

        return result;
    }

    /*******************************************************************************************//**
     * Calculates the mean value of the elements given in the vector.
     *
     * @param vector input vector
     * @return mean value
     **********************************************************************************************/
    qreal mean(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Returns the centroid (x and y coordinates) of the given positions in the matrix. Each row
     * in the patrix represents one position.
     *
     * @param matrix matrix with two columns (for x and y coordinates)
     * @return centroid (x and y coordinates) of given positions
     **********************************************************************************************/
    QPair<qreal, qreal> centroid(const std::vector<std::vector<qreal> > matrix);

    /*******************************************************************************************//**
     * Returns the maximum value in the matrix.
     *
     * @param matrix input matrix
     * @return maximum value
     **********************************************************************************************/
    qreal maxValue(std::vector<std::vector<qreal> > matrix);

    /*******************************************************************************************//**
     * Returns the maximum value in the vector.
     *
     * @param vector input vector
     * @return maximum value
     **********************************************************************************************/
    qreal maxValue(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Returns the minimum value in the vector.
     *
     * @param vector input vector
     * @return maximum value
     **********************************************************************************************/
    qreal minValue(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Returns the maximum value of the element in the vector.
     *
     * @param vector input vector
     * @return maximum value
     **********************************************************************************************/
    int maxIndex(std::vector<qreal>& vector);

    /*******************************************************************************************//**
     * Returns the minimum value of the element in the vector.
     *
     * @param vector input vector
     * @return minimum value
     **********************************************************************************************/
    int minIndex(std::vector<qreal>& vector);

    /*******************************************************************************************//**
     * Returns a vector containing the indices of the input vector of elements which are larger
     * than the given value.
     *
     * @param vector input vector
     * @param value  threshold value
     * @return indices of the input vector of elements which are larger than the given value.
     **********************************************************************************************/
    std::vector<size_t> indicesLargerThan(std::vector<qreal>& vector, qreal value);

    /*******************************************************************************************//**
     * Returns a vector containing the indices of the input vector of elements which are equal
     * to the given value.
     *
     * @param vector input vector
     * @param value  threshold value
     * @return indices of the input vector of elements which are equal to the given value.
     **********************************************************************************************/
    template<class T>
    std::vector<size_t> indicesUnequalTo(std::vector<T>& vector, T value)
    {
        std::vector<size_t> newV;

        for (size_t i = 0; i < vector.size(); ++i)
        {
            if (vector[i] != value)
            {
                newV.push_back(i);
            }
        }

        return newV;
    }

    /*******************************************************************************************//**
     * Adds for each component the value of the second vector to the value of the first vector and
     * returnes the new vector.
     *
     * @param vector1 input vector 1
     * @param vector2 input vector 2
     * @return new vector
     **********************************************************************************************/
    std::vector<qreal> add(std::vector<qreal> vector1, std::vector<qreal> vector2);

    /*******************************************************************************************//**
     * Adds the given value to each component of the vector and returnes the new vector.
     *
     * @param vector input vector
     * @param value  value added to each component
     * @return new vector
     **********************************************************************************************/
    std::vector<qreal> addToComponents(std::vector<qreal> vector, qreal value);

    /*******************************************************************************************//**
     * Calculates the quare of each component and returnes the new vector.
     *
     * @param vector input vector
     * @return vector with resulting components
     **********************************************************************************************/
    std::vector<qreal> squareComponents(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Calculates the square root of each component and returnes the new vector.
     *
     * @param vector input vector
     * @return vector with squared components
     **********************************************************************************************/
    std::vector<qreal> sqrtComponents(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Divides each component of the vector by the given value and returnes the new vector.
     *
     * @param vector input vector
     * @param value  value used for the division
     * @return new vector
     **********************************************************************************************/
    std::vector<qreal> divideComponrntsBy(std::vector<qreal> vector, qreal value);

    /*******************************************************************************************//**
     * Returns a vector which contains the differences between two following values in a vector.
     * E.g.: [1,2,5,3] -> [1,3,-2].
     *
     * @param vector input vector
     * @return new vector
     **********************************************************************************************/
    std::vector<int> differences(std::vector<int>& vector);

    /*******************************************************************************************//**
     * Creates one vector from multiple vectors.
     *
     * @param vector input vector
     * @return new vector
     **********************************************************************************************/
    std::vector<qreal> toVector(std::vector<std::vector<qreal>> vector);

    /*******************************************************************************************//**
     * Calculates the angular mean value for the values given in the vector.
     * https://en.wikipedia.org/wiki/Mean_of_circular_quantities
     *
     * @param vector input vector
     * @return angular mean
     **********************************************************************************************/
    qreal angularMean(std::vector<qreal> vector);

    /*******************************************************************************************//**
     * Determines the angular distance between the given values.
     *
     * @param value1 first value
     * @param value2 second value
     **********************************************************************************************/
    qreal angularDifference(qreal value1, qreal value2);

    /*******************************************************************************************//**
     * Returns the clockwise angle between the vertical and the line defined by the points P1(x1,x2)
     * and P2(x2,y2).
     *
     * @param x1 x value of first point
     * @param x2 x value of second point
     * @param y1 y value of second point
     * @param y2 y value of second point
     **********************************************************************************************/
    qreal getAngle(const qreal x1, const qreal x2, const qreal y1, const qreal y2);
}

#endif // MATHHELPFUNCTIONS_H
