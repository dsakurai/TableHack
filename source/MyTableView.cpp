#include "MyTableView.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemModel>
#include <QStringList>
#include <clipboard_win32.h>

MyTableView::MyTableView(QWidget *parent)
    : QTableView(parent)
{ }

void MyTableView::copyAllToClipboard()
{
    QAbstractItemModel *m = model();
    if (!m) return;
    copyCells(0, m->rowCount() - 1, 0, m->columnCount() - 1);
}

void MyTableView::copyCells(int r0, int r1, int c0, int c1) const
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
