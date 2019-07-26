#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QStyledItemDelegate>

/***********************************************************************************************//**
 * Item delegate.
 **************************************************************************************************/
class ItemDelegate : public QStyledItemDelegate
{

protected:

    /***********************************************************************************************//**
     * Paint function.
     **************************************************************************************************/
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // ITEMDELEGATE_H
