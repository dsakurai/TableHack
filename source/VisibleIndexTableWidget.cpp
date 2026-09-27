#include "VisibleIndexTableWidget.h"

#include "VisibleHeaderView.h"

#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

VisibleIndexTableWidget::VisibleIndexTableWidget(QWidget *parent)
    : QWidget(parent) {
        auto *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        qTableWidget_ = new QTableWidget(this);
        layout->addWidget(qTableWidget_);
        
        
        // Move rows and columns within the table.
        qTableWidget_->horizontalHeader()->setSectionsMovable(true);
        qTableWidget_->horizontalHeader()->setDragEnabled(true);
        horizontalHeader_ = new VisibleHeader(qTableWidget_->horizontalHeader(), this);
        //
        qTableWidget_->verticalHeader()->setSectionsMovable(true);
        qTableWidget_->verticalHeader()->setDragEnabled(true);
        verticalHeader_ = new VisibleHeader(qTableWidget_->verticalHeader(), this);
}

VisibleIndexTableWidget::~VisibleIndexTableWidget() = default;

// VisibleIndexModelIndexList VisibleIndexTableWidget::selectedIndices() const {
//     VisibleIndexModelIndexList visibleIndices;

//     if (!qTableWidget_)
//         return visibleIndices;

//     QModelIndexList idxs = qTableWidget_->selectionModel()->selectedIndexes();

//     for (const QModelIndex &idx : idxs) {
//         auto visibleRow = qTableWidget_->verticalHeader()->visualIndex(idx.row());
//         auto visibleColumn = qTableWidget_->horizontalHeader()->visualIndex(idx.column());
//         visibleIndices.append({visibleRow, visibleColumn});
//     }
//     return visibleIndices;
// }
