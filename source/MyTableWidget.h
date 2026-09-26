#pragma once

#include <QTableWidget>

class QStandardItemModel;

class MyTableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);
public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();
    void showHeaderContextMenu(const QPoint &pos);
    void removeSelectedColumns();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    /** Copy the specified range of cells to the clipboard. */
    void copyCells(int r0, int r1, int c0, int c1) const;
};
