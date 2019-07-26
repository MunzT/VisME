/***********************************************************************************************//**
 * @author Tanja Munz
 * @file saccadedetector.cpp
 **************************************************************************************************/
#define _USE_MATH_DEFINES
#include "mathhelpfunctions.h"
#include "saccadedetector.h"
#include "settings.h"
#include "trial.h"
#include <iostream>
#include <cmath>
#include <QList>

SaccadeDetector::SaccadeDetector(){}


std::vector<std::vector<Saccade>> SaccadeDetector::determineMicrosaccades(Trial* trial,
                                                                          Settings* settings)
{
    std::vector<GazeType> gazeTypes = {GazeType::GT_Right, GazeType::GT_Left, GazeType::GT_Average};
    std::vector<std::vector<Saccade>> msforGazeTypes = std::vector<std::vector<Saccade>>();

    int ignoreBeforeMissingData = settings->m_microsaccadeSettings->m_useIgnoreBeforeMissingData ?
                            trial->timeToSamples(settings->m_microsaccadeSettings->m_ignoreBeforeMissingData) : 0;
    int ignoreAfterMissingData = settings->m_microsaccadeSettings->m_useIgnoreAfterMissingData ?
                          trial->timeToSamples(settings->m_microsaccadeSettings->m_ignoreAfterMissingData) : 0;

    for (size_t i = 0; i < gazeTypes.size(); ++i)
    {
        std::vector<Saccade> microsaccades = std::vector<Saccade>();

        // first detect areas without missing data then use only these parts for detection
        // detect microsaccades for all data (not just fixation areas...)
        const QMap<int, GazeData>& gazeData = trial->gazeData(gazeTypes[i]);

        if (!gazeData.isEmpty())
        {
            int start = gazeData.firstKey();
            int end = gazeData.lastKey();

            std::vector<Saccade> saccadeFilterResult;

            int indicesStart = -1;
            int indicesEnd = -1;
            bool removeNextBeginning = false;
            if (start <= end)
            {
                for (int j = start; j <= end; ++j)
                {
                    if (gazeData.contains(j))
                    {
                        if (indicesStart == -1)
                        {
                            if (removeNextBeginning)
                            {
                                indicesStart = j + ignoreAfterMissingData;
                                removeNextBeginning = false;
                            }
                            else
                            {
                                indicesStart = std::max(j, indicesStart);
                            }
                        }
                    }
                    else if (indicesStart != -1)
                    {
                        indicesEnd = std::max(indicesStart, j - ignoreBeforeMissingData);
                        if (indicesStart < indicesEnd)
                        {
                            saccadeFilterResult = calculateMicrosaccades(gazeData, indicesStart,
                                                                         indicesEnd, trial, settings);
                            microsaccades.insert(microsaccades.end(), saccadeFilterResult.begin(), saccadeFilterResult.end());

                        }
                        indicesStart = -1;
                        removeNextBeginning = true;
                    }
                }
            }

            if (indicesStart != -1)
            {
               indicesEnd = end;
               saccadeFilterResult = calculateMicrosaccades(gazeData, indicesStart, indicesEnd,
                                                            trial, settings);
               microsaccades.insert(microsaccades.end(), saccadeFilterResult.begin(), saccadeFilterResult.end());

            }

        }
        msforGazeTypes.push_back(microsaccades);
    }

    // binocular saccades
    if (settings->m_microsaccadeSettings->m_binocular)
    {
        // right and left binocular saccades: if overlap
        QPair<std::vector<Saccade>, std::vector<Saccade>> binSaccades =
                determineBinocularSaccades(msforGazeTypes[1], msforGazeTypes[0]);

        // average saccades: if overlap with both saccades in the right and left eye
        QPair<std::vector<Saccade>, std::vector<Saccade>> binSaccadesAverage =
                determineBinocularSaccades(msforGazeTypes[2], msforGazeTypes[0]);
        binSaccadesAverage = determineBinocularSaccades(binSaccadesAverage.first, msforGazeTypes[1]);

        std::vector<std::vector<Saccade>> binSaccadesResult = {binSaccades.second, binSaccades.first,
                                                               binSaccadesAverage.first};
        return binSaccadesResult;
    }

    return msforGazeTypes;
}


