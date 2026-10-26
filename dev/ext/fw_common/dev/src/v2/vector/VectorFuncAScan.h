/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFunc.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>
#include <v2/scan/AScan.h>
#include <QMetaType>
#include <QObject>

class AScanWorker;

// ----------------------------------------------------------------------------
// -- VectorFuncAScan

/**
 * @brief The VectorFuncAScan class
 */
class VectorFuncAScan : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DGain, quint8, 1, FUNC_ASCAN_PARAM_D_GAIN, 10)
    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DVoltage, quint8, 1, FUNC_ASCAN_PARAM_D_VOLTAGE, 100)
    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DUtElement, quint8, 1, FUNC_ASCAN_PARAM_D_UT_ELEMENT, 0)
    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DSampleFreq, quint8, 1, FUNC_ASCAN_PARAM_D_SAMPLE_FREQ, 0)
    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DPulseWidth, quint16, 2, FUNC_ASCAN_PARAM_D_PULSE_WIDTH, 0)


    FUNC_PARAM_PRE(m_ascan.getSensorSettings()->m_param, LCelerity, quint32, 4, FUNC_ASCAN_PARAM_L_CELERITY, 5940)
    FUNC_PARAM_PRE(m_ascan.getAScanMeasure()->m_param, LProbeDelayMm, qreal, 8, FUNC_ASCAN_PARAM_L_PROBE_DELAY, 0)

    FUNC_PARAM_PRE(m_ascan.getAScanEnhancedSample()->m_param, LLowPassFilter, quint8, 1, FUNC_ASCAN_PARAM_L_LOWPASS_FILTER, 0)

    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates0->m_param, LGate0Start, LStart, qreal, 8, FUNC_ASCAN_PARAM_L_GATE0_START, 100)
    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates0->m_param, LGate0Width, LWidth, qreal, 8, FUNC_ASCAN_PARAM_L_GATE0_WIDTH, 200)
    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates0->m_param, LGate0Height, LHeight, qreal, 8, FUNC_ASCAN_PARAM_L_GATE0_HEIGHT, 20)

    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates1->m_param, LGate1Start, LStart, qreal, 8, FUNC_ASCAN_PARAM_L_GATE1_START, 300)
    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates1->m_param, LGate1Width, LWidth, qreal, 8, FUNC_ASCAN_PARAM_L_GATE1_WIDTH, 200)
    FUNC_PARAM_PRE_P_D(m_ascan.getAScanMeasure()->m_paramGates1->m_param, LGate1Height, LHeight, qreal, 8, FUNC_ASCAN_PARAM_L_GATE1_HEIGHT, 40)

    // Option AutoGain function
    Q_PROPERTY(bool paramDAutoGain READ paramDAutoGain WRITE setParamDAutoGain NOTIFY paramDAutoGainChanged)
    Q_PROPERTY(qint16 paramDAutoGainThreshold READ paramDAutoGainThreshold WRITE setParamDAutoGainThreshold NOTIFY paramDAutoGainThresholdChanged)
    Q_PROPERTY(quint16 paramLGateNoiseStart READ paramLGateNoiseStart WRITE setParamLGateNoiseStart NOTIFY paramLGateNoiseStartChanged)
    Q_PROPERTY(quint16 paramLGateNoiseWidth READ paramLGateNoiseWidth WRITE setParamLGateNoiseWidth NOTIFY paramLGateNoiseWidthChanged)

    // UsKey Mode Params
    Q_PROPERTY(quint8 paramDFilterMode READ getParamDFilterMode WRITE setParamDFilterMode NOTIFY paramDFilterModeChanged)
    Q_PROPERTY(quint8 paramDWaveType   READ getParamDWaveType   WRITE setParamDWaveType   NOTIFY paramDWaveTypeChanged)

    // Option Parametrization des portes 1 et 2
    Q_PROPERTY(quint8 paramDGate0Option   READ getParamDGate0Option   WRITE setParamDGate0Option   NOTIFY paramDGate0OptionChanged)
    Q_PROPERTY(quint8 paramDGate1Option   READ getParamDGate1Option   WRITE setParamDGate1Option   NOTIFY paramDGate1OptionChanged)

    // Option Auto Sync Porte : la porte 2 se cale automatiquement sur le 2e echo de la porte 1
    Q_PROPERTY(bool paramDAutoSyncGates READ paramDAutoSyncGates WRITE setParamDAutoSyncGates NOTIFY paramDAutoSyncGatesChanged)

