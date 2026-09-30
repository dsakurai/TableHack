#pragma once

#include "UndoableTableWidget.h"

class TableWidget : public UndoableTableWidget {
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = nullptr);

public slots:
    void showHorizontalHeaderContextMenu(const QPoint &pos);
    void showVerticalHeaderContextMenu(const QPoint &pos);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
