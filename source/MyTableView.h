#pragma once

#include <QTableView>

class QStandardItemModel;

class MyTableView : public QTableView {
    Q_OBJECT
public:
    explicit MyTableView(QWidget *parent = nullptr);
public slots:
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteFromClipboard();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    /** Copy the specified range of cells to the clipboard. */
    void copyCells(int r0, int r1, int c0, int c1) const;
};
