#include <QApplication>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <QVBoxLayout>
#include "TableWidget.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Main window
    QWidget window;
    window.setWindowTitle("Example: Table View + Clipboard");

    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Table widget with sample data (use QTableWidget for simple headers)
    auto *tableWidget = new TableWidget(&window);
    tableWidget->setRowCount(3);
    tableWidget->setColumnCount(3);

    QPushButton *btn = new QPushButton("Copy HTML table (Win32 CF_HTML v2)", &window);

    layout->addWidget(tableWidget);
    layout->addWidget(btn);

    window.resize(400, 300);
    window.show();

    return app.exec();
}