std::vector<std::vector<Saccade>> SaccadeDetector::determineRegularSaccades(Trial* trial,
                                                                            Settings* settings)
{
    std::vector<GazeType> gazeTypes = {GazeType::GT_Right, GazeType::GT_Left, GazeType::GT_Average};
    std::vector<std::vector<Saccade>> saccadesforGazeTypes = std::vector<std::vector<Saccade>>();

    for (size_t i = 0; i < gazeTypes.size(); ++i)
    {
        std::vector<Saccade> saccades = std::vector<Saccade>();
        const QMap<int, GazeData>& g = trial->gazeData(gazeTypes[i]);

        if (!g.empty())
        {
            QMapIterator<int, GazeData> it(g);

            std::vector<int> indices;
            int prevIndex = g.firstKey() - 1;

            int start = g.firstKey();
            int end = g.lastKey();

            int ignoreBeforeMissingData = settings->m_fixationSettings->m_useIgnoreBeforeMissingData ?
                                    trial->timeToSamples(settings->m_fixationSettings->m_ignoreBeforeMissingData) : 0;
            int ignoreAfterMissingData = settings->m_fixationSettings->m_useIgnoreAfterMissingData ?
                                  trial->timeToSamples(settings->m_fixationSettings->m_ignoreAfterMissingData) : 0;

            qreal pixelsPerDegree = trial->getParticipant()->getPixelsPerDegree();

            int ignoreUntilIndex = -1;
            int minimum = start;

            while(it.hasNext())
            {
                it.next();

                if (it.key() < start)
                {
                    prevIndex = it.key();
                    continue;
                }


                // use saccade filter on areas without missing data
                if ((it.key() - prevIndex > 1 || !it.hasNext() || it.key() == end))
                {
                    if (!indices.empty())
                    {
                        std::vector<std::vector<qreal> > x;

                        int maxValues = indices.size();
                        it.key() - prevIndex > 1 ? maxValues = std::max(0, maxValues - ignoreBeforeMissingData) : maxValues;

                        for (size_t i = 0; i < maxValues; ++i)
                        {
                            std::vector<qreal> values;

                            values.push_back(g[indices[i]].getXPosition() / pixelsPerDegree); // 0
                            values.push_back(g[indices[i]].getYPosition() / pixelsPerDegree); // 1
                            values.push_back(indices[i]); // 2, index/time

                            x.push_back(values);
                        }

                        QPair<qreal, qreal> newRadius;

                        std::vector<Saccade> saccadeFilterResult =
                                saccadesFilter(x,
                                               newRadius,
                                               settings->m_fixationSettings,
                                               trial);

                        saccades.insert(saccades.end(), saccadeFilterResult.begin(), saccadeFilterResult.end());

                        indices.clear();
                    }

                    it.key() - prevIndex > 1 ? ignoreUntilIndex = it.key() + ignoreAfterMissingData : ignoreUntilIndex = it.key();
                }
                else
                {
                    if (it.key() > ignoreUntilIndex)
                    {
                        indices.push_back(it.key());
                    }
                }

                prevIndex = it.key();
            }
        }

        saccadesforGazeTypes.push_back(saccades);
    }

    if (settings->m_fixationSettings->m_binocular)
    {
        // right and left binocular saccades: if overlap
        QPair<std::vector<Saccade>, std::vector<Saccade>> binSaccades =
                determineBinocularSaccades(saccadesforGazeTypes[1], saccadesforGazeTypes[0]);

        // average saccades: if overlap with both saccades in the right and left eye
        QPair<std::vector<Saccade>, std::vector<Saccade>> binSaccadesAverage =
                determineBinocularSaccades(saccadesforGazeTypes[2], saccadesforGazeTypes[0]);
        binSaccadesAverage = determineBinocularSaccades(binSaccadesAverage.first,
                                                        saccadesforGazeTypes[1]);

        std::vector<std::vector<Saccade>> binSaccadesResult = {binSaccades.second, binSaccades.first,
                                                               binSaccadesAverage.first};

        saccadesforGazeTypes = binSaccadesResult;
    }


    int ignoreAtStart = settings->m_fixationSettings->m_useIgnoreAtStart ?
                            trial->timeToSamples(settings->m_fixationSettings->m_ignoreAtStart) : 0;
    int ignoreAtEnd = settings->m_fixationSettings->m_useIgnoreAtEnd ?
                          trial->timeToSamples(settings->m_fixationSettings->m_ignoreAtEnd) : 0;

    for (size_t i = 0; i < gazeTypes.size(); ++i)
    {
        const QMap<int, GazeData>& g = trial->gazeData(gazeTypes[i]);

        if (!g.empty())
        {
            // remove saccades outside detection area
            int start = g.firstKey();
            int end = g.lastKey();

            if (settings->m_fixationSettings->m_useIgnoreAtStart ||settings->m_fixationSettings->m_useIgnoreAtEnd)
            {
                std::vector<int> indicesToDelete;

                // remove invalid sacades
                for (size_t j = 0; j < saccadesforGazeTypes[i].size(); ++j)
                {
                    if (int(saccadesforGazeTypes[i][j].getOnsetIndex()) < start + ignoreAtStart ||
                            int(saccadesforGazeTypes[i][j].getEndIndex()) > end - ignoreAtEnd)
                    {
                        indicesToDelete.push_back(int(j));
                    }
                }
                if (!indicesToDelete.empty())
                {
                    for (int j = int(indicesToDelete.size()) - 1; j >= 0 ; --j)
                    {
                        saccadesforGazeTypes[i].erase(saccadesforGazeTypes[i].begin() + indicesToDelete[j]);
                    }
                }
            }

            // check for minimum distance between saccades -> for overshooting
            if (settings->m_fixationSettings->m_useMinIntersaccInterval && !saccadesforGazeTypes[i].empty())
            {
                std::vector<int> indicesToDelete;
                for (size_t j = 1; j < saccadesforGazeTypes[i].size(); ++j)
                {
                    size_t timeDiff = saccadesforGazeTypes[i][j].getOnsetIndex() - saccadesforGazeTypes[i][j - 1].getEndIndex();
                    if (timeDiff <= trial->timeToSamples(settings->m_fixationSettings->m_minIntersaccInterval))
                    {
                        indicesToDelete.push_back(int(j));
                    }
                }
                if (!indicesToDelete.empty())
                {
                    for (int j = int(indicesToDelete.size()) - 1; j >= 0 ; --j)
                    {
                        saccadesforGazeTypes[i].erase(saccadesforGazeTypes[i].begin() + indicesToDelete[j]);
                    }
                }
            }
        }
    }

    return saccadesforGazeTypes;
}


