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
    if (!m)
        return;

    QString htmlTable = "<table>";
    for (int r = 0; r < m->rowCount(); ++r) {
        htmlTable += "<tr>";
        for (int c = 0; c < m->columnCount(); ++c) {
            QModelIndex idx = m->index(r, c);
            QString cell = m->data(idx, Qt::DisplayRole).toString();
            htmlTable += "<td>" + cell.toHtmlEscaped() + "</td>";
        }
        htmlTable += "</tr>";
    }
    htmlTable += "</table>";

    QString plainText;
    for (int r = 0; r < m->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < m->columnCount(); ++c) {
            QModelIndex idx = m->index(r, c);
            row << m->data(idx, Qt::DisplayRole).toString();
        }
        plainText += row.join('\t');
        if (r < m->rowCount() - 1)
            plainText += "\r\n";
    }

    copyHtmlToClipboardWin32(htmlTable, plainText);
}
