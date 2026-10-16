/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExportCsv.h
# Author      : Thierry Lechevallier
# Date        : 29/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
class AScan;

// -- Qt Includes
#include <QObject>
#include <QThread>
#include <QFile>
#include <QTextStream>

/**
 * @brief The ExportCsv class
 *
 * Streaming CSV writer for a BSCAN acquisition : the file is created (with its header)
 * when the acquisition starts, then one line is appended for each ASCAN received.
 * All file accesses are done in a dedicated thread; requests are queued so they are
 * processed in the order they were emitted (open -> lines -> close).
 */
class ExportCsv : public QObject, public Loggable
{
    Q_OBJECT

public :
    ExportCsv();
    ~ExportCsv();

    void open(const QString& p_fileName);   ///< create the file and write the header
    void appendAScan(AScan* p_pAScan);      ///< format one line from the ASCAN (caller thread) and queue it
    void close();                           ///< close the file once all queued lines are written
    QString getFileName() const;

signals :
    void evError(QString);                  ///< emitted when the file can't be created or written

    void evPrivateOpen(QString);
    void evPrivateWriteLine(QString);
    void evPrivateClose();

private slots :
    void onPrivateOpen(QString p_fileName);
    void onPrivateWriteLine(QString p_line);
    void onPrivateClose();

private :

    QString m_fileName;
    QFile m_file;
    QTextStream m_flux;

    QThread * m_pThread;
};

