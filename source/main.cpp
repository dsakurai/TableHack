#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTableView>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QTextStream>
#include <QRect>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QKeySequence>
#include <QAbstractItemView>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <cstring>

// Writes CF_HTML/CF_UNICODETEXT via the raw Win32 clipboard API.
// Needed because browsers read the clipboard through GetClipboardData,
// which does not reliably bridge to Qt's OLE-only "text/html" mime data.
static bool writeHtmlToWindowsClipboard(HWND hwnd, const QString &html, const QString &plainText)
{
    const QByteArray startMarker = "<!--StartFragment-->";
    const QByteArray endMarker = "<!--EndFragment-->";
    const QByteArray body = html.toUtf8();

    static const char *headerFmt =
        "Version:1.0\r\n"
        "StartHTML:%010d\r\n"
        "EndHTML:%010d\r\n"
        "StartFragment:%010d\r\n"
        "EndFragment:%010d\r\n";

    char headerBuf[128];
    const int headerLen = std::snprintf(headerBuf, sizeof(headerBuf), headerFmt, 0, 0, 0, 0);
    const int startHtml = headerLen;
    const int startFragment = startHtml + startMarker.size();
    const int endFragment = startFragment + body.size();
    const int endHtml = endFragment + endMarker.size();
    std::snprintf(headerBuf, sizeof(headerBuf), headerFmt, startHtml, endHtml, startFragment, endFragment);

    const QByteArray payload = QByteArray(headerBuf) + startMarker + body + endMarker;

    const UINT cfHtml = RegisterClipboardFormatA("HTML Format");
    if (cfHtml == 0 || !OpenClipboard(hwnd))
        return false;

    EmptyClipboard();

    bool ok = false;
    if (HGLOBAL hHtml = GlobalAlloc(GMEM_MOVEABLE, payload.size() + 1)) {
        if (void *dst = GlobalLock(hHtml)) {
            memcpy(dst, payload.constData(), payload.size());
            static_cast<char *>(dst)[payload.size()] = '\0';
            GlobalUnlock(hHtml);
            ok = SetClipboardData(cfHtml, hHtml) != nullptr;
        }
    }

    const std::wstring wtext = plainText.toStdWString();
    const size_t textBytes = (wtext.size() + 1) * sizeof(wchar_t);
    if (HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, textBytes)) {
        if (void *dst = GlobalLock(hText)) {
            memcpy(dst, wtext.c_str(), textBytes);
            GlobalUnlock(hText);
            SetClipboardData(CF_UNICODETEXT, hText);
        }
    }

    CloseClipboard();
    return ok;
}
#endif

static QString selectionToHtmlTable(QTableView *view)
{
    auto *model = view->model();
    if (!model) return {};

    auto selection = view->selectionModel()->selection();
    if (selection.isEmpty()) return {};

    // For simplicity, handle only the first continuous range
    const QItemSelectionRange &range = selection.first();

    QString html;
    QTextStream out(&html);

    out << "<table>\n";

    for (int row = range.top(); row <= range.bottom(); ++row) {
        out << "  <tr>\n";
        for (int col = range.left(); col <= range.right(); ++col) {
            QModelIndex idx = model->index(row, col, view->rootIndex());
            QVariant data = model->data(idx, Qt::DisplayRole);
            QString text = data.toString().toHtmlEscaped();

            out << "    <td>" << text << "</td>\n";
        }
        out << "  </tr>\n";
    }

    out << "</table>";
    return html;
}

class HtmlCopyTableView : public QTableView
{
    Q_OBJECT
public:
    explicit HtmlCopyTableView(QWidget *parent = nullptr)
        : QTableView(parent)
    {
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->matches(QKeySequence::Copy)) {
            copySelectionAsHtml();
            return;
        }
        QTableView::keyPressEvent(event);
    }

private:
    void copySelectionAsHtml()
    {
        QString htmlTable = selectionToHtmlTable(this);
        if (htmlTable.isEmpty())
            return;

        // Plain text fallback (tab-separated, newline per row)
        QString plain = selectionToPlainText(this);

#ifdef Q_OS_WIN
        if (writeHtmlToWindowsClipboard(reinterpret_cast<HWND>(winId()), htmlTable, plain))
            return;
#endif

        auto *mime = new QMimeData;
        mime->setText(plain);

        // Set HTML format for apps that support it (Excel, Word, browsers, etc.)
        // On Windows this uses CF_HTML; on other platforms it's "text/html".
        mime->setData("text/html", htmlTable.toUtf8());

        QApplication::clipboard()->setMimeData(mime);
    }

    static QString selectionToPlainText(QTableView *view)
    {
        auto *model = view->model();
        if (!model) return {};

        auto selection = view->selectionModel()->selection();
        if (selection.isEmpty()) return {};

        const QItemSelectionRange &range = selection.first();

        QString text;
        QTextStream out(&text);

        for (int row = range.top(); row <= range.bottom(); ++row) {
            for (int col = range.left(); col <= range.right(); ++col) {
                QModelIndex idx = model->index(row, col, view->rootIndex());
                out << model->data(idx, Qt::DisplayRole).toString();
                if (col != range.right())
                    out << '\t';
            }
            if (row != range.bottom())
                out << '\n';
        }
        return text;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStandardItemModel *model = new QStandardItemModel(5, 4);
    for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 4; ++c)
            model->setItem(r, c, new QStandardItem(QString("R%1C%2").arg(r).arg(c)));

    HtmlCopyTableView view;
    view.setModel(model);
    view.setSelectionBehavior(QAbstractItemView::SelectItems);
    view.setSelectionMode(QAbstractItemView::ContiguousSelection);
    view.resize(400, 300);
    view.show();

    return app.exec();
}

#include "main.moc"