
#include "UndoableTableWidget.h"
#include "BruteForceUndoCommand.h"

#include <QHeaderView>
#include <QScopedValueRollback>
#include <QSignalBlocker>

UndoableTableWidget::UndoableTableWidget(int rows, int columns, QWidget *parent)
    : MinimalTableWidget(rows, columns, parent),
      m_undoStack(new QUndoStack(this))
{
    BruteForceUndoCommand::initializeBackup(this);
    
    // Note: push() calls redo().

    connect(model(), &QAbstractItemModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });

    connect(model(), &QAbstractItemModel::rowsInserted, [this](const QModelIndex &parent, int first, int last){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });
    connect(model(), &QAbstractItemModel::rowsRemoved, [this](const QModelIndex &parent, int first, int last){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });
    connect(model(), &QAbstractItemModel::columnsInserted, [this](const QModelIndex &parent, int first, int last){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });
    connect(model(), &QAbstractItemModel::columnsRemoved, [this](const QModelIndex &parent, int first, int last){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });
    
    connect(verticalHeader(), &QHeaderView::sectionMoved, [this](int logicalIndex, int oldVisualIndex, int newVisualIndex){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });

    connect(horizontalHeader(), &QHeaderView::sectionMoved, [this](int logicalIndex, int oldVisualIndex, int newVisualIndex){
        if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        this->undoStack()->push(new BruteForceUndoCommand(this));
    });
}

void UndoableTableWidget::undo()
{
    if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
    if (m_undoStack) {
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        m_undoStack->undo();
    }
}

void UndoableTableWidget::redo()
{
    if (m_isUndoRedoInProgress) return;  // Don’t push new commands while undoing/redoing
    if (m_undoStack) {
        QScopedValueRollback<bool> guard(m_isUndoRedoInProgress, true);
        m_undoStack->redo();
    }
}

QUndoStack *UndoableTableWidget::undoStack() const
{
    return m_undoStack;
}