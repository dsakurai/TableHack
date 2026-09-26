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

    auto* model = new QStandardItemModel(3, 3);

    tableView->setModel(model);

    // Button to copy HTML table to clipboard (keeps existing behavior)
    QPushButton *btn = new QPushButton("Copy HTML table (Win32 CF_HTML v2)", &window);
    QObject::connect(btn, &QPushButton::clicked, [model]() {
        // Build HTML table from model
        QString htmlTable = "<table>";
        for (int r = 0; r < model->rowCount(); ++r) {
            htmlTable += "<tr>";
            for (int c = 0; c < model->columnCount(); ++c) {
                QString cell = model->item(r, c) ? model->item(r, c)->text() : QString();
                htmlTable += "<td>" + cell.toHtmlEscaped() + "</td>";
            }
            htmlTable += "</tr>";
        }
        htmlTable += "</table>";

        // Plain text (tab-separated)
        QString plainText;
        for (int r = 0; r < model->rowCount(); ++r) {
            QStringList row;
            for (int c = 0; c < model->columnCount(); ++c)
                row << (model->item(r, c) ? model->item(r, c)->text() : QString());
            plainText += row.join('\t');
            if (r < model->rowCount() - 1)
                plainText += "\r\n";
        }

        copyHtmlToClipboardWin32(htmlTable, plainText);
    });

    layout->addWidget(tableView);
    layout->addWidget(btn);

    window.resize(400, 300);
    window.show();

    return app.exec();
}