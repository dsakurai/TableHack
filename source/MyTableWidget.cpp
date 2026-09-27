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
#include <QPointer>
#include <QSet>
#include <algorithm>

MyTableWidget::MyTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    // Move rows and columns within the table.
    horizontalHeader()->setSectionsMovable(true);
    horizontalHeader()->setDragEnabled(true);
    //
    verticalHeader()->setSectionsMovable(true);
    verticalHeader()->setDragEnabled(true);

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

class VisibleIndex {
public:
    VisibleIndex(const QModelIndex& idx, const MyTableWidget *w)
        : index_(idx), tableWidget_(w) {}
    
    int row() const { 
        return tableWidget_->verticalHeader()->visualIndex(index_.row());
    }
    int column() const {
        return tableWidget_->horizontalHeader()->visualIndex(index_.column());
    }
private:
    const QModelIndex index_;
    const QPointer<const MyTableWidget> tableWidget_;
};

void MyTableWidget::copyCells(const QModelIndexList &idxs) const
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

void MyTableWidget::copySelectionToClipboard()
{
    QItemSelectionModel *sel = selectionModel();
    if (!sel)
        return;
    copyCells(sel->selectedIndexes());
}

void MyTableWidget::cutSelectionToClipboard()
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

void MyTableWidget::pasteFromClipboard()
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
