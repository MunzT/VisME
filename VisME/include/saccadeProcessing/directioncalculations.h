#ifndef DIRECTIONCALCULATIONS_H
#define DIRECTIONCALCULATIONS_H

#include "enums.h"
#include <vector>
#include <QObject>

class Trial;
class Fixation;

/***********************************************************************************************//**
 * This class is responsible for determining of angles of microsaccades/saccades/... in relation
 * to the view/other fixations.
 **************************************************************************************************/
class DirectionCalculations
{

public:

    /*******************************************************************************************//**
     * Constructor.
     * Does nothing.
     **********************************************************************************************/
    DirectionCalculations();

    /*******************************************************************************************//**
     * Determines angles of microsaccades/saccades/... in relation to the view/other fixations for
     * the given fixations.
     *
     * @param trial             current trial
     * @param dataPlotDirection type of directions
     * @param dataPlotData      type of data
     * @param anglesForTrial    angles will be appended to this list
     * @param fixations         list of fixations
     **********************************************************************************************/
    void getAngularDataForTrial(Trial* trial,
                                PlotDirection dataPlotDirection,
                                PlotData dataPlotData,
                                std::vector<qreal>& anglesForTrial,
                                std::vector<Fixation*> fixations);
};

#endif // DIRECTIONCALCULATIONS_H
