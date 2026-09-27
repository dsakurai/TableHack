#include "MyTableWidget.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QStringList>
#include <clipboard_win32.h>
#include <QClipboard>
#include <QApplication>
#include <QKeyEvent>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QSet>
#include <algorithm>

MyTableWidget::MyTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    // provide context menu on horizontal header for column actions
    QHeaderView *h = horizontalHeader();
    if (h) {
        h->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(h, &QHeaderView::customContextMenuRequested, this, &MyTableWidget::showHeaderContextMenu);
    }

    // context menu for vertical header (rows)
    QHeaderView *vh = verticalHeader();
    if (vh) {
        vh->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(vh, &QHeaderView::customContextMenuRequested, this, &MyTableWidget::showVerticalHeaderContextMenu);
    }
}

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

void MyTableWidget::showHeaderContextMenu(const QPoint &pos)
{
    QHeaderView *h = horizontalHeader();
    if (!h)
        return;
    int section = h->logicalIndexAt(pos);
    QMenu menu(h);
    QAction *insertBefore = menu.addAction(tr("Insert column before"));
    QAction *insertAfter = menu.addAction(tr("Insert column after"));
    QAction *removeAction = menu.addAction(tr("Remove selected columns"));
    connect(insertBefore, &QAction::triggered, this, [this, section]() { insertColumnAt(qMax(0, section)); });
    connect(insertAfter, &QAction::triggered, this, [this, section]() { insertColumnAt(section + 1); });
    connect(removeAction, &QAction::triggered, this, &MyTableWidget::removeSelectedColumns);
    menu.exec(h->mapToGlobal(pos));
}

void MyTableWidget::removeSelectedLines(Line line)
{
    QItemSelectionModel *sel = selectionModel();
    if (!sel) return;

    QSet<int> columns; // either column coordinates or row coordinates. Regarding the variable naming, we just assume it's columns

    for (const QModelIndex &cell : sel->selectedIndexes()) {
        if (line == Line::Columns)
            columns.insert(cell.column());
        if (line == Line::Rows)
            columns.insert(cell.row());
    }
    if (columns.isEmpty())
        return;

    QList<int> columnList = columns.values();
    std::sort(columnList.begin(), columnList.end(), std::greater<int>());
    for (int c : columnList)
        if (line == Line::Columns)
            removeColumn(c);
        else if (line == Line::Rows)
            removeRow(c);
}

void MyTableWidget::removeSelectedColumns()
{
    removeSelectedLines(Line::Columns);
}

void MyTableWidget::showVerticalHeaderContextMenu(const QPoint &pos)
{
    QHeaderView *h = verticalHeader();
    if (!h)
        return;
    int section = h->logicalIndexAt(pos);
    QMenu menu(h);
    QAction *insertBefore = menu.addAction(tr("Insert row before"));
    QAction *insertAfter = menu.addAction(tr("Insert row after"));
    QAction *removeAction = menu.addAction(tr("Remove selected rows"));
    connect(insertBefore, &QAction::triggered, this, [this, section]() { insertRowAt(qMax(0, section)); });
    connect(insertAfter, &QAction::triggered, this, [this, section]() { insertRowAt(section + 1); });
    connect(removeAction, &QAction::triggered, this, &MyTableWidget::removeSelectedRows);
    menu.exec(h->mapToGlobal(pos));
}

void MyTableWidget::removeSelectedRows()
{
    removeSelectedLines(Line::Rows);
}

void MyTableWidget::insertColumnAt(int index)
{
    // prefer selection-based insertion: if selection present, adjust index
    QItemSelectionModel *sel = selectionModel();
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        if (!idxs.isEmpty()) {
            int minc = INT_MAX, maxc = INT_MIN;
            for (const QModelIndex &idx : idxs) {
                minc = qMin(minc, idx.column());
                maxc = qMax(maxc, idx.column());
            }
            // if index equals clicked position, keep it; otherwise use provided index
            Q_UNUSED(minc);
            Q_UNUSED(maxc);
        }
    }
    insertColumn(qBound(0, index, columnCount()));
}

void MyTableWidget::insertRowAt(int index)
{
    QItemSelectionModel *sel = selectionModel();
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        if (!idxs.isEmpty()) {
            int minr = INT_MAX, maxr = INT_MIN;
            for (const QModelIndex &idx : idxs) {
                minr = qMin(minr, idx.row());
                maxr = qMax(maxr, idx.row());
            }
            Q_UNUSED(minr);
            Q_UNUSED(maxr);
        }
    }
    insertRow(qBound(0, index, rowCount()));
}

// Backwards-compatible wrappers (old slot names)
void MyTableWidget::insertColumnLeft()
{
    // insert before current selection if any, else at front
    QItemSelectionModel *sel = selectionModel();
    int index = 0;
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        int minc = INT_MAX;
        for (const QModelIndex &idx : idxs)
            minc = qMin(minc, idx.column());
        if (minc != INT_MAX)
            index = minc;
    }
    insertColumnAt(index);
}

void MyTableWidget::insertColumnRight()
{
    // insert after selection if any, else at end
    QItemSelectionModel *sel = selectionModel();
    int index = columnCount();
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        int maxc = -1;
        for (const QModelIndex &idx : idxs)
            maxc = qMax(maxc, idx.column());
        if (maxc >= 0)
            index = maxc + 1;
    }
    insertColumnAt(index);
}

void MyTableWidget::insertRowAbove()
{
    QItemSelectionModel *sel = selectionModel();
    int index = 0;
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        int minr = INT_MAX;
        for (const QModelIndex &idx : idxs)
            minr = qMin(minr, idx.row());
        if (minr != INT_MAX)
            index = minr;
    }
    insertRowAt(index);
}

void MyTableWidget::insertRowBelow()
{
    QItemSelectionModel *sel = selectionModel();
    int index = rowCount();
    if (sel) {
        QModelIndexList idxs = sel->selectedIndexes();
        int maxr = -1;
        for (const QModelIndex &idx : idxs)
            maxr = qMax(maxr, idx.row());
        if (maxr >= 0)
            index = maxr + 1;
    }
    insertRowAt(index);
}