void SaccadeDetector::removeMicrosaccadesForIntersaccadicInterval(Trial* trial, Settings* settings, Fixation* fixation)
{
    if (!settings->m_microsaccadeSettings->m_useMinIntersaccInterval ||
            fixation->getMicrosaccadesCount(false) <= 1)
    {
        return;
    }

    size_t prevMSIndex = 0;
    std::vector<int> msToRemove;

    for (size_t i = 1; i < fixation->getMicrosaccadesCount(false); ++i)
    {
        Saccade* prevMs = fixation->getMicrosaccadeAt(prevMSIndex, false);
        Saccade* ms = fixation->getMicrosaccadeAt(i, false);

        // check for minimum distance between saccades -> for overshooting
        size_t timeDiff = ms->getOnsetIndex() - prevMs->getEndIndex();
        if (timeDiff <= trial->timeToSamples(settings->m_microsaccadeSettings->m_minIntersaccInterval))
        {
            // remove
            msToRemove.push_back(int(i));
        }
        else
        {
            prevMSIndex = i;
        }
    }

    fixation->removeMicrosaccades(msToRemove, false);
}


std::vector<Saccade> SaccadeDetector::calculateMicrosaccades(
        const QMap<int, GazeData>& gazeData,
        int indicesStart, int indicesEnd,
        Trial* trial,
        Settings* settings)
{
    std::vector<std::vector<std::vector<qreal> > > xSections;
    std::vector<std::vector<qreal> > x;

    qreal pixelsPerDegree = trial->getParticipant()->getPixelsPerDegree();

    for (int i = indicesStart; i <= indicesEnd; ++i)
    {
        if (!gazeData.contains(i))
        {
            if (!x.empty())
            {
                xSections.push_back(x);
                x.clear();
            }
            continue;
        }

        std::vector<qreal> values;

        values.push_back(gazeData[i].getXPosition() / pixelsPerDegree); // 0
        values.push_back(gazeData[i].getYPosition() / pixelsPerDegree); // 1
        values.push_back(i); //2, index/time

        x.push_back(values);
    }
    if (!x.empty())
    {
        xSections.push_back(x);
    }

    std::vector<Saccade> saccades;
    foreach (std::vector<std::vector<qreal> > xValues, xSections)
    {
        QPair<qreal, qreal> newRadius;
        std::vector<Saccade> s = saccadesFilter(
            xValues,
            newRadius,
            settings->m_microsaccadeSettings,
            trial);

         saccades.insert(saccades.end(), s.begin(), s.end());
    }

    return saccades;
}


std::vector<Saccade> SaccadeDetector::saccadesFilter(std::vector<std::vector<qreal> >& x,
                                                     QPair<qreal, qreal>& newRadius,
                                                     SaccadeFilterSettings* settings,
                                                     Trial* trial)
{
    // velocity
    std::vector<std::vector<qreal> > v = determineVelocity(x, trial->frequencyRate(), settings->m_velocityWindowSize);

    if (v.empty()) // no data for processing
    {
        return std::vector<Saccade>();
    }

    // threshold
    std::vector<qreal> col0 = MathHelpFunctions::getColumn(v, 0);
    std::vector<qreal> col1 = MathHelpFunctions::getColumn(v, 1);

    auto medx = MathHelpFunctions::median<qreal>(col0);
    auto msdx = sqrt(MathHelpFunctions::median<qreal>(
                          MathHelpFunctions::squareComponents(
                              MathHelpFunctions::addToComponents(col0, -medx))));
    auto medy = MathHelpFunctions::median<qreal>(col1);
    auto msdy = sqrt(MathHelpFunctions::median<qreal>(
                          MathHelpFunctions::squareComponents(
                              MathHelpFunctions::addToComponents(col1, -medy))));

    if (msdx < 1e-10)
    {
        msdx = sqrt(pow(MathHelpFunctions::mean(col0), 2));
        if (msdx < 1e-10)
        {
            return std::vector<Saccade>();
        }
    }
    if (msdy < 1e-10)
    {
        msdy = sqrt(pow(MathHelpFunctions::mean(col1), 2));
        if (msdy < 1e-10)
        {
            return std::vector<Saccade>();
        }
    }

    qreal radiusX = settings->m_velocityThreshold * msdx;
    qreal radiusY = settings->m_velocityThreshold * msdy;
    newRadius = QPair<qreal, qreal>(radiusX, radiusY);

    // Apply test criterion: elliptic treshold
    std::vector<qreal> test = MathHelpFunctions::add(
                                  MathHelpFunctions::squareComponents(
                                      MathHelpFunctions::divideComponrntsBy(col0, radiusX)),
                              MathHelpFunctions::squareComponents(
                                      MathHelpFunctions::divideComponrntsBy(col1, radiusY)));

    std::vector<size_t> indx = MathHelpFunctions::indicesLargerThan(test, 1);

    // Determine saccades
    size_t N = indx.size();
    std::vector<Saccade> sac;
    size_t dur = 1;
    int a = 0;
    int k = 0;
    int b;

    // Loop over saccade candidates
    while (k < int(N) - 1)
    {
        if (indx[k+1] - indx[k] == 1)
        {
            dur += 1;
        }
        else
        {
            // Minimum duration criterion (exception: last saccade)
            if (dur >= trial->timeToSamples(settings->m_minDuration))
            {
                b = k;
                Saccade m = Saccade(indx[a], indx[b], 0, 0, 0, 0, 0);
                sac.push_back(m);
            }
            a = k + 1;
            dur = 1;
        }
        k = k + 1;
    }

    // Check minimum duration for last saccade
    if (N > 0 && dur >= trial->timeToSamples(settings->m_minDuration))
    {
        b = k;
        Saccade m = Saccade(indx[a], indx[b], 0, 0, 0, 0, 0);
        sac.push_back(m);
    }

    if (!sac.empty())
    {
        // Compute peak velocity, horiztonal and vertical components
        for (size_t s = 0; s < sac.size(); ++s)
        {
            // Onset and offset for saccades
            a = int(sac[s].getOnsetIndex());
            b = int(sac[s].getEndIndex());

            std::vector<int> idx(b - a + 1);
            for (int i = 0; i <= b - a; ++i)
            {
                idx[i] = i + a;
            }

            std::vector<qreal> v_idx0(idx.size());
            std::vector<qreal> v_idx1(idx.size());
            std::vector<qreal> x_idx0(idx.size());
            std::vector<qreal> x_idx1(idx.size());
            for (size_t i = 0; i < idx.size(); ++i)
            {
                v_idx0[i] = v[idx[i]][0];
                v_idx1[i] = v[idx[i]][1];
                x_idx0[i] = x[idx[i]][0];
                x_idx1[i] = x[idx[i]][1];
            }

            // Saccade peak velocity (vpeak)
            qreal vpeak = MathHelpFunctions::maxValue(
                              MathHelpFunctions::sqrtComponents(
                                  MathHelpFunctions::add(
                                    MathHelpFunctions::squareComponents(v_idx0),
                                    MathHelpFunctions::squareComponents(v_idx1))));

            sac[s].setPeakVelocity(vpeak);

            // Saccade vector (dx,dy);
            qreal dx = x[b][0] - x[a][0];
            qreal dy = x[b][1] - x[a][1];

            sac[s].setHorizontalComponent(dx);
            sac[s].setVerticalComponent(dy);
            // Saccade amplitude (dX,dY)
            qreal minx = MathHelpFunctions::minValue(x_idx0);
            qreal maxx = MathHelpFunctions::maxValue(x_idx0);
            qreal miny = MathHelpFunctions::minValue(x_idx1);
            qreal maxy = MathHelpFunctions::maxValue(x_idx1);
            int ix1 = MathHelpFunctions::minIndex(x_idx0);
            int ix2 = MathHelpFunctions::maxIndex(x_idx0);
            int iy1 = MathHelpFunctions::minIndex(x_idx1);
            int iy2 = MathHelpFunctions::maxIndex(x_idx1);
            qreal dX = ((ix2 - ix1) < 0 ? -1.0 : 1.0) * (maxx - minx);
            qreal dY = ((iy2 - iy1) < 0 ? -1.0 : 1.0) * (maxy - miny);
            sac[s].setHorizontalAmplitude(dX);
            sac[s].setVerticalAmplitude(dY);

            // from paper "McCamy et al.: Microsaccades Scan Highly Informative Image Areas"
            // amplitude > 1 degree => saccade
            // amplitude <= 1 degree => microsaccade
            qreal amplitude = sqrt(pow(dX, 2) + pow(dY, 2));
            sac[s].setValid((settings->m_useMaxAmplitude ? amplitude <= settings->m_maxAmplitude : true) &&
                            (settings->m_useMinAmplitude ? amplitude >= settings->m_minAmplitude : true));

            // set valid according to peak velocity
            sac[s].setValid(sac[s].isValid() &&
                            (settings->m_useMaxVelocity ? sac[s].getPeakVelocity() <= settings->m_maxVelocity : true) &&
                            (settings->m_useMinVelocity ? sac[s].getPeakVelocity() >= settings->m_minVelocity : true));

            // set valid according to duration
            sac[s].setValid(sac[s].isValid() && (settings->m_useMaxDuration ? sac[s].getDuration() <= trial->timeToSamples(settings->m_maxDuration) : true));
        }

        // check for saccades that are too large
        for (int i = int(sac.size()) - 1; i >= 0 ; --i) // i needs to be int
        {
            sac[i].setEndIndex(x[sac[i].getEndIndex()][2]);
            sac[i].setOnsetIndex(x[sac[i].getOnsetIndex()][2]);

            // delete saccades that are too large (-> they are no microsaccades)
            if (!sac[i].isValid())
            {
                sac.erase(sac.begin() + i);
            }
        }

        return sac;
    }
    return std::vector<Saccade>();
}


