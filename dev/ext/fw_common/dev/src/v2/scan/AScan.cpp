/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AScan.cpp
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/

// -- Internal includes
#include "AScan.h"

// -- Qt Includes
#include <QIODevice>
#include <QDataStream>

// ----------------------------------------------------------------------------
// -- class AScanRawSample

AScanRawSample::AScanRawSample() : XmlProject("AScanRawSample")
{
}

void AScanRawSample::copy(const AScanRawSample &p_value)
{
    m_epocTimeMs = p_value.m_epocTimeMs;
    m_position = p_value.m_position;
    m_sampleCount = p_value.m_sampleCount;
    m_sampleFormat = p_value.m_sampleFormat;
    memcpy(m_rawSamples, p_value.m_rawSamples, sizeof(m_rawSamples));
}

bool AScanRawSample::AScanRawSample::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    QString sRawSample;
    bRet &= XML_READ_UNIQUE(QString, sRawSample);
    XML_READ_UNIQUE_DEFAULT(quint8, m_sampleFormat, 0);
    const bool is14Bit = (SAMPLE_FORMAT_14BIT == m_sampleFormat);
    const QByteArray baRaw = QByteArray::fromHex(sRawSample.toLatin1());
    const qint32 available = is14Bit ? qint32(baRaw.size() / 2) : qint32(baRaw.size());
    m_sampleCount = (ASCAN_SAMPLE_CAPACITY == available) ? qint32(ASCAN_SAMPLE_CAPACITY) : qint32(ASCAN_SAMPLE_SIZE);
    memset(m_rawSamples, 0, sizeof(m_rawSamples));
    const quint8 *bytes = reinterpret_cast<const quint8 *>(baRaw.constData());
    for( qint32 i = 0; i < qMin(available, m_sampleCount); i++ ) {
        m_rawSamples[i] = is14Bit ? qint16(quint16(bytes[2 * i]) | (quint16(bytes[2 * i + 1]) << 8)) : qint16(bytes[i]);
    }
    bRet &= XML_READ_UNIQUE(qint64, m_epocTimeMs);
    QString sPosition;
    bRet &= XML_READ_UNIQUE(QString, sPosition);
    m_position = XmlHelper::QPointFFromString(sPosition);
    return bRet;
}

bool AScanRawSample::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString sRawSample;
    QByteArray ba;
    const bool is14Bit = (SAMPLE_FORMAT_14BIT == m_sampleFormat);
    ba.resize(is14Bit ? m_sampleCount * 2 : m_sampleCount);
    for( qint32 i = 0; i < m_sampleCount; i++ ){
        if(is14Bit){
            ba[2 * i] = char(quint16(m_rawSamples[i]) & 0xFF);
            ba[2 * i + 1] = char(quint16(m_rawSamples[i]) >> 8);
        } else {
            ba[i] = char(quint8(m_rawSamples[i]));
        }
    }
    QDataStream ds(&ba, QIODevice::WriteOnly);
    sRawSample = ba.toHex().data();
    bRet &= XML_WRITE_UNIQUE(sRawSample);
    bRet &= XML_WRITE_UNIQUE(m_sampleFormat);
    bRet &= XML_WRITE_UNIQUE(m_epocTimeMs);
    QString sPosition = XmlHelper::QPointFToString(m_position);
    bRet &= XML_WRITE_UNIQUE(sPosition);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- class AScanEnhancedSample

AScanEnhancedSample::AScanEnhancedSample() : XmlProject("AScanEnhancedSample")
{
}

void AScanEnhancedSample::copyParam(const AScanEnhancedSample &p_value)
{
    m_paramLLowPassFilter = p_value.m_paramLLowPassFilter;
    m_paramDWaveType        = p_value.m_paramDWaveType;
    m_hwRectified           = p_value.m_hwRectified;
}

void AScanEnhancedSample::copy(const AScanEnhancedSample &p_value)
{
    copyParam(p_value);
    memcpy(m_enhancedSamples, p_value.m_enhancedSamples, sizeof(m_enhancedSamples));
}

bool AScanEnhancedSample::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    QString sEnhancedSample;
    bRet &= XML_READ_UNIQUE(quint8, m_paramLLowPassFilter);
    bRet &= XML_READ_UNIQUE(quint8, m_paramDWaveType);
    return bRet;
}

