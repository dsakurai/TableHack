#pragma once

#include <QTableWidget>

/**
 * Data equals QTableWidget::text() (i.e. QString).
 * We copy and paste data as just QString text (one QString instance per cell).
 * No fonts or other formatting is considered data.
 */
class MinimalTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit MinimalTableWidget(int rows, int columns, QWidget *parent = nullptr);

    enum class RowOrColumn {
        Row,
        Column,
    };
    enum class BeforeOrAfter {
        Before,
        After,
    };
    /** Remove the selected rows or columns. */
    void removeSelectedLine(RowOrColumn rc);
    /** Insert a row or column before or after the specified position in the header. */
    void insertLine(RowOrColumn rc, BeforeOrAfter ba, const QPoint& posInHeader);

public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();

private:
    /** Copy the specified range of cells to the clipboard. */
    void copyCells(const QModelIndexList &idxs) const;

};

