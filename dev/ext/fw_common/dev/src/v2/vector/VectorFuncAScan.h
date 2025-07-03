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

public :

    enum ASCAN_SAMPLE_FREQUENCY {
        SAMPLE_FREQUENCY_80MHZ,
        SAMPLE_FREQUENCY_40MHZ,
        SAMPLE_FREQUENCY_20MHZ
    };

    VectorFuncAScan(VectorStd *p_pVectorStd, quint32 p_instance); ///< p_instance in 0-3
    ~VectorFuncAScan();

    // -- PARAM

    void sendAScanRawSample(AScanRawSample *p_pAScan);
//    bool isPendingAScan() const;    ///< return true if the list m_lAScanToTreat is not empty.
//    AScan* takeAScan(void);         ///< return the oldest AScan (AScan is no more stored).

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    void clearAScan();///< delete all pending ASCAN


    static const qint32 ASCAN_BUFFER_SIZE_MAX = 100;
signals :
    void evNewAScanTreated(AScan *);
    void evPrivateAScanTreated();

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

private slots :
    void onAScanTreated();

private :
    AScan m_ascan;
    qreal m_celerityMperS;

    AScan* m_pAScanRx;    ///< current ASCAN received
    quint32 m_AScanRxPart;

    //QList<AScan *> m_lAScanToTreat;
    //mutable QMutex m_mAScanToTreat;

    AScanWorker * m_pAScanWorker;
    bool m_clearPipe;   ///< if true, we have clean pending ASCAN. remove next ASCAN to be sure
};
