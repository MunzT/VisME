#include "itemdelegate.h"
#include <QStyleOptionViewItem>


void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem o = option;
    o.decorationPosition = QStyleOptionViewItem::Right;
    QStyledItemDelegate::paint(painter, o, index);
}
