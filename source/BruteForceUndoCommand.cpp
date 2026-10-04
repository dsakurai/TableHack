#include "BruteForceUndoCommand.h"
#include "UndoableTableWidget.h"

QMap<QPointer<UndoableTableWidget>, FullBackup>
    BruteForceUndoCommand::s_current_backup;

void BruteForceUndoCommand::initializeBackup(const QPointer<UndoableTableWidget> &table)
{
    if (!table) throw std::runtime_error("Table pointer is null");
    s_current_backup.remove(table);
    s_current_backup.insert(table, FullBackup{*table.get()});
}

BruteForceUndoCommand::BruteForceUndoCommand(const QPointer<UndoableTableWidget> &table)
    : table_(table)
{
    auto* t = table_.get();
    if (!t)
        throw std::runtime_error("Table pointer is null");

    auto &global_backup = s_current_backup[table_];
    state_before = global_backup;
    global_backup = FullBackup{*t};
    state_after = global_backup;
}

void BruteForceUndoCommand::undo()
{
	if (!table_)
		throw std::runtime_error("Table pointer is null");
	state_before.restore(*table_);
	s_current_backup.insert(table_, state_before);
}

void BruteForceUndoCommand::redo()
{
	if (!table_)
		throw std::runtime_error("Table pointer is null");
	state_after.restore(*table_);
	s_current_backup.insert(table_, state_after);
}
