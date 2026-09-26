#include <QApplication>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QVBoxLayout>
#include "MyTableView.h"
#include <QStandardItemModel>
#include <clipboard_win32.h>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("Example: Table View + Clipboard");

    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Table view with sample data (subclassed and moved to MyTableView)
    MyTableView *tableView = new MyTableView(&window);

    // Button to copy HTML table to clipboard
    tableView->setModel(new QStandardItemModel(3, 3));

    QPushButton *btn = new QPushButton("Copy HTML table (Win32 CF_HTML v2)", &window);
    QObject::connect(btn, &QPushButton::clicked, tableView, &MyTableView::copyAllToClipboard);

    layout->addWidget(tableView);
    layout->addWidget(btn);

    window.resize(400, 300);
    window.show();

    return app.exec();
}