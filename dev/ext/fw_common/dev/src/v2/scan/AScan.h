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
#include <xml/XmlProject.h>

// -- Qt Includes
#include <QPointF>
#include <QObject>
#include <QMutex>

// ----------------------------------------------------------------------------
// -- class AScanRawSample

class AScanRawSample : public XmlProject {
public :
    AScanRawSample();

    static const qint32 ASCAN_SAMPLE_SIZE = 4000;       ///< legacy e US-Key
    static const qint32 ASCAN_SAMPLE_CAPACITY = 4096;   ///< massimo (D4)
    enum SAMPLE_FORMAT {
        SAMPLE_FORMAT_8BIT = 0,                         ///< legacy
        SAMPLE_FORMAT_14BIT = 1                         ///< D4
    };
    qint16 m_rawSamples[ASCAN_SAMPLE_CAPACITY];
    quint8 m_sampleFormat = SAMPLE_FORMAT_8BIT;
    qint32 m_sampleCount = ASCAN_SAMPLE_SIZE;           ///< campioni validi in questo A-Scan
    qint64 m_epocTimeMs = 0;
    QPointF m_position  = QPointF(0,0); ///< BScan use only X

    void copy(const AScanRawSample& p_value);

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- class AScanEnhancedSample

class AScanEnhancedSample : public XmlProject {
public :
    AScanEnhancedSample();

    static const quint16 ENHANCED_SCALE = 64;
    quint16 m_enhancedSamples[AScanRawSample::ASCAN_SAMPLE_CAPACITY];
    quint8 m_paramLLowPassFilter = 0;
    quint8 m_paramDWaveType      = 0;
    bool   m_hwRectified         = false;   ///< true = samples already rectified by the US-Key (A-scan mode)

    void copyParam(const AScanEnhancedSample& p_value); ///< copy only parameters
    void copy(const AScanEnhancedSample& p_value);      ///< copy data and parameters

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- class SensorSettings

class SensorSettings : public XmlProject {
public :
    SensorSettings();

    quint8 m_paramDGain         = 0;
    quint16 m_paramDPulseWidth  = 0;    ///< 18 to 1600
    quint8 m_paramDSampleFreq   = 0;    ///< ProtMsg::FUNC_ASCAN_PARAM_D_SAMPLE_FREQ : 0 = 80 MHz, 1 = 40 MHz, 2 = 20 MHz, 3 = 125 MHz
    quint8 m_paramDUtElement    = 0;
    quint8 m_paramDVoltage      = 0;
    quint32 m_paramLCelerity = 5940;

    qreal getSampleFrequency() const;
    qreal getSampleStepMM() const;  ///< use m_paramDSampleFreq and m_paramLCelerity to compute distance between 2 samples

    void copyParam(const SensorSettings& p_value);

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- class AScanGate

class AScanGate : public XmlProject {
public :
    AScanGate(qint32 p_gateId);

    qreal m_paramLStart     = 0;
    qreal m_paramLWidth     = 0;
    qreal m_paramLHeight    = 0;
    quint8  m_paramDOption  = 0; // 0 = Premier maximum, 1 = Maximum absolu, 2 = Flanc montant, 3 = Custom Roboplanet

    qint32 m_gateId;

    void copyParam(const AScanGate& p_value);

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- class AScanMeasure

class AScanMeasure : public XmlProject {
public :
    AScanMeasure();
    ~AScanMeasure();

    static const qint32 ASCAN_NB_GATE = 2;
    AScanGate* m_gates[ASCAN_NB_GATE];

    AScanGate* m_paramGates0;
    AScanGate* m_paramGates1;

    qreal m_thicknessMmMultiEcho    = 0;
    qreal m_thicknessMm1Echo        = 0;
    qreal m_paramLProbeDelayMm      = 0;
    QList<quint32> m_listPicsIndex;         //
    qint32 m_indexSecondEchoGate0   = -1;   ///< sample index of the 2nd echo found in Gate 0 (-1 if none), used by "Auto Sync Porte"

    void copyParam(const AScanMeasure& p_value);
    void copy(const AScanMeasure& p_value);

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- class AScan

/**
 * @brief The AScan class
 *
 * Structure to store data of AScan
 *
 * TODO : add settings used to compute m_ascanEnhancedSample and m_ascanMeasure.
 * Process again the AScan if these settings are changed.
 *
 */
class AScan : public QObject, public XmlProjectLoggable {

    Q_OBJECT
public :
    AScan();
    ~AScan();

    void init();

    AScanRawSample * getAScanRawSample();
    AScanEnhancedSample * getAScanEnhancedSample();
    SensorSettings * getSensorSettings();
    AScanMeasure * getAScanMeasure();
    const AScanMeasure * getAScanMeasure() const;

    quint32 m_source;   ///< Sensor number. Value in [0..VectorStd::NB_FUNC_ASCAN[

    void lock();
    void unlock();

    void copyParam(AScan& p_value); ///< copy only parameters
    void copy(AScan& p_value);      ///< copy data and parameters

    quint32 mmToIndex(qreal p_posMm);   ///< compute an index function to Probe Delay, FrequencySample. return 0 if invalid

    qint32 m_cellsCount = 0;

protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :

    // -- Data
    AScanRawSample m_ascanRawSample;            ///< AScan from sensor
    AScanEnhancedSample m_ascanEnhancedSample;  ///< m_ascanRawSample with filters
    SensorSettings m_sensorSettings;            ///< Settings of the sensors during the measure
    AScanMeasure m_ascanMeasure;                ///< Data computed from the process of m_ascanRawSample

    QMutex m_lock;
};
