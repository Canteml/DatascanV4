#include "ClipboardAdapter.h"

#include <QApplication>
#include <QClipboard>

ClipboardAdapter::ClipboardAdapter(QObject *parent)
    : QObject{parent}
{
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipboardAdapter::textChanged);
}

QString ClipboardAdapter::getText()
{
    return m_clipboard->text();
}

void ClipboardAdapter::setText(QString text)
{
    m_clipboard->setText(text);
}
