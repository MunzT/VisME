/***********************************************************************************************//**
 * @author Tanja Munz
 * @file keypressfilter.cpp
 **************************************************************************************************/
#include "keypressfilter.h"
#include <QKeyEvent>


KeyPressFilter::KeyPressFilter()
{}


bool KeyPressFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
      {
        auto keyEvent = static_cast<QKeyEvent*>(event);

        switch(keyEvent->key())
        {
            case Qt::Key_N:
            case Qt::Key_Right:
                emit nextEvent();
                return true;

            case Qt::Key_P:
            case Qt::Key_Left:
                emit previousEvent();
                return true;

            default:
                break;
        }
      }
      return QObject::eventFilter(object, event);
}
