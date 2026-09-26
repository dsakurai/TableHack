#include <QApplication>
#include <QPushButton>
#include <QByteArray>
#include <QString>
#include <windows.h>
#include <string>

static QByteArray makeHtmlClipboardData(const QString &htmlFragment)
{
    const QString fullHtml =
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
        "<HTML>\n"
        "<BODY>\n"
        "<!--StartFragment-->" + htmlFragment + "<!--EndFragment-->\n"
        "</BODY>\n"
        "</HTML>\n";

    const QByteArray utf8 = fullHtml.toUtf8();

    int startFragment = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n<HTML>\n<BODY>\n<!--StartFragment-->").toUtf8().size();
    int endFragment   = startFragment + htmlFragment.toUtf8().size();

    QString header =
        "Version:0.9\r\n"
        "StartHTML:%1\r\n"
        "EndHTML:%2\r\n"
        "StartFragment:%3\r\n"
        "EndFragment:%4\r\n"
        "StartSelection:%3\r\n"
        "EndSelection:%4\r\n";

    header = header.arg(0, 10, 10, QChar('0'))
                   .arg(utf8.size(), 10, 10, QChar('0'))
                   .arg(startFragment, 10, 10, QChar('0'))
                   .arg(endFragment, 10, 10, QChar('0'));

    QByteArray headerUtf8 = header.toUtf8();

    QByteArray result;
    result.reserve(headerUtf8.size() + utf8.size());
    result.append(headerUtf8);
    result.append(utf8);
    return result;
}

static void copyHtmlToClipboardWin32(const QString &htmlFragment, const QString &plainText)
{
    QByteArray cfHtml = makeHtmlClipboardData(htmlFragment);

    // Convert plainText to UTF-16 LE (Windows wide string) without BOM
    std::wstring wtext = plainText.toStdWString();

    if (!OpenClipboard(nullptr)) {
        qWarning("Failed to open clipboard");
        return;
    }

    if (!EmptyClipboard()) {
        CloseClipboard();
        qWarning("Failed to empty clipboard");
        return;
    }

    UINT cfHtmlFormat = RegisterClipboardFormatA("HTML Format");
    if (!cfHtmlFormat) {
        CloseClipboard();
        qWarning("Failed to register HTML Format");
        return;
    }

    // CF_HTML
    HGLOBAL hHtml = GlobalAlloc(GMEM_MOVEABLE, cfHtml.size());
    if (!hHtml) {
        CloseClipboard();
        qWarning("GlobalAlloc failed for HTML");
        return;
    }
    void *pHtml = GlobalLock(hHtml);
    if (!pHtml) {
        GlobalFree(hHtml);
        CloseClipboard();
        qWarning("GlobalLock failed for HTML");
        return;
    }
    memcpy(pHtml, cfHtml.constData(), cfHtml.size());
    GlobalUnlock(hHtml);

    if (!SetClipboardData(cfHtmlFormat, hHtml)) {
        GlobalFree(hHtml);
        CloseClipboard();
        qWarning("SetClipboardData failed for HTML");
        return;
    }

    // CF_UNICODETEXT
    size_t bytes = wtext.size() * sizeof(wchar_t);
    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, bytes + sizeof(wchar_t)); // + null
    if (!hText) {
        CloseClipboard();
        qWarning("GlobalAlloc failed for text");
        return;
    }
    void *pText = GlobalLock(hText);
    if (!pText) {
        GlobalFree(hText);
        CloseClipboard();
        qWarning("GlobalLock failed for text");
        return;
    }
    memcpy(pText, wtext.data(), bytes);
    ((wchar_t*)pText)[wtext.size()] = L'\0';
    GlobalUnlock(hText);

    if (!SetClipboardData(CF_UNICODETEXT, hText)) {
        GlobalFree(hText);
        CloseClipboard();
        qWarning("SetClipboardData failed for text");
        return;
    }

    CloseClipboard();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPushButton btn("Copy HTML table (Win32 CF_HTML)");
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