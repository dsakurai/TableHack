#include "MinimalTableWidget.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QStringList>
#include <clipboard_win32.h>
#include <QClipboard>
#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QPointer>
#include <QSet>
#include <algorithm>

MinimalTableWidget::MinimalTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    // Move rows and columns within the table.
    horizontalHeader()->setSectionsMovable(true);
    horizontalHeader()->setDragEnabled(true);
    //
    verticalHeader()->setSectionsMovable(true);
    verticalHeader()->setDragEnabled(true);
}

class VisibleIndex {
public:
    VisibleIndex(const QModelIndex& idx, const MinimalTableWidget *w)
        : index_(idx), tableWidget_(w) {}
    
    int row() const { 
        return tableWidget_->verticalHeader()->visualIndex(index_.row());
    }
    int column() const {
        return tableWidget_->horizontalHeader()->visualIndex(index_.column());
    }
private:
    const QModelIndex index_;
    const QPointer<const MinimalTableWidget> tableWidget_;
};

void MinimalTableWidget::copyCells(const QModelIndexList &idxs) const
{
    if (idxs.isEmpty())
        return;

    QAbstractItemModel *m = model();
    if (!m)
        return;
    
    // compute bounding rectangle in visible (header) coordinates
    int vr_min = INT_MAX, vr_max = INT_MIN, vc_min = INT_MAX, vc_max = INT_MIN;
    for (const QModelIndex &idx : idxs) {
        VisibleIndex v(idx, this);
        int vr = v.row();
        int vc = v.column();
        vr_min = qMin(vr_min, vr);
        vr_max = qMax(vr_max, vr);
        vc_min = qMin(vc_min, vc);
        vc_max = qMax(vc_max, vc);
    }

    const int num_rows = (vr_max - vr_min + 1);
    const int num_cols = (vc_max - vc_min + 1);

    // build matrix and fill on-the-fly using positions relative to r0/c0
    QVector<QVector<QString>> matrix(num_rows, QVector<QString>(num_cols));
    for (const QModelIndex &idx : idxs) {
        VisibleIndex v(idx, this);
        matrix[v.row() - vr_min][v.column() - vc_min] = m->data(idx, Qt::DisplayRole).toString();
    }

    // transform matrix -> HTML
    QString html;
    html += "<table>";
    for (int r = 0; r < num_rows; ++r) {
        html += "<tr>";
        for (int c = 0; c < num_cols; ++c) {
            QString cell = matrix[r][c];
            html += "<td>" + cell.toHtmlEscaped() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    // transform matrix -> plain TSV
    QStringList lines;
    for (int r = 0; r < num_rows; ++r) {
        QStringList row;
        for (int c = 0; c < num_cols; ++c)
            row << matrix[r][c];
        lines << row.join('\t');
    }
    QString plain = lines.join("\r\n");

    copyHtmlToClipboardWin32(html, plain);
}

void MinimalTableWidget::copySelectionToClipboard()
{
    QItemSelectionModel *sel = selectionModel();
    if (!sel)
        return;
    copyCells(sel->selectedIndexes());
}

void MinimalTableWidget::cutSelectionToClipboard()
{
    QAbstractItemModel *m = model();
    if (!m) return;

    QItemSelectionModel *sel = selectionModel();
    if (!sel) return;

    const QModelIndexList idxs = sel->selectedIndexes();
    
    // copy selected range
    copyCells(idxs);
    // then clear those cells
    for (const QModelIndex &idx : idxs)
        m->setData(idx, QString());
}

void MinimalTableWidget::pasteFromClipboard()
{
    QAbstractItemModel *m = model();
    if (!m)
        return;
    const QClipboard *cb = QApplication::clipboard();
    QString text = cb->text();
    if (text.isEmpty())
        return;

    // Determine start position: top-left of selection
    const QModelIndexList idxs = selectionModel() ? selectionModel()->selectedIndexes() : QModelIndexList();
    if (idxs.isEmpty()) {return;}
    
    if (idxs.size() > 1) {return;}
    VisibleIndex visible {idxs.first(), this};
    const int startRowVisible = visible.row();
    const int startColVisible = visible.column();

    QStringList rows = text.split('\n');
    for (int r = 0; r < rows.size(); ++r) {
        QString line = rows[r];
        // trim CR
        if (!line.isEmpty() && line.endsWith('\r'))
            line.chop(1);
        QStringList cells = line.split('\t');
        for (int c = 0; c < cells.size(); ++c) {
            int rr = startRowVisible + r;
            int cc = startColVisible + c;
            if (rr < 0 || cc < 0)
                throw std::out_of_range("Negative index while pasting from clipboard");
            if (rr >= m->rowCount() || cc >= m->columnCount())
                continue; // clipboard content is larger than the model of the Table, do not expand model here

            m->setData(m->index(
                verticalHeader()->logicalIndex(rr),
                horizontalHeader()->logicalIndex(cc)),
                cells[c]);
        }
    }
}

void MinimalTableWidget::removeSelectedLine(RowOrColumn rc)
{
    QItemSelectionModel *sel = selectionModel();
    if (!sel) return;

    QSet<int> columns; // either column coordinates or row coordinates. Regarding the variable naming, we just assume it's columns

    for (const QModelIndex &cell : sel->selectedIndexes()) {
        if (rc == RowOrColumn::Column)
            columns.insert(cell.column());
        if (rc == RowOrColumn::Row)
            columns.insert(cell.row());
    }

    QList<int> columnList = columns.values();
    std::sort(columnList.begin(), columnList.end(), std::greater<int>());
    for (int c : columnList)
        if (rc == RowOrColumn::Column)
            removeColumn(c);
        else if (rc == RowOrColumn::Row)
            removeRow(c);
}