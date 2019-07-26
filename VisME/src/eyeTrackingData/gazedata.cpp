/***********************************************************************************************//**
 * @author Tanja Munz
 * @file gazedata.cpp
 **************************************************************************************************/
#include "gazedata.h"


GazeData::GazeData()
{}


qreal GazeData::getXPosition() const
{
    return m_xPosition;
}


void GazeData::setXPosition(qreal xPosition)
{
    m_xPosition = xPosition;
}


qreal GazeData::getYPosition() const
{
    return m_yPosition;
}


void GazeData::setYPosition(qreal yPosition)
{
    m_yPosition = yPosition;
}
