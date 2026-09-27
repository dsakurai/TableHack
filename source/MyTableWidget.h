#pragma once

#include <QTableWidget>
#include <QModelIndex>
#include <QModelIndexList>

class QStandardItemModel;

class MyTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);
public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();
    void showHorizontalHeaderContextMenu(const QPoint &pos);
    void removeSelectedColumns();
    void showVerticalHeaderContextMenu(const QPoint &pos);
    void removeSelectedRows();
    void insertRowAbove();
    void insertRowBelow();
    void insertRowAt(int index);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum class Line {
        Rows,
        Columns,
    };
    /** Copy the specified range of cells to the clipboard. */

    void copyCells(const QModelIndexList &idxs) const;
    void removeSelectedLines(Line line);
};