public :

    enum ASCAN_SAMPLE_FREQUENCY {
        SAMPLE_FREQUENCY_80MHZ,
        SAMPLE_FREQUENCY_40MHZ,
        SAMPLE_FREQUENCY_20MHZ
    };

    bool paramDAutoGain() const { return m_autoGainEnabled; }
    void setParamDAutoGain(bool value) {
        if(m_autoGainEnabled == value)
            return;
        m_autoGainEnabled = value;
        emit paramDAutoGainChanged(value);
    }

    qint16 paramDAutoGainThreshold() const { return m_autoGainThreshold; }
    void setParamDAutoGainThreshold(qint16 value){
        if(m_autoGainThreshold == value)
            return;
        m_autoGainThreshold = value;
        emit paramDAutoGainThresholdChanged(value);
    }

    quint16 paramLGateNoiseStart() const { return m_gateNoiseStart; }
    void setParamLGateNoiseStart(quint16 value){
        if(m_gateNoiseStart == value)
            return;
        m_gateNoiseStart = value;
        emit paramLGateNoiseStartChanged(value);
    }

    quint16 paramLGateNoiseWidth() const { return m_gateNoiseWidth; }
    void setParamLGateNoiseWidth(quint16 value){
        if(m_gateNoiseWidth == value)
            return;
        m_gateNoiseWidth = value;
        emit paramLGateNoiseWidthChanged(value);
    }

    quint8 getParamDFilterMode() const { return m_filterMode; }
    void setParamDFilterMode(quint8 value) {
        if (m_filterMode != value) {
            m_filterMode = value;
            emit paramDFilterModeChanged(value);
        }
    }

    quint8 getParamDWaveType() const { return m_waveType; }
    void setParamDWaveType(quint8 value) {
        if (m_waveType != value) {
            m_waveType = value;
            m_ascan.getAScanEnhancedSample()->m_paramDWaveType = value;
            emit paramDWaveTypeChanged(value);
        }
    }

    quint8 getParamDGate0Option() const {
        return m_ascan.getAScanMeasure()->m_paramGates0->m_paramDOption;
    }
    void setParamDGate0Option(quint8 value) {
        if (m_ascan.getAScanMeasure()->m_paramGates0->m_paramDOption != value) {
            m_ascan.getAScanMeasure()->m_paramGates0->m_paramDOption = value;
            emit paramDGate0OptionChanged(value);
        }
    }

    quint8 getParamDGate1Option() const {
        return m_ascan.getAScanMeasure()->m_paramGates1->m_paramDOption;
    }
    void setParamDGate1Option(quint8 value) {
        if (m_ascan.getAScanMeasure()->m_paramGates1->m_paramDOption != value) {
            m_ascan.getAScanMeasure()->m_paramGates1->m_paramDOption = value;
            emit paramDGate1OptionChanged(value);
        }
    }

    bool paramDAutoSyncGates() const { return m_autoSyncGatesEnabled; }
    void setParamDAutoSyncGates(bool value) {
        if(m_autoSyncGatesEnabled == value)
            return;
        m_autoSyncGatesEnabled = value;
        emit paramDAutoSyncGatesChanged(value);
    }

    VectorFuncAScan(VectorStd *p_pVectorStd, quint32 p_instance); ///< p_instance in 0-3
    ~VectorFuncAScan();

    // -- PARAM

    void sendAScanRawSample(AScanRawSample *p_pAScan);
//    bool isPendingAScan() const;    ///< return true if the list m_lAScanToTreat is not empty.
//    AScan* takeAScan(void);         ///< return the oldest AScan (AScan is no more stored).

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    void clearAScan();///< delete all pending ASCAN

    void injectRawSamples(const quint8* samples, qint64 timestampMs); ///< inject raw 8-bit samples bypassing ProtMsg (used by USB standalone devices)

    // While a local USB device (US-Key) is the AScan source, the AScan parameters and
    // samples sent by the v2 devices are ignored, and their parameters are not requested
    // on connection : they would overwrite the parameters of the local device.
    void setLocalSourceActive(bool p_value);
    bool isLocalSourceActive() const { return m_localSourceActive; }

    static const qint32 ASCAN_BUFFER_SIZE_MAX = 100;
signals :
    void evNewAScanTreated(AScan *);
    void evPrivateAScanTreated();

    void paramDAutoGainChanged(bool value);
    void paramDAutoGainThresholdChanged(qint16 value);
    void paramLGateNoiseStartChanged(quint16 value);
    void paramLGateNoiseWidthChanged(quint16 value);
    void paramDFilterModeChanged(quint8 value);
    void paramDWaveTypeChanged(quint8 value);
    void paramDGate0OptionChanged(quint8 value);
    void paramDGate1OptionChanged(quint8 value);
    void paramDAutoSyncGatesChanged(bool value);

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)

private :
    void resetAScanPart();
    void syncGate1OnSecondEcho(AScan *p_pAScan); ///< "Auto Sync Porte" : center Gate 1 on the 2nd echo found in Gate 0

    static constexpr qreal AUTO_SYNC_GATE_HYSTERESIS_MM = 0.1; ///< minimal shift before moving Gate 1 (avoids jitter and device msg flood)

private slots :
    void onAScanTreated();

private :
    AScan m_ascan;
    qreal m_celerityMperS;

    AScan* m_pAScanRx;    ///< current ASCAN received
    quint32 m_AScanRxPart;

    bool m_localSourceActive = false;   ///< see setLocalSourceActive()
    quint32 m_ignoredMsgCount = 0;      ///< v2 messages ignored while m_localSourceActive (log only)

    bool m_autoGainEnabled = false;
    qint16 m_autoGainThreshold = 5;
    quint16 m_gateNoiseStart = 0;
    quint16 m_gateNoiseWidth = 10;

    bool m_autoSyncGatesEnabled = false;

    quint8 m_filterMode = 0;
    quint8 m_waveType   = 0;

    //QList<AScan *> m_lAScanToTreat;
    //mutable QMutex m_mAScanToTreat;

    AScanWorker * m_pAScanWorker;
    bool m_clearPipe;   ///< if true, we have clean pending ASCAN. remove next ASCAN to be sure
};

Q_DECLARE_OPAQUE_POINTER(VectorFuncAScan*)
#ifndef METATYPE_VECTORFUNCASCAN
#define METATYPE_VECTORFUNCASCAN
Q_DECLARE_METATYPE(VectorFuncAScan*)
#endif
