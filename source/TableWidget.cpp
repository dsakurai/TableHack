#include "TableWidget.h"

#include <QKeyEvent>
#include <QHeaderView>
#include <QMenu>

TableWidget::TableWidget(QWidget *parent)
    : MinimalTableWidget(parent) {
    // provide context menu on horizontal header for column actions
    if (QHeaderView *h = horizontalHeader()) {
        h->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(h, &QHeaderView::customContextMenuRequested, this, &TableWidget::showHorizontalHeaderContextMenu);
    }

    // context menu for vertical header (rows)
    if (QHeaderView *vh = verticalHeader()) {
        vh->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(vh, &QHeaderView::customContextMenuRequested, this, &TableWidget::showVerticalHeaderContextMenu);
    }
}

void TableWidget::keyPressEvent(QKeyEvent *event)
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

void TableWidget::showHorizontalHeaderContextMenu(const QPoint &pos)
{
    QHeaderView *h = horizontalHeader();
    if (!h) return;

    QMenu menu(h);

    connect(menu.addAction(tr("Insert column before")), &QAction::triggered, this,
            [this, pos]() { insertLine(RowOrColumn::Column, BeforeOrAfter::Before, pos); });

    connect(menu.addAction(tr("Insert column after")), &QAction::triggered, this,
            [this, pos]() { insertLine(RowOrColumn::Column, BeforeOrAfter::After, pos); });

    connect(menu.addAction(tr("Remove selected columns")), &QAction::triggered, this,
            [this]() { removeSelectedLine(RowOrColumn::Column); });

    // Show the context menu at the position of the cursor.
    menu.exec(h->mapToGlobal(pos));
}


void TableWidget::showVerticalHeaderContextMenu(const QPoint &pos)
{
    QHeaderView *h = verticalHeader();
    if (!h) return;
    
    QMenu menu(h);
    
    connect(menu.addAction(tr("Insert row before")), &QAction::triggered, this,
            [this, pos]() { insertLine(RowOrColumn::Row, BeforeOrAfter::Before, pos); });

    connect(menu.addAction(tr("Insert row after")), &QAction::triggered, this,
            [this, pos]() { insertLine(RowOrColumn::Row, BeforeOrAfter::After, pos); });
    connect(menu.addAction(tr("Remove selected rows")), &QAction::triggered, this,
            [this]() { removeSelectedLine(RowOrColumn::Row); });

    // Show the context menu at the position of the cursor.
    menu.exec(h->mapToGlobal(pos));
}