bool AScanEnhancedSample::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString sEnhancedSample;
    QByteArray ba;
    ba.setRawData((const char*)m_enhancedSamples, sizeof(m_enhancedSamples));
    QDataStream ds(&ba, QIODevice::WriteOnly);
    sEnhancedSample = ba.toHex().data();
    bRet &= XML_WRITE_UNIQUE(m_paramLLowPassFilter);
    bRet &= XML_WRITE_UNIQUE(m_paramDWaveType);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- class SensorSettings

SensorSettings::SensorSettings() : XmlProject("SensorSettings")
{
}

qreal SensorSettings::getSampleFrequency() const
{
    qreal rRet = 80000000;
    // 0 = 80 MHz, 1 = 40 MHz, 2 = 20 MHz
    if ( 1 == m_paramDSampleFreq) {
        rRet = 40000000;
    } else if( 2 == m_paramDSampleFreq ) {
        rRet = 20000000;
    } else if( 3 == m_paramDSampleFreq ) {
        rRet = 125000000;
    }
    return rRet;
}

qreal SensorSettings::getSampleStepMM() const
{
    qreal p = 1/getSampleFrequency(); // in s
    // m_paramLCelerity is in m/s
    qreal dMm = m_paramLCelerity*1000; // in mm/s
    qreal s = p*dMm/2; // /2 because an echo is a go and a back
    return s;
}

void SensorSettings::copyParam(const SensorSettings &p_value)
{
    m_paramDGain = p_value.m_paramDGain;
    m_paramDPulseWidth = p_value.m_paramDPulseWidth;
    m_paramDSampleFreq = p_value.m_paramDSampleFreq;
    m_paramDUtElement = p_value.m_paramDUtElement;
    m_paramDVoltage = p_value.m_paramDVoltage;
    m_paramLCelerity = p_value.m_paramLCelerity;
}

bool SensorSettings::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDGain,10);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDPulseWidth, 125);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDSampleFreq, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDUtElement, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDVoltage, 100);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_paramLCelerity, 5940);
    return bRet;
}

bool SensorSettings::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDGain);
    bRet &= XML_WRITE_UNIQUE(m_paramDPulseWidth);
    bRet &= XML_WRITE_UNIQUE(m_paramDSampleFreq);
    bRet &= XML_WRITE_UNIQUE(m_paramDUtElement);
    bRet &= XML_WRITE_UNIQUE(m_paramDVoltage);
    bRet &= XML_WRITE_UNIQUE(m_paramLCelerity);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- class AScanGate

AScanGate::AScanGate(qint32 p_gateId) : XmlProject(QString("AScanGate%1").arg(p_gateId)), m_gateId(p_gateId)
{

}

void AScanGate::copyParam(const AScanGate &p_value)
{
    m_paramLStart = p_value.m_paramLStart;
    m_paramLWidth = p_value.m_paramLWidth;
    m_paramLHeight = p_value.m_paramLHeight;
    m_paramDOption = p_value.m_paramDOption;
}

// -- Inherited from XmlProject
bool AScanGate::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLStart, 10);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLWidth, 50);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLHeight, 10);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDOption, 0);
    return bRet;
}

bool AScanGate::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramLStart);
    bRet &= XML_WRITE_UNIQUE(m_paramLWidth);
    bRet &= XML_WRITE_UNIQUE(m_paramLHeight);
    bRet &= XML_WRITE_UNIQUE(m_paramDOption);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- class AScanMeasure

AScanMeasure::AScanMeasure() : XmlProject("AScanMeasure")
{
    for(int i=0; i<ASCAN_NB_GATE; i++) {
        m_gates[i] = new AScanGate(i);
    }

    m_paramGates0 = m_gates[0];
    m_paramGates1 = m_gates[1];
}

AScanMeasure::~AScanMeasure()
{
    for(int i=0; i<ASCAN_NB_GATE; i++) {
        delete m_gates[i];
        m_gates[i] = nullptr;
    }
}

void AScanMeasure::copyParam(const AScanMeasure &p_value)
{
    for(int i=0; i<ASCAN_NB_GATE; i++) {
        m_gates[i]->copyParam(*p_value.m_gates[i]);
    }
    m_paramLProbeDelayMm = p_value.m_paramLProbeDelayMm;
}

void AScanMeasure::copy(const AScanMeasure &p_value)
{
    copyParam(p_value);
    // data
    m_thicknessMmMultiEcho    = p_value.m_thicknessMmMultiEcho;
    m_thicknessMm1Echo        = p_value.m_thicknessMm1Echo;
    m_listPicsIndex           = p_value.m_listPicsIndex;
    m_indexSecondEchoGate0    = p_value.m_indexSecondEchoGate0;
}

bool AScanMeasure::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    for(int i=0; i<ASCAN_NB_GATE; i++) {
        bRet &= m_gates[i]->loadParam(p_pParent);
    }
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_thicknessMmMultiEcho, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_thicknessMm1Echo, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLProbeDelayMm, 0);
    return bRet;
}

bool AScanMeasure::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    for(int i=0; i<ASCAN_NB_GATE; i++) {
        bRet &= m_gates[i]->saveParam(p_pDoc, p_pParent);
    }
    bRet &= XML_WRITE_UNIQUE(m_thicknessMmMultiEcho);
    bRet &= XML_WRITE_UNIQUE(m_thicknessMm1Echo);
    bRet &= XML_WRITE_UNIQUE(m_paramLProbeDelayMm);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- class AScan

AScan::AScan() : XmlProjectLoggable("AScan")
{
    m_source = 0;
}

AScan::~AScan()
{

}

void AScan::init()
{
    for(int i=0; i<AScanRawSample::ASCAN_SAMPLE_CAPACITY; i++) {
        m_ascanRawSample.m_rawSamples[i] = 0;
        m_ascanEnhancedSample.m_enhancedSamples[i] = 0;
    }
}

AScanRawSample * AScan::getAScanRawSample()
{
    return &m_ascanRawSample;
}

AScanEnhancedSample * AScan::getAScanEnhancedSample()
{
    return &m_ascanEnhancedSample;
}

SensorSettings * AScan::getSensorSettings()
{
    return &m_sensorSettings;
}

AScanMeasure * AScan::getAScanMeasure()
{
    return &m_ascanMeasure;
}

const AScanMeasure * AScan::getAScanMeasure() const
{
    return &m_ascanMeasure;
}

void AScan::lock()
{
    m_lock.lock();
}

void AScan::unlock()
{
    m_lock.unlock();
}

void AScan::copyParam(AScan &p_value)
{
    m_sensorSettings.copyParam(*(p_value.getSensorSettings()));
    m_ascanEnhancedSample.copyParam(*(p_value.getAScanEnhancedSample()));
    m_ascanMeasure.copyParam(*(p_value.getAScanMeasure()));
    m_source = p_value.m_source;
}

void AScan::copy(AScan &p_value)
{
    m_sensorSettings.copyParam(*(p_value.getSensorSettings()));
    m_ascanRawSample.copy(*(p_value.getAScanRawSample()));
    m_ascanEnhancedSample.copy(*(p_value.getAScanEnhancedSample()));
    m_ascanMeasure.copy(*(p_value.getAScanMeasure()));
    m_source = p_value.m_source;
}

quint32 AScan::mmToIndex(qreal p_posMm)
{
    qreal d = m_sensorSettings.getSampleStepMM();
    qreal index = (p_posMm + m_ascanMeasure.m_paramLProbeDelayMm) / d;
    const qint32 sampleCount = m_ascanRawSample.m_sampleCount;
    if( index >= 0 && index < sampleCount ) {
        return index;
    }
    if( 3 != m_sensorSettings.m_paramDSampleFreq) {
        fLogWarning(QString("position out of range : %1").arg(index));
    }
    if( index >= sampleCount ) {
        return sampleCount-1;
    }
    return 0;
}

// -- Inherited from XmlProject
bool AScan::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_ascanRawSample.loadParam(p_pParent);
    bRet &= m_ascanEnhancedSample.loadParam(p_pParent);
    bRet &= m_sensorSettings.loadParam(p_pParent);
    bRet &= m_ascanMeasure.loadParam(p_pParent);
    return bRet;
}

bool AScan::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_ascanRawSample.saveParam(p_pDoc, p_pParent);
    bRet &= m_ascanEnhancedSample.saveParam(p_pDoc, p_pParent);
    bRet &= m_sensorSettings.saveParam(p_pDoc, p_pParent);
    bRet &= m_ascanMeasure.saveParam(p_pDoc, p_pParent);
    return bRet;
}

