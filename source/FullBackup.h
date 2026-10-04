#pragma once

#include <QByteArray>
#include <QVector>
#include <QString>

class UndoableTableWidget;

/**
 * Snapshot of a table's dimensions and cell text.
 */
class FullBackup
{
public:
    FullBackup() = default;

    explicit FullBackup (const UndoableTableWidget &table);

    void restore(UndoableTableWidget &table) const;

private:
    int rowCount_ = 0;
    int columnCount_ = 0;
    QVector<QString> cellTexts_;
    QByteArray horizontalHeaderState_;
    QByteArray verticalHeaderState_;
};