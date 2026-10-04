#pragma once

#include "MinimalTableWidget.h"

#include <QUndoStack>

class UndoableTableWidget : public MinimalTableWidget
{
    Q_OBJECT
public:
    explicit UndoableTableWidget(int rows, int columns, QWidget *parent = nullptr);

    QUndoStack *undoStack() const;
    void undo();
    void redo();
private:
    QUndoStack *m_undoStack;
    // bool guard_ = false;
    bool m_isUndoRedoInProgress = false;
};