#pragma once

#include "FullBackup.h"
#include <QUndoCommand>
#include <QPointer>
#include <QMap>

class UndoableTableWidget;

/**
 * Save everything without regard to RAM efficiency.
 */
class BruteForceUndoCommand : public QUndoCommand
{
public:
    static void initializeBackup(const QPointer<UndoableTableWidget> &table);

    BruteForceUndoCommand(const QPointer<UndoableTableWidget> &table);

    void undo() override;
    void redo() override;
    

private:
    
    FullBackup state_before;
    FullBackup state_after;
    static QMap<QPointer<UndoableTableWidget>, FullBackup> s_current_backup;

    QPointer<UndoableTableWidget> table_;
};
