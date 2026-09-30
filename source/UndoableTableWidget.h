#pragma once

#include "MinimalTableWidget.h"

#include <QUndoStack>

class UndoableTableWidget : public MinimalTableWidget
{
    Q_OBJECT
public:
    explicit UndoableTableWidget(QWidget *parent = nullptr);

    QUndoStack *undoStack() const;
private:
    QUndoStack *m_undoStack;
};