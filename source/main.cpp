#include <QApplication>
#include <QPushButton>
#include <QString>
#include <clipboard_win32.h>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPushButton btn("Copy HTML table (Win32 CF_HTML v2)");
    QObject::connect(&btn, &QPushButton::clicked, []() {
        QString htmlTable =
            "<table border=\"1\">"
            "<tr><td>A1</td><td>B1</td></tr>"
            "<tr><td>A2</td><td>B2</td></tr>"
            "</table>";

        QString plainText = "A1\tB1\r\nA2\tB2";

        copyHtmlToClipboardWin32(htmlTable, plainText);
    });

    btn.resize(300, 60);
    btn.show();

    return app.exec();
}