std::vector<std::vector<qreal> > SaccadeDetector::determineVelocity(
        std::vector<std::vector<qreal> >& xValues, qreal frequency, size_t windowSize)//, int type)
{
    // first two columns in xValues is x and y value; third column in xValues is for time
    int N = int(xValues.size()); // rows

    std::vector<std::vector<qreal> > velocityMatrix;
    for (int i = 0; i < N; ++i)
    {
        std::vector<qreal> velocityRow;
        for (int j = 0; j < 2; ++j)
        {
            velocityRow.push_back(0);
        }
        velocityMatrix.push_back(velocityRow);
    }

    // in order to determine the velocity windowSize data samples are required
    int size = (int(windowSize) - 1) / 2;
    if (xValues.size() < windowSize)
    {
        return velocityMatrix;
    }

    // with general window size
    for (size_t i = 0; i < 2; ++i)
    {
        for (int j = size; j < N - size; ++j)
        {
            qreal windowValue = 0;
            for (int k = -size; k <= size; ++k)
            {
                if (k == 0)
                {
                    continue;
                }
                windowValue += (k < 0 ? -1 : 1) * xValues[size_t(j + k)][i];
            }
            velocityMatrix[size_t(j)][i] = frequency / 6.0 * windowValue;
        }
    }

    // values for which not enough data samples are available
    for (size_t i = 0; i < 2; ++i)
    {
        for (int j = 0; j < size - 1; ++j)
        {
            qreal windowValue1 = 0;
            qreal windowValue2 = 0;
            for (int k = 0; k <= size; ++k)
            {
                if (size % 2 == 0 && k % 2 != 0)
                {
                    continue;
                }
                windowValue1 += (k > size / 2 ? 1 : -1) * xValues[k][i];
                windowValue2 += (k > size / 2 ? -1 : 1) * xValues[N - k - 1][i];
            }

            velocityMatrix[1][i] = frequency / qreal(size) * windowValue1;
            velocityMatrix[N - 2][i] = frequency / qreal(size) * windowValue2;
        }
    }
    return velocityMatrix;

    // original algorithm used by Engbert
    //if (type == 2)
    //{
    //    for (size_t i = 0; i < 2; ++i)
    //    {
    //        for (int j = 2; j < N - 2; ++j)
    //        {
    //            velocityMatrix[j][i] = frequency / 6.0 * (xValues[j + 2][i] + xValues[j + 1][i] -
    //                                                     xValues[j - 1][i] - xValues[j - 2][i]);
    //        }
    //    }
    //
    //    for (size_t i = 0; i < 2; ++i)
    //    {
    //        velocityMatrix[1][i] = frequency / 2.0 * (xValues[2][i] - xValues[0][i]);
    //        velocityMatrix[N - 2][i] = frequency / 2.0 * (xValues[N - 1][i] - xValues[N - 3][i]);
    //    }
    //}
    //else
    //{
    //    for (size_t i = 0; i < 2; ++i)
    //    {
    //        for (int j = 1; j < N - 2; ++j)
    //        {
    //            velocityMatrix[j][i] = frequency / 2.0 * (xValues[j + 1][i] - xValues[j - 1][i]);
    //        }
    //    }
    //}
}


QPair<std::vector<Saccade>, std::vector<Saccade>> SaccadeDetector::determineBinocularSaccades(
        std::vector<Saccade> leftEye, std::vector<Saccade> rightEye)
{
    std::vector<Saccade> binL;
    std::vector<Saccade> binR;
    std::vector<Saccade> monoL;
    std::vector<Saccade> monoR;

    int NB = 0;
    int NR = 0;
    int NL = 0;
    if (leftEye.size() * rightEye.size() > 0)
    {
        // Determine saccade clusters
        int TR = 0;
        int TL = 0;
        for (size_t i = 0; i < rightEye.size(); ++i)
        {
            TR = std::max(TR, int(rightEye[i].getEndIndex()));
        }
        for (size_t i = 0; i < leftEye.size(); ++i)
        {
            TL = std::max(TL, int(leftEye[i].getEndIndex()));
        }

        int TB = std::max(TR, TL);

        std::vector<int> s(size_t(TB + 1), 0);

        for (size_t i = 0; i < leftEye.size(); ++i)
        {
            for (size_t j = leftEye[i].getOnsetIndex(); j <= leftEye[i].getEndIndex(); ++j)
            {
                s[j] = 1;
            }
        }
        for (size_t i = 0; i < rightEye.size(); ++i)
        {
            for (size_t j = rightEye[i].getOnsetIndex(); j <= rightEye[i].getEndIndex(); ++j)
            {
                s[j] = 1;
            }
        }
        s[0] = 0;
        s[size_t(TB)] = 0;

        // Find onsets and offsets of microsaccades
        std::vector<int> diff = MathHelpFunctions::differences(s);
        std::vector<size_t> onoff = MathHelpFunctions::indicesUnequalTo(diff, 0);

        std::vector<std::vector<size_t> > m;
        for (size_t i = 0; i < onoff.size(); i += 2)
        {
            std::vector<size_t> v;
            v.push_back(onoff[i] + 1);
            v.push_back(onoff[i + 1] + 1);
            m.push_back(v);
        }

        // Determine binocular saccades
        for (size_t i = 0; i < m.size(); ++i)
        {
            std::vector<size_t> left;
            std::vector<size_t> right;
            for (size_t j = 0; j < leftEye.size(); ++j)
            {
                if (m[i][0] <= leftEye[j].getOnsetIndex() && leftEye[j].getEndIndex() <= m[i][1])
                {
                    left.push_back(j);
                }
            }
            for (size_t j = 0; j < rightEye.size(); ++j)
            {
                if (m[i][0] <= rightEye[j].getOnsetIndex() && rightEye[j].getEndIndex() <= m[i][1])
                {
                    right.push_back(j);
                }
            }

            // Binocular saccades
            if (left.size() * right.size() > 0)
            {
                std::vector<qreal> ampr;
                std::vector<qreal> ampl;

                for (size_t j = 0; j < right.size(); ++j)
                {
                    ampr.push_back(sqrt(pow(rightEye[right[j]].getHorizontalAmplitude(), 2) +
                                   pow(rightEye[right[j]].getVerticalAmplitude(), 2)));
                }
                for (size_t j = 0; j < left.size(); ++j)
                {
                    ampl.push_back(sqrt(pow(leftEye[left[j]].getHorizontalAmplitude(), 2) +
                                   pow(leftEye[left[j]].getVerticalAmplitude(), 2)));
                }
                // Determine largest event in each eye
                int ir = MathHelpFunctions::maxIndex(ampr);
                int il = MathHelpFunctions::maxIndex(ampl);
                NB = NB + 1;

                binL.push_back(leftEye[left[il]]);
                binR.push_back(rightEye[right[ir]]);
            }
            else
            {
                // Determine monocular saccades
                if (left.empty())
                {
                    NR = NR + 1;

                    std::vector<qreal> ampr;
                    for (size_t j = 0; j < right.size(); ++j)
                    {
                        ampr.push_back(sqrt(pow(rightEye[right[j]].getHorizontalAmplitude(), 2) +
                                       pow(rightEye[right[j]].getVerticalAmplitude(), 2)));
                    }
                }
                if (right.empty())
                {
                    NL = NL + 1;

                    std::vector<qreal> ampl;
                    for (size_t j = 0; j < left.size(); ++j)
                    {
                        ampl.push_back(sqrt(pow(leftEye[left[j]].getHorizontalAmplitude(), 2) +
                                       pow(leftEye[left[j]].getVerticalAmplitude(), 2)));
                    }
                }
            }
        }
    }
    else
    {
        // Special case of exclusively monocular saccades
        if (rightEye.empty())
        {
            monoL = leftEye;
        }
        if (leftEye.empty())
        {
            monoR = rightEye;
        }
    }

    return QPair<std::vector<Saccade>, std::vector<Saccade>>(binL, binR); // monoR, monoL?
}

