/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ClipboardAdapter.cpp
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

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
