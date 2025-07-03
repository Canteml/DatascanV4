/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : Location1d.h
# Author      : Thierry Lechevallier
# Date        : 09/09/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
class AScan;
class Surface1d;
class Surface1dData;
class Surface1dView;
class Zone1d;
class ExamManager;
class DisplaySurface;

#include <v2/vector/VectorStd.h>

// -- Qt Includes
#include <QObject>
#include <QThread>

/**
 * @brief The Location class
 *
 * One piece can have several Locations.
 * One location is attached to a scan done by a vector.
 * The scan can be a Generatrix, a Punctual Thickness Measurement, CSCAN? ...
 *
 * The main goal is to store all AScans and propagate them to GUI.
 *
 * One sensor is attached to one surface. We can manage 4 sensors for now.
 *
 * ASCAN are catched directly from VectorFuncAScanX
 */
class Location1d : public QObject, public Loggable {
    Q_OBJECT
public :
    Location1d(const QString& p_name);
    ~Location1d();

    void clearScan();  ///< remove all data

    QString getName() const;
    void setName(const QString & p_value);

    bool isRecording(void) const;
    void setRecord(bool p_value);

    void deleteStoredAScan(AScan* p_pAScan);  ///< request a delete of this AScan (by a Cell)

    void pushAScan(AScan*);

    void zoneAddToCurrentPosition();
    void zoneAddToPosition(qreal p_posMm);
    void zoneFixBounds();
    void zoneRemoveLast();
    const QList<Zone1d*>* getZones(void) const;
    Surface1d* getSurface(qint32 p_index) const;
    DisplaySurface* getDisplaySurface(qint32 p_index) const;

    const qreal DISTANCE_MIN_ZONE_MM = 50;

signals :
    void evTreatAScan();
    void evRequestSurfaceUpdate(Surface1d*);
    void evLocationZoneChanged(Location1d*);    ///< signal zone changed

private slots :
    void onTreatAScan();
    void onMeasureDisplayChanged();

private :
    void resetAutoZone();

    // -- Properties
    QString m_name; ///< name of the generatrice
    QList<Zone1d*> m_lZone1d;               ///< List of Zone sorted by growing Origin value.
    QList<Zone1d*> m_lZone1dAppendOrder;    ///< List of Zone sorted by appending time (first is older)

    // -- Behaviour
    QList <AScan*> m_lAScanPending;                     ///< AScan to process
    QMutex m_mAScanPending;
    QList <AScan*> m_lAScanStored;                      ///< AScan stored
    QMutex m_mAScanStored;
    QMutex m_mutTreatAScan;

    bool m_zoneAutoStartFound;
    qreal m_zoneAutoStartPosMm;
    QList<qreal> m_zoneAutoHoleEndPos;  ///< position of contigus peaks after a hole detection

    Surface1d* m_aSurface1d[VectorStd::NB_FUNC_ASCAN];  ///< make BSCAN1D
    DisplaySurface* m_aDisplaySurface1d[VectorStd::NB_FUNC_ASCAN];

    bool m_isRecording;             ///< if recording, new ASCAN are stored here
    QThread * m_pThread;
    volatile bool m_isProcessingAScan;

    ExamManager * m_pExamManager;
};


