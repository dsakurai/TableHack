#include "TableWidget.h"

TableWidget::TableWidget(QWidget *parent)
	: VisibleIndexTableWidget(parent) {
}

void TableWidget::insertRowBelow()
{
    VisibleIndexModelIndexList idxs = selectedIndexes();

    QModelIndexList idxs = sel->selectedIndexes();

    int index = rowCount();

    int maxr = -1;
    for (const QModelIndex &idx : idxs)
        maxr = qMax(maxr, idx.row());
    if (maxr >= 0)
        index = maxr + 1;

    insertRowAt(index);
}
