/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Probe.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/


// -- Internal include
#include <scan/probe/Probe.h>
#include <scan/probe/ProbeUsKey.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- Class ProbeParam


ProbeParam::ProbeParam() : XmlProjectLoggable("ProbeParam")
{

}

ProbeParam::ProbeParam(const ProbeParam & p_value) : QObject(), XmlProjectLoggable("ProbeParam")
{
    (*this) = p_value;
}

ProbeParam::~ProbeParam()
{

}

quint32 ProbeParam::getId() const
{
    return m_ID;
}

void ProbeParam::setId(quint32 p_value)
{
    m_ID = p_value;
}

QSizeF ProbeParam::getParamSensorSize() const
{
    return m_paramSensorSize;
}

void ProbeParam::setParamSensorSize(const QSizeF & p_value)
{
    m_paramSensorSize = p_value;
}

QString ProbeParam::getParamSensorReference(void) const
{
    return m_paramSensorReference;
}

void ProbeParam::setParamSensorReference(const QString & p_value)
{
    m_paramSensorReference = p_value;
}

// -- XmlProject
bool ProbeParam::customLoadParam(QDomElement *p_pParent)
{
    bool bRet = true;
    QString _sensorSize;
    bRet &= XML_READ_UNIQUE(QString, _sensorSize);
    m_paramSensorSize = XmlHelper::QSizeFFromString(_sensorSize);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorReference);

    return bRet;
}

bool ProbeParam::customLoadDefaultParam()
{
    m_paramSensorSize = QSizeF(0,0);
    m_paramSensorReference = "RP";
    return true;
}

bool ProbeParam::customApplyParam()
{
    return true;
}

bool ProbeParam::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    quint32 _type = getProbeType();
    XML_WRITE_UNIQUE(_type);  // save type to identify the item when loading project

    QString _sensorSize = XmlHelper::QSizeFToString(m_paramSensorSize);
    XML_WRITE_UNIQUE(_sensorSize);
    XML_WRITE_UNIQUE(m_paramSensorReference);

    return true;
}


// -- operator
ProbeParam & ProbeParam::operator= (const ProbeParam & p_value)
{
    if (this != &p_value) {
        m_paramSensorSize = p_value.getParamSensorSize();
        m_paramSensorReference = p_value.getParamSensorReference();
    }
    return *this;
}

bool ProbeParam::operator== (const ProbeParam & p_value)
{
    return m_ID == p_value.getId();
}


// ----------------------------------------------------------------------------
// -- Class ProbeCalibrate

ProbeCalibrate::ProbeCalibrate()
{
    setLogName("ProbeCalibrate");
    setXmlSectionName("ProbeCalibrate");
}

ProbeCalibrate::ProbeCalibrate(const ProbeCalibrate & p_value) : Calibrate(p_value)
{
    // \todo
}

ProbeCalibrate::~ProbeCalibrate()
{
    // delete m_pMeasurParam & m_pCoupling;
}

// -- operator
ProbeCalibrate & ProbeCalibrate::operator= (const ProbeCalibrate & /*p_value*/)
{
    // \todo
    return (*this);
}

bool ProbeCalibrate::operator== (const ProbeCalibrate & /*p_value*/)
{
    // \todo
    return true;
}


// ----------------------------------------------------------------------------
// -- Class Probe

QString Probe::PobeTypeName(quint32 e)
{
    const std::map<quint32,const char*> MyEnumStrings {
           { Probe::ProbeType::TYPE_UNDEF, "UNDEFINED" },
           { Probe::ProbeType::TYPE_USKEY, "USKEY" }
       };
       auto   it  = MyEnumStrings.find(e);
       return it == MyEnumStrings.end() ? QString("Out of range") : QString(it->second);
}


Probe::Probe(ProbeParam * p_pProbeParam, ProbeCalibrate * p_pProbeCalibrate) : QObject(),
    XmlProjectLoggable("Probe")
{
    m_pProbeParam = p_pProbeParam;
    m_pProbeCalibrate = p_pProbeCalibrate;

    // param
    m_paramSensorNumberElements = 0;
}

Probe::~Probe()
{
    // inherited classe is the owner
    m_pProbeParam = nullptr;
    m_pProbeCalibrate = nullptr;
}

quint32 Probe::getProbeType(void) const
{
    return m_pProbeParam->getProbeType();
}

QString Probe::getProbeTypeName(void) const
{
    return PobeTypeName(m_pProbeParam->getProbeType());
}


void Probe::setProbeParam(const ProbeParam & p_value)
{
    *m_pProbeParam = p_value;
}

