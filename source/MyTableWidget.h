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
    void showVerticalHeaderContextMenu(const QPoint &pos);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum class RowOrColumn {
        Row,
        Column,
    };

    /** Copy the specified range of cells to the clipboard. */
    void copyCells(const QModelIndexList &idxs) const;

    /** Remove the selected rows or columns. */
    void removeSelectedLine(RowOrColumn rc);
};
