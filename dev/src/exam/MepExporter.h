#pragma once

#include <QObject>
#include <log/Loggable.h>

class MepExporter : public QObject, public Loggable
{
    Q_OBJECT

public:

    MepExporter();

public slots:

    Q_INVOKABLE QString startExport(const QString &p_fileName);

signals:

    void evExportFinished();

};
