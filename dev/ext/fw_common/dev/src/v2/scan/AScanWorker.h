/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AScan.h
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <log/Loggable.h>
#include <v2/scan/AScan.h>

// -- Qt Includes
#include <QObject>
#include <QThread>
#include <QVector>
/**
 * @brief The AScanWorker class
 *
 * This class process a ASCAN :
 *  - compute enhanced sample to improve signal
 *  - compute measures (Multiple echos and one echo)
 *
 */
class AScanWorker : public QObject, public Loggable
{
    Q_OBJECT
public :
    AScanWorker();
    ~AScanWorker();
    bool isBusy();
    void pushAScanToTreat(AScan *);
    AScan* takeAScanTreated();
    bool hasAscanTreated();
    void clearAScan();

signals :
    void evAScanTreated();
    void evPrivateTreatAScan();

private slots :
    void onPrivateTreatAScan();

private :

    void computeEnhancedAScan();
    void computeMeasures();
    void computeEnhancedAScanRF();

    // Behaviour
    volatile bool m_isBusy;  ///< if true, do not try to start an other treatment
    AScan* m_pAScanTreated;
    QThread* m_pThread;
    volatile qint8 m_clearPipe; ///< if true, we are cleaning ASCAN pipe, so don't treat last ASCAN
    QList<AScan*> m_lAscanToTreat;  ///< in
    QList<AScan*> m_lAscanTreated;  ///< out
    QMutex m_lAScan;
    QVector<qreal> computeRFCoefficients(int p_lissage, qreal p_sampFreq);
};


