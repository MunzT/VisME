/***********************************************************************************************//**
 * @author Tanja Munz
 * @file DiagramLegend.cpp
 **************************************************************************************************/
#define _USE_MATH_DEFINES
#include "colors.h"
#include "diagramlegend.h"
#include "settings.h"
#include <cmath>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>


DiagramLegend::DiagramLegend(Settings* settings) :
    m_settings(settings)
{}


void DiagramLegend::paintEvent(QPaintEvent*)
{
    QStyleOption styleOption;
    styleOption.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    painter.setBrush(Qt::black);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget,& opt,& painter, this);

    int testConditionCount = m_settings->m_testConditions.size();
    setMaximumHeight(testConditionCount * 20);

    if (testConditionCount > 0)
    {
        QMapIterator<QString, QColor> it(m_settings->m_testConditions);
        int count = 0;
        while (it.hasNext())
        {
            it.next();
            QString testCondition = it.key();
            QColor color = it.value();

            painter.setPen(QPen(color, 1));
            painter.setBrush(color);
            int w = int(width() / 2.0);
            int h = height() / testConditionCount;
            painter.drawRect(0, count * h, w, h);
            painter.drawText(w, count * h, w, h, Qt::AlignVCenter | Qt::AlignHCenter, testCondition);

            count++;
        }
    }
    setMinimumHeight(13 * testConditionCount);
}


void DiagramLegend::show()
{
    int testConditionCount = m_settings->m_testConditions.size();
    setMaximumHeight(testConditionCount * 20);

    QWidget::show();
}
