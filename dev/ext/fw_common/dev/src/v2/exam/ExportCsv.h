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
class Location1d;
class Surface1d;

// -- Qt Includes
#include <QObject>
#include <QThread>

class ExportCsv : public QObject, public Loggable
{
    Q_OBJECT       

public :
    ExportCsv();
    ~ExportCsv();

    void startExport(const QString& p_fileName);
    void cancelExport();
    QString getFileName() const;

signals :
    void evProgessChanged(qint32);
    void evExportTerminated(bool);      ///< signaled at the end of the write, after an error => state in argument
    void evPrivateStartExport();

private slots :
    void onPrivateStartExport();

private :

    QString m_fileName;
    volatile bool m_requestCancel;

    QThread * m_pThread;
};

