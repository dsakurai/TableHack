#include "MyTableWidget.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QStringList>
#include <clipboard_win32.h>
#include <QClipboard>
#include <QApplication>
#include <QKeyEvent>

MyTableWidget::MyTableWidget(QWidget *parent)
    : QTableWidget(parent)
{ }

// removed: copyAllToClipboard() - use copySelectionToClipboard() instead

void MyTableWidget::copyCells(int r0, int r1, int c0, int c1) const
{
    QAbstractItemModel *m = model();
    if (!m) {
        return;
    }
    QString html;
    html += "<table>";
    for (int r = r0; r <= r1; ++r) {
        html += "<tr>";
        for (int c = c0; c <= c1; ++c) {
            QModelIndex idx = m->index(r, c);
            QString cell = m->data(idx, Qt::DisplayRole).toString();
            html += "<td>" + cell.toHtmlEscaped() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table>";

    QStringList lines;
    for (int r = r0; r <= r1; ++r) {
        QStringList row;
        for (int c = c0; c <= c1; ++c) {
            QModelIndex idx = m->index(r, c);
            row << m->data(idx, Qt::DisplayRole).toString();
        }
        lines << row.join('\t');
    }
    QString plain = lines.join("\r\n");

    copyHtmlToClipboardWin32(html, plain);
}

void MyTableWidget::copySelectionToClipboard()
{
    QItemSelectionModel *sel = selectionModel();
    if (!sel)
        return;
    QModelIndexList idxs = sel->selectedIndexes();
    int r0 = INT_MAX, r1 = INT_MIN, c0 = INT_MAX, c1 = INT_MIN;
    for (const QModelIndex &idx : idxs) {
        r0 = qMin(r0, idx.row());
        r1 = qMax(r1, idx.row());
        c0 = qMin(c0, idx.column());
        c1 = qMax(c1, idx.column());
    }
    copyCells(r0, r1, c0, c1);
}

void MyTableWidget::cutSelectionToClipboard()
{
    QAbstractItemModel *m = model();
    if (!m)
        return;
    QItemSelectionModel *sel = selectionModel();
    if (!sel)
        return;
    QModelIndexList idxs = sel->selectedIndexes();
    // copy selected range then clear those cells
    int r0 = INT_MAX, r1 = INT_MIN, c0 = INT_MAX, c1 = INT_MIN;
    for (const QModelIndex &idx : idxs) {
        r0 = qMin(r0, idx.row());
        r1 = qMax(r1, idx.row());
        c0 = qMin(c0, idx.column());
        c1 = qMax(c1, idx.column());
    }
    copyCells(r0, r1, c0, c1);
    for (const QModelIndex &idx : idxs)
        m->setData(idx, QString());
}

void MyTableWidget::pasteFromClipboard()
{
    QAbstractItemModel *m = model();
    if (!m)
        return;
    const QClipboard *cb = QApplication::clipboard();
    QString text = cb->text();
    if (text.isEmpty())
        return;

    // Determine start position: top-left of selection or (0,0)
    int startRow = 0, startCol = 0;
    QModelIndexList idxs = selectionModel() ? selectionModel()->selectedIndexes() : QModelIndexList();
    if (!idxs.isEmpty()) {
        int r = INT_MAX, c = INT_MAX;
        for (const QModelIndex &i : idxs) { r = qMin(r, i.row()); c = qMin(c, i.column()); }
        startRow = r; startCol = c;
    }

    QStringList rows = text.split('\n');
    for (int r = 0; r < rows.size(); ++r) {
        QString line = rows[r];
        // trim CR
        if (!line.isEmpty() && line.endsWith('\r'))
            line.chop(1);
        QStringList cells = line.split('\t');
        for (int c = 0; c < cells.size(); ++c) {
            int rr = startRow + r;
            int cc = startCol + c;
            if (rr < 0 || cc < 0)
                continue;
            if (rr >= m->rowCount() || cc >= m->columnCount())
                continue; // do not expand model here
            m->setData(m->index(rr, cc), cells[c]);
        }
    }
}

void MyTableWidget::keyPressEvent(QKeyEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_C) {
        copySelectionToClipboard();
        return;
    }
    if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_X) {
        cutSelectionToClipboard();
        return;
    }
    if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_V) {
        pasteFromClipboard();
        return;
    }
    QTableWidget::keyPressEvent(event);
}
