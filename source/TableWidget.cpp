#include "TableWidget.h"

#include <QKeyEvent>

TableWidget::TableWidget(QWidget *parent)
    : MinimalTableWidget(parent) {
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