ProbeParam * Probe::getProbeParamCopy(void) const
{
    return m_pProbeParam->copy();  // return a copy
}

ProbeParam *Probe::getProbeParam() const
{
    return m_pProbeParam;
}

ProbeCalibrate * Probe::getProbeCalibrate() const
{
    return m_pProbeCalibrate;
}

void Probe::setProbeCalibrate(const ProbeCalibrate & p_value)
{
    *m_pProbeCalibrate = p_value;
}


// -- Parameters
QString Probe::getParamSensorBrand(void) const
{
    return m_paramSensorBrand;
}

void Probe::setParamSensorBrand(const QString & p_value)
{
    m_paramSensorBrand = p_value;
}

QString Probe::getParamSensorType(void)
{
    return m_paramSensorType;
}

void Probe::setParamSensorType(const QString & p_value)
{
    m_paramSensorType = p_value;
}

QString Probe::getParamSensorSerialNumber(void)
{
    return m_paramSensorSerialNumber;
}

void Probe::setParamSensorSerialNumber(const QString & p_value)
{
    m_paramSensorSerialNumber = p_value;
}

QString Probe::getParamSensorWave(void)
{
    return m_paramSensorWave;
}

void Probe::setParamSensorWave(const QString & p_value)
{
    m_paramSensorWave = p_value;
}

QString Probe::getParamSensorFrequency(void)
{
    return m_paramSensorFrequency;
}

void Probe::setParamSensorFrequency(const QString & p_value)
{
    m_paramSensorFrequency = p_value;
}

QString Probe::getParamSensorAngle(void)
{
    return m_paramSensorAngle;
}

void Probe::setParamSensorAngle(const QString & p_value)
{
    m_paramSensorAngle = p_value;
}

QString Probe::getParamSensorPiezoSize(void)
{
    return m_paramSensorPiezoSize;
}

void Probe::setParamSensorPiezoSize(const QString & p_value)
{
    m_paramSensorPiezoSize = p_value;
}

quint32 Probe::getParamSensorNumberElements(void)
{
    return m_paramSensorNumberElements;
}

void Probe::setParamSensorNumberElements(quint32 p_value)
{
    m_paramSensorNumberElements = p_value;
}

// -- XmlProject
bool Probe::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    // Composition
    bRet &= m_pProbeParam->loadParam(p_pParent);
    bRet &= m_pProbeCalibrate->loadParam(p_pParent);

    // Parameters
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorBrand);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorType);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorSerialNumber);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorWave);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorFrequency);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorAngle);
    bRet &= XML_READ_UNIQUE(QString, m_paramSensorPiezoSize);
    bRet &= XML_READ_UNIQUE(quint32, m_paramSensorNumberElements);

    return bRet;
}

bool Probe::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= m_pProbeParam->customLoadDefaultParam();
    bRet &= m_pProbeCalibrate->customLoadDefaultParam();

    m_paramSensorBrand = "ROBOPLANET";
    m_paramSensorType = "RX1";
    m_paramSensorSerialNumber = "#";
    m_paramSensorWave = "Ultrasonic";
    m_paramSensorFrequency = "5 MHz";
    m_paramSensorAngle = "0";
    m_paramSensorPiezoSize = "8 mm";
    m_paramSensorNumberElements = 2;

    return m_pProbeParam->customLoadDefaultParam();
}

bool Probe::customApplyParam()
{
    bool bRet = true;
    bRet &= m_pProbeParam->customApplyParam();
    bRet &= m_pProbeCalibrate->customApplyParam();

    return bRet;
}

bool Probe::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;

    quint32 _type = getProbeType();
    bRet &= XML_WRITE_UNIQUE(_type);    // write type to be loaded next time

    // Composition
    bRet &= m_pProbeParam->saveParam(p_pDoc, p_pParent);
    bRet &= m_pProbeCalibrate->saveParam(p_pDoc, p_pParent);

    // Parameters
    bRet &= XML_WRITE_UNIQUE(m_paramSensorBrand);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorType);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorSerialNumber);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorWave);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorFrequency);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorAngle);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorPiezoSize);
    bRet &= XML_WRITE_UNIQUE(m_paramSensorNumberElements);

    return bRet;
}

// -- Behaviour

Probe *Probe::makeProbe(quint32 p_type)
{
    Probe * pProbe = nullptr;
    switch(p_type) {
        case TYPE_USKEY :
            pProbe = new ProbeUsKey();
            break;
        default :
            Logger::logError(QString("Probe *Probe::makeProbe type not defined : %1").arg(p_type));
    }
    return pProbe;
}
