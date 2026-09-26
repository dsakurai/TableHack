#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QClipboard>
#include <QMimeData>
#include <QKeySequence>
#include <QAction>
#include <QMenu>

static void copySelectionToClipboard(QTableView *view)
{
    QItemSelectionModel *sel = view->selectionModel();
    if (!sel || !sel->hasSelection())
        return;

    QModelIndexList indexes = sel->selectedIndexes();
    std::sort(indexes.begin(), indexes.end());

    QString tsv;
    QString html;
    html += "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body>";
    html += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\">";

    int currentRow = -1;
    for (const QModelIndex &idx : indexes) {
        if (idx.row() != currentRow) {
            if (currentRow != -1) {
                if (tsv.endsWith('\t'))
                    tsv.chop(1);
                html += "</tr>";
            }
            html += "<tr>";
            if (!tsv.isEmpty())
                tsv += '\n';
            currentRow = idx.row();
        }
        QString cell = idx.data().toString();
        tsv += cell;
        tsv += '\t';
        html += "<td>" + (cell.isEmpty() ? QStringLiteral("&nbsp;") : cell.toHtmlEscaped()) + "</td>";
    }
    if (currentRow != -1) {
        if (tsv.endsWith('\t'))
            tsv.chop(1);
        html += "</tr>";
    }
    html += "</table></body></html>";

    QMimeData *md = new QMimeData;
    md->setHtml(html);
    md->setText(tsv);
    QClipboard *cb = QApplication::clipboard();
    cb->setMimeData(md, QClipboard::Clipboard);
}

static void pasteFromClipboard(QTableView *view)
{
    QClipboard *cb = QApplication::clipboard();
    QString text = cb->text(QClipboard::Clipboard);
    if (text.isEmpty())
        return;

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(view->model());
    if (!model)
        return;

    QModelIndex start = view->currentIndex();
    int row = start.isValid() ? start.row() : 0;
    int col = start.isValid() ? start.column() : 0;

    QStringList rows = text.split('\n');
    for (int r = 0; r < rows.size(); ++r) {
        QStringList cols = rows[r].split('\t');
        for (int c = 0; c < cols.size(); ++c) {
            int tr = row + r;
            int tc = col + c;
            if (tr >= model->rowCount() || tc >= model->columnCount())
                continue;
            QStandardItem *item = model->item(tr, tc);
            if (!item) {
                item = new QStandardItem;
                model->setItem(tr, tc, item);
            }
            item->setText(cols[c]);
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTableView view;
    QStandardItemModel model(8, 4); // default size

    // Fill model with sample data
    for (int row = 0; row < model.rowCount(); ++row) {
        for (int col = 0; col < model.columnCount(); ++col) {
            QStandardItem *item = new QStandardItem(QString("R%1C%2").arg(row).arg(col));
            model.setItem(row, col, item);
        }
    }

    view.setModel(&model);
    view.setWindowTitle("QTableView Example");
    view.resize(800, 600);
    view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    view.setSelectionBehavior(QAbstractItemView::SelectItems);
    view.horizontalHeader()->setStretchLastSection(true);

    // Actions for copy/paste
    QAction *copyAct = new QAction(&view);
    copyAct->setShortcut(QKeySequence::Copy);
    QObject::connect(copyAct, &QAction::triggered, [&view]() { copySelectionToClipboard(&view); });
    view.addAction(copyAct);

    QAction *pasteAct = new QAction(&view);
    pasteAct->setShortcut(QKeySequence::Paste);
    QObject::connect(pasteAct, &QAction::triggered, [&view]() { pasteFromClipboard(&view); });
    view.addAction(pasteAct);

    // Context menu with copy/paste
    view.setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(&view, &QWidget::customContextMenuRequested, [&view](const QPoint &pos){
        QMenu menu(&view);
        menu.addAction(view.actions().at(0)); // copy
        menu.addAction(view.actions().at(1)); // paste
        menu.exec(view.viewport()->mapToGlobal(pos));
    });

    view.show();

    return app.exec();
}