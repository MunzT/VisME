/***********************************************************************************************//**
 * @author Tanja Munz
 * @file abstractplot.h
 **************************************************************************************************/
#ifndef ABSTRACTPLOT_H
#define ABSTRACTPLOT_H

#include "abstractplot.h"
#include <QPainter>
#include <QWidget>

/***********************************************************************************************//**
 * Abstract class for plots.
 **************************************************************************************************/
class AbstractPlot : public QWidget
{

    Q_OBJECT

public:

    /*******************************************************************************************//**
     * Constructor
     **********************************************************************************************/
    AbstractPlot();

protected:

    /*******************************************************************************************//**
     * Paint event.
     **********************************************************************************************/
    void paintEvent(QPaintEvent*);

    /*******************************************************************************************//**
     * Resize event.
     *
     * @param event resize event
     **********************************************************************************************/
    void resizeEvent(QResizeEvent* event);

    /*******************************************************************************************//**
     * Virtual draw function. Needs to be implemented in subclass.
     **********************************************************************************************/
    virtual void draw(QPainter&) = 0;

    /*******************************************************************************************//**
     * Writes text instead of plotting a diagram.
     *
     * @param plotSize horizontal/vertical size of the plot in px
     * @param painter  painter for drawing
     * @param message  text for the area
     **********************************************************************************************/
    void drawMessage(const int plotSize, QPainter& painter, const QString message);

    /*******************************************************************************************//**
     * Splits the given data to the given number of bins for the inner vector of data. The actual
     * data values do not remain just the number how many values belong to a specific bin.
     *
     * @param data the inner vector contains datat values which will be added to bins
     * @param bins number of bins
     **********************************************************************************************/
    std::vector<std::vector<qreal>> dataToBins(std::vector<std::vector<qreal>> data, int bins);

    /*******************************************************************************************//**
     * Determines for the given map a list which contains for each aggregation bin an entry (0 if
     * no data is available in the map).
     *
     * @param valuesMap map from aggregation bin to value
     * @param bins      number of bins that should be used for aggregating the the data
     **********************************************************************************************/
    std::vector<qreal> getValuesList(const QMap<qreal, qreal> valuesMap, int bins);

    /// Current size of the widget (minimum of x and y direction); used as maximum size for the
    /// data plot.
    int m_maxSize;

    /// When origin of plot is centered in the graph.
    bool m_horizontallyCentered;
    bool m_verticallyCentered;

    /// Color used for plotting data.
    QColor m_mainPlotColor;

};

#endif // ABSTRACTPLOT_H
