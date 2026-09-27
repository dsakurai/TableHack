#pragma once

#include "VisibleIndexTableWidget.h"

class MyTableWidget : public VisibleIndexTableWidget {
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);
public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();
    void showHeaderContextMenu(const QPoint &pos);
    void removeSelectedColumns();
    void showVerticalHeaderContextMenu(const QPoint &pos);
    void removeSelectedRows();
    void insertColumnLeft();
    void insertColumnRight();
    void insertRowAbove();
    void insertRowBelow();
    void insertColumnAt(int index);
    void insertRowAt(int index);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum class Line {
        Rows,
        Columns,
    };
    /** Copy the specified range of cells to the clipboard. */

    void copyCells(int r0, int r1, int c0, int c1) const;
    void removeSelectedLines(Line line);
};
