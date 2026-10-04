#include "FullBackup.h"

#include "UndoableTableWidget.h"
#include <QHeaderView>
#include <QTableWidgetItem>

FullBackup::FullBackup(const UndoableTableWidget &table)
{
    if (!table.horizontalHeader()) throw std::runtime_error("Horizontal header is missing");
    if (!table.verticalHeader()) throw std::runtime_error("Vertical header is missing");

    rowCount_ = table.rowCount();
    columnCount_ = table.columnCount();
    horizontalHeaderState_ = table.horizontalHeader()->saveState();
    verticalHeaderState_ = table.verticalHeader()->saveState();

    cellTexts_.resize(rowCount_ * columnCount_);
    for (int row = 0; row < rowCount_; ++row) {
        for (int column = 0; column < columnCount_; ++column) {
            const QTableWidgetItem *item = table.item(row, column);
            cellTexts_[row * columnCount_ + column] = item ? item->text() : QString{};
        }
    }
}

void FullBackup::restore(UndoableTableWidget &table) const
{
    table.setRowCount(rowCount_);
    table.setColumnCount(columnCount_);

    table.horizontalHeader()->restoreState(horizontalHeaderState_);
    table.verticalHeader()->restoreState(verticalHeaderState_);

    for (int row = 0; row < rowCount_; ++row) {
        for (int column = 0; column < columnCount_; ++column) {
            table.setItem(row, column,
                          new QTableWidgetItem(cellTexts_[row * columnCount_ + column]));
        }
    }
}