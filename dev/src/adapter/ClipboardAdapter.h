/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ClipboardAdapter.h
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

#pragma once

#include <QObject>
#include <QClipboard>

class ClipboardAdapter : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)

public:

    explicit ClipboardAdapter(QObject *parent = nullptr);

    QString getText();
    void setText(QString text);

signals:

    void textChanged();

private:

    QClipboard *m_clipboard;

};
