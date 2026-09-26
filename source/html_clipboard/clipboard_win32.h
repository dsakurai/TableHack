#pragma once

#include <QByteArray>
#include <QString>

QByteArray makeHtmlClipboardData(const QString &htmlFragment);
void copyHtmlToClipboardWin32(const QString &htmlFragment, const QString &plainText);
