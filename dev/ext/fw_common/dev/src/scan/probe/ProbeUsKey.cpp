/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ProbeUsKey.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#include <scan/probe/ProbeUsKey.h>

// ----------------------------------------------------------------------------
// -- Class ProbeParamUsKey

ProbeParamUsKey::ProbeParamUsKey()
{

}

ProbeParamUsKey::ProbeParamUsKey(const ProbeParamUsKey & p_value) : ProbeParam(p_value)
{
    (*this) = p_value;
}

ProbeParamUsKey::~ProbeParamUsKey()
{

}


// -- ProbeParam
quint32 ProbeParamUsKey::getProbeType(void) const
{
    return Probe::TYPE_USKEY;
}

ProbeParam* ProbeParamUsKey::copy() const
{
    return new ProbeParamUsKey(*this);
}


// -- Parameters
double ProbeParamUsKey::getParamFilter() const
{
    return m_paramFilter;
}

void ProbeParamUsKey::setParamFilter(double p_paramFilter)
{
    m_paramFilter = p_paramFilter;
}

double ProbeParamUsKey::getParamSensorMode() const
{
    return m_paramSensorMode;
}

void ProbeParamUsKey::setParamSensorMode(double p_paramSensorMode)
{
    m_paramSensorMode = p_paramSensorMode;
}

double ProbeParamUsKey::getParamGain() const
{
    return m_paramGain;
}

void ProbeParamUsKey::setParamGain(double p_paramGain)
{
    m_paramGain = p_paramGain;
}

double ProbeParamUsKey::getParamVoltage() const
{
    return m_paramVoltage;
}

void ProbeParamUsKey::setParamVoltage(double p_paramVoltage)
{
    m_paramVoltage = p_paramVoltage;
}

double ProbeParamUsKey::getParamWidth() const
{
    return m_paramWidth;
}

void ProbeParamUsKey::setParamWidth(double p_paramWidth)
{
    m_paramWidth = p_paramWidth;
}

double ProbeParamUsKey::getParamSamplingFreqMode() const
{
    return m_paramSamplingFreqMode;
}

void ProbeParamUsKey::setParamSamplingFreqMode(double p_paramSamplingFreqMode)
{
    m_paramSamplingFreqMode = p_paramSamplingFreqMode;
}


// -- XmlProject
bool ProbeParamUsKey::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = ProbeParam::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(double, m_paramFilter);
    bRet &= XML_READ_UNIQUE(double, m_paramSensorMode);
    bRet &= XML_READ_UNIQUE(double, m_paramGain);
    bRet &= XML_READ_UNIQUE(double, m_paramVoltage);
    bRet &= XML_READ_UNIQUE(double, m_paramWidth);
    bRet &= XML_READ_UNIQUE(double, m_paramSamplingFreqMode);
    return bRet;
}

bool ProbeParamUsKey::customLoadDefaultParam()
{
    bool bRet = ProbeParam::customLoadDefaultParam();
    m_paramFilter = 4;    // 4 = Broad band
    m_paramSensorMode = 1;      // 1 = dual element
    m_paramGain = 12;               // 12 (standard)
    m_paramVoltage = 150;           // 150 (standard)
    m_paramWidth = 10;              // 10 (funtion to the sensor)
    return bRet;
}

bool ProbeParamUsKey::customApplyParam()
{
    bool bRet = ProbeParam::customApplyParam();
    return bRet;
}

bool ProbeParamUsKey::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = ProbeParam::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_paramFilter);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorMode);
    bRet &= XML_WRITE_UNIQUE(m_paramGain);
    bRet &= XML_WRITE_UNIQUE(m_paramVoltage);
    bRet &= XML_WRITE_UNIQUE(m_paramWidth);
    bRet &= XML_WRITE_UNIQUE(m_paramSamplingFreqMode);
    return bRet;
}


// ----------------------------------------------------------------------------
// -- Class ProbeCalibrateUsKey


ProbeCalibrateUsKey::ProbeCalibrateUsKey()
{

}

