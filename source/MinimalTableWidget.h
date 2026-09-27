#pragma once

#include <QTableWidget>

class MinimalTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit MinimalTableWidget(QWidget *parent = nullptr);

    enum class RowOrColumn {
        Row,
        Column,
    };
    /** Remove the selected rows or columns. */
    void removeSelectedLine(RowOrColumn rc);

public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();

private:
    /** Copy the specified range of cells to the clipboard. */
    void copyCells(const QModelIndexList &idxs) const;

};

