
#include "UndoableTableWidget.h"

UndoableTableWidget::UndoableTableWidget(QWidget *parent)
    : MinimalTableWidget(parent),
      m_undoStack(new QUndoStack(this))
{
}

QUndoStack *UndoableTableWidget::undoStack() const
{
    return m_undoStack;
}