ProbeCalibrateUsKey::ProbeCalibrateUsKey(const ProbeCalibrateUsKey &p_value) : ProbeCalibrate(p_value)
{
    (*this) = p_value;
}

ProbeCalibrateUsKey::~ProbeCalibrateUsKey()
{

}

// -- ProbeParam
quint32 ProbeCalibrateUsKey::getProbeCalibrateType(void) const
{
    return ProbeCalibrate::TYPE_USKEY_STEP_WEDGE;
}

ProbeCalibrate* ProbeCalibrateUsKey::copy() const
{
    return new ProbeCalibrateUsKey(*this);
}


// -- Parameters
QString ProbeCalibrateUsKey::getParamWedgeType(void) const
{
    return m_paramWedgeType;
}

void ProbeCalibrateUsKey::setParamWedgeType(const QString & p_value)
{
    m_paramWedgeType = p_value;
}

QString ProbeCalibrateUsKey::getParamWedgeThicknesses(void) const
{
    return m_paramWedgeThicknesses;
}

void ProbeCalibrateUsKey::setParamWedgeThicknesses(const QString & p_value)
{
    m_paramWedgeThicknesses = p_value;
}

QString ProbeCalibrateUsKey::getParamCalibGainRef(void) const
{
    return  m_paramCalibGainRef;
}

void ProbeCalibrateUsKey::setParamCalibGainRef(const QString & p_value)
{
    m_paramCalibGainRef = p_value;
}

QString ProbeCalibrateUsKey::getParamCalibGainControl(void) const
{
    return  m_paramCalibGainControl;
}

void ProbeCalibrateUsKey::setParamCalibGainControl(const QString & p_value)
{
    m_paramCalibGainControl = p_value;
}

QString ProbeCalibrateUsKey::getParamCalibBaseTime(void) const
{
    return m_paramCalibBaseTime;
}

void ProbeCalibrateUsKey::setParamCalibBaseTime(const QString & p_value)
{
    m_paramCalibBaseTime = p_value;
}


// -- XmlProject
bool ProbeCalibrateUsKey::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = ProbeCalibrate::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(QString, m_paramWedgeType);
    bRet &= XML_READ_UNIQUE(QString, m_paramWedgeThicknesses);
    bRet &= XML_READ_UNIQUE(QString, m_paramCalibGainRef);
    bRet &= XML_READ_UNIQUE(QString, m_paramCalibGainControl);
    bRet &= XML_READ_UNIQUE(QString, m_paramCalibBaseTime);
    return bRet;
}

bool ProbeCalibrateUsKey::customLoadDefaultParam()
{
    bool bRet = ProbeCalibrate::customLoadDefaultParam();
    m_paramWedgeType = tr("Cale à gradins");
    m_paramWedgeThicknesses = "3; 5; 8 mm";
    m_paramCalibGainRef = "15";
    m_paramCalibGainControl = "15";
    m_paramCalibBaseTime = "125 ns";
    return bRet;
}

bool ProbeCalibrateUsKey::customApplyParam()
{
    bool bRet = ProbeCalibrate::customApplyParam();
    // nothing to do
    return bRet;
}

bool ProbeCalibrateUsKey::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = ProbeCalibrate::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_paramWedgeType);
    bRet &= XML_WRITE_UNIQUE(m_paramWedgeThicknesses);
    bRet &= XML_WRITE_UNIQUE(m_paramCalibGainRef);
    bRet &= XML_WRITE_UNIQUE(m_paramCalibGainControl);
    bRet &= XML_WRITE_UNIQUE(m_paramCalibBaseTime);
    return bRet;
}

// ----------------------------------------------------------------------------
// -- Class ProbeUsKey


ProbeUsKey::ProbeUsKey() : Probe(&m_probeParamUsKey, &m_probeCalibrateUsKey)
{

}

ProbeUsKey::~ProbeUsKey()
{

}

// -- XmlProject
bool ProbeUsKey::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = Probe::customLoadParam(p_pParent);
    bRet &= XML_READ_UNIQUE(QString, m_sensorBrand);
    bRet &= XML_READ_UNIQUE(QString, m_sensorType);
    bRet &= XML_READ_UNIQUE(QString, m_sensorSerialNumber);
    bRet &= XML_READ_UNIQUE(QString, m_sensorWave);
    bRet &= XML_READ_UNIQUE(QString, m_sensorAngle);
    bRet &= XML_READ_UNIQUE(QString, m_sensorFrequency);
    bRet &= XML_READ_UNIQUE(quint32, m_sensorNbElements);
    bRet &= XML_READ_UNIQUE(qreal, m_sensorPiezoSizeMm);
    return bRet;
}

bool ProbeUsKey::customLoadDefaultParam()
{
    bool bRet = Probe::customLoadDefaultParam();
    m_sensorBrand = "ROBOPLANET";
    m_sensorType = "";
    m_sensorSerialNumber = "";
    m_sensorWave = "";
    m_sensorAngle = "0";
    m_sensorFrequency = "5 Mhz";
    m_sensorNbElements = 2;
    m_sensorPiezoSizeMm = 5;
    return bRet;
}

bool ProbeUsKey::customApplyParam()
{
    bool bRet = Probe::customApplyParam();
    return bRet;
}

bool ProbeUsKey::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = Probe::customSaveParam(p_pDoc, p_pParent);
    bRet &= XML_WRITE_UNIQUE(m_sensorBrand);
    bRet &= XML_WRITE_UNIQUE(m_sensorType);
    bRet &= XML_WRITE_UNIQUE(m_sensorSerialNumber);
    bRet &= XML_WRITE_UNIQUE(m_sensorWave);
    bRet &= XML_WRITE_UNIQUE(m_sensorAngle);
    bRet &= XML_WRITE_UNIQUE(m_sensorFrequency);
    bRet &= XML_WRITE_UNIQUE(m_sensorNbElements);
    bRet &= XML_WRITE_UNIQUE(m_sensorPiezoSizeMm);
    return bRet;
}


// -- References
QString ProbeUsKey::getSensorBrand() const
{
    return m_sensorBrand;
}

void ProbeUsKey::setParamSensorBrand(const QString & p_sensorBrand)
{
    m_sensorBrand = p_sensorBrand;
}

QString ProbeUsKey::getSensorType() const
{
    return m_sensorType;
}

void ProbeUsKey::setSensorType(const QString & p_sensorType)
{
    m_sensorType = p_sensorType;
}

QString ProbeUsKey::getSensorSerialNumber() const
{
    return m_sensorSerialNumber;
}

void ProbeUsKey::setSensorSerialNumber(const QString & p_sensorSerialNumber)
{
    m_sensorSerialNumber = p_sensorSerialNumber;
}

QString ProbeUsKey::getSensorWave() const
{
    return m_sensorWave;
}

void ProbeUsKey::setSensorWave(const QString & p_sensorWave)
{
    m_sensorWave = p_sensorWave;
}

QString ProbeUsKey::getSensorAngle() const
{
    return m_sensorAngle;
}

void ProbeUsKey::setSensorAngle(const QString & p_sensorAngle)
{
    m_sensorAngle = p_sensorAngle;
}

QString ProbeUsKey::getSensorFrequency() const
{
    return m_sensorFrequency;
}

void ProbeUsKey::setSensorFrequency(const QString & p_sensorFrequency)
{
    m_sensorFrequency = p_sensorFrequency;
}

quint32 ProbeUsKey::getSensorNbElements() const
{
    return m_sensorNbElements;
}

void ProbeUsKey::setSensorNbElements(quint32 p_sensorNbElements)
{
    m_sensorNbElements = p_sensorNbElements;
}

qreal ProbeUsKey::getSensorPiezoSizeMm() const
{
    return m_sensorPiezoSizeMm;
}

void ProbeUsKey::setSensorPiezoSizeMm(qreal p_sensorPiezoSizeMm)
{
    m_sensorPiezoSizeMm = p_sensorPiezoSizeMm;
}
