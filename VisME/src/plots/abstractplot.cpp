/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstractplot.cpp
 **************************************************************************************************/
#include "abstractplot.h"
#include <cmath>
#include <QResizeEvent>
#include <QStyleOption>


AbstractPlot::AbstractPlot() :
    m_maxSize(1)
{}


void AbstractPlot::paintEvent(QPaintEvent*)
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

    qreal w = 0;
    qreal h = 0;
    if (m_horizontallyCentered && m_verticallyCentered)
    {
        w = width() / 2.0;
        h = height() / 2.0;
    }

    // move painter
    painter.translate(w, h);

    draw(painter);
}


void AbstractPlot::resizeEvent(QResizeEvent* event)
{
    m_maxSize = std::min(event->size().height(), event->size().width());

    QWidget::resizeEvent(event);
}


void AbstractPlot::drawMessage(const int plotSize, QPainter& painter, const QString message)
{
    painter.drawText(0, 0, int(plotSize), int(plotSize),
                     Qt::AlignVCenter | Qt::AlignHCenter | Qt::TextWordWrap,
                     message);
}


std::vector<std::vector<qreal>> AbstractPlot::dataToBins(std::vector<std::vector<qreal>> data, int bins)
{
    QMap<qreal, qreal> valuesMap;
    std::vector<std::vector<qreal> > orderedValues;

    if (data.empty())
    {
        return orderedValues;
    }

    for (size_t i = 0; i < data.size(); ++i)
    {
        valuesMap.clear();
        for (size_t j = 0; j < data[i].size(); ++j)
        {
            valuesMap[fmod(round(fmod(data[i][j], 360) / bins) * bins, 360)] += 1;
        }

        orderedValues.push_back(getValuesList(valuesMap, bins));
    }
    return orderedValues;
}


std::vector<qreal> AbstractPlot::getValuesList(const QMap<qreal, qreal> valuesMap, int bins)
{
    // creates a list which contains a value for each bin; 0 if no value is available
    std::vector<qreal> valuesForEachBin;

    int index = 0;
    QMapIterator<qreal, qreal> iter(valuesMap);

    while (iter.hasNext())
    {
        iter.next();

        while(index <= iter.key() - bins / 2.0) // make area for comparison large enough (floating points...)
        {
            valuesForEachBin.push_back(0);
            index += bins;
        }

        valuesForEachBin.push_back(iter.value());
        index += bins;
    }
    // add empty slots to the end...
    while(index <= 360 - bins / 2.0)
    {
        valuesForEachBin.push_back(0);
        index += bins;
    }

    return valuesForEachBin;
}
