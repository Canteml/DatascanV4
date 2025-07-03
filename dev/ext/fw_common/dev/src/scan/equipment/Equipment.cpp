/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Equipment.cpp
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

// -- Internal includes

#include <scan/equipment/Equipment.h>
#include <scan/equipment/Coupling.h>
#include <scan/vector/Vector.h>
#include <scan/probe/Probe.h>

#include <xml/XmlHelper.h>

QString Equipment::EquipmentTypeName(quint32 e)
{
    const std::map<quint32,const char*> MyEnumStrings {
           { Equipment::EquipmentTypeStd::TYPE_UNDEF, "UNDEFINED" },
           { Equipment::EquipmentTypeStd::TYPE_UTPOLE_USKEY, "UT POLE + USKEY" },
           { Equipment::EquipmentTypeStd::TYPE_ALTISCAN_USKEY, "ALTISCAN + USKEY" }
    };
    auto   it  = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? QString("Out of range") : QString(it->second);
}

Equipment::Equipment(Vector* p_pVector, Probe* p_pProbe, Coupling* p_pCoupling) :
    XmlProjectLoggable("Equipment")
{
    m_pVector = p_pVector;
    m_pProbe = p_pProbe;
    m_pCoupling = p_pCoupling;

    m_resolutionMax = QPointF(0,0);
    m_frequencyAcqMax = 0;
    m_zoneScanningMax = QSize(0,0);
    m_speedMaxMMS = 0;
}

Equipment::Equipment():
    XmlProjectLoggable("Equipment")
{
    m_pVector = nullptr;
    m_pProbe = nullptr;
    m_pCoupling = nullptr;

    m_resolutionMax = QPointF(0,0);
    m_frequencyAcqMax = 0;
    m_zoneScanningMax = QSize(0,0);
    m_speedMaxMMS = 0;
}

Equipment::~Equipment()
{
    // delete composition children
    if( nullptr != m_pVector ) {
        delete m_pVector;
        m_pVector = nullptr;
    }
    if( nullptr != m_pProbe ) {
        delete m_pProbe;
        m_pProbe = nullptr;
    }
    if( nullptr != m_pCoupling ) {
        delete m_pCoupling;
        m_pCoupling = nullptr;
    }
}


quint32 Equipment::getEquipmentTypeStd(void) const
{
    quint32 iRet = EquipmentTypeStd::TYPE_UTPOLE_NOT_STD;

    if( m_pVector != nullptr && m_pProbe != nullptr ) {
        if( Vector::VectorType::TYPE_UTPOLE == m_pVector->getVectorType() && Probe::ProbeType::TYPE_USKEY == m_pProbe->getProbeType() ) {
            iRet = TYPE_UTPOLE_USKEY;
        } else if( Vector::VectorType::TYPE_ALTISCAN == m_pVector->getVectorType() && Probe::ProbeType::TYPE_USKEY == m_pProbe->getProbeType() ) {
            iRet = TYPE_ALTISCAN_USKEY;
        } else {
            fLogError(QString("Not an Std Type (%1,¨2)").arg(m_pVector->getVectorType()).arg(m_pProbe->getProbeType()));
        }
    }
    return iRet;
}

QString Equipment::getProbeTypeStdName(void) const
{
    QString sRet = tr("Inconnu");
    quint32 iType = getEquipmentTypeStd();
    switch (iType) {
        case TYPE_UTPOLE_NOT_STD :
            sRet = "Non standard";
            break;
        case TYPE_UTPOLE_USKEY :
            sRet = "UTPOLE + USKEY";
            break;
        default :
            fLogWarning(QString("Unknow type %1").arg(iType));
    }
    return sRet;
}

// -- Properties
QPointF Equipment::getResolutionMax() const
{
    return m_resolutionMax;
}

qreal Equipment::getFrequencyAcqMax() const
{
    return m_frequencyAcqMax;
}

QSizeF Equipment::getZoneScanningMax() const
{
    return m_zoneScanningMax;
}

qreal Equipment::getSpeedMaxMMS() const
{
    return m_speedMaxMMS;
}


// -- Composition accessors

void Equipment::setVector(Vector * p_vector)
{
    if( nullptr != m_pVector ) {
        delete m_pVector;
        m_pVector = nullptr;
    }
    m_pVector = p_vector;
}

Vector * Equipment::getVector() const
{
    return m_pVector;
}

void Equipment::setPRobe(Probe * p_probe)
{
    if( nullptr != m_pProbe ) {
        delete m_pProbe;
        m_pProbe = nullptr;
    }
    m_pProbe = p_probe;
}

Probe * Equipment::getProbe() const
{
    return m_pProbe;
}

void Equipment::setCoupling(Coupling *p_coupling)
{
    if( nullptr != p_coupling ) {
        delete m_pCoupling;
        m_pCoupling = nullptr;
    }
    m_pCoupling = p_coupling;
}

void Equipment::setCoupling(const Coupling & p_coupling)
{
    *m_pCoupling = p_coupling;
}

Coupling * Equipment::getCoupling() const
{
    return m_pCoupling;
}


// -- XmlProject
bool Equipment::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    QString resolutionMax, zoneScanningMax;
    bRet &= XML_READ_UNIQUE(quint32, m_type);
    bRet &= XML_READ_UNIQUE(QString, resolutionMax);
    m_resolutionMax = XmlHelper::QPointFromString(resolutionMax);
    bRet &= XML_READ_UNIQUE(qreal, m_frequencyAcqMax);        
    bRet &= XML_READ_UNIQUE(QString, zoneScanningMax);
    m_zoneScanningMax = XmlHelper::QSizeFFromString(zoneScanningMax);
    bRet &= XML_READ_UNIQUE(qreal, m_speedMaxMMS);

    // -- Composition
    // Vector
    if(nullptr != m_pVector) {
        delete m_pVector;
        m_pVector = nullptr;
    }
    bool bOk;
    QDomElement eVector = p_pParent->firstChildElement("Vector");
    if( !eVector.isNull() ) {
        int iVectorType = eVector.firstChildElement("_type").text().toInt(&bOk);
        if( bOk ) {
            m_pVector = Vector::makeVector(iVectorType);
            if( nullptr != m_pVector ) {
                bRet &= m_pVector->loadParam(p_pParent);
            }
        }
        bRet &= (nullptr != m_pVector);
    } else {
        bRet = false;
    }

    // Probe
    if(nullptr != m_pProbe) {
        delete m_pProbe;
        m_pProbe = nullptr;
    }
    QDomElement eProbe = p_pParent->firstChildElement("Probe");
    if( !eProbe.isNull() ) {
        int iProbeType = eProbe.firstChildElement("_type").text().toInt(&bOk);
        if( bOk ) {
            m_pProbe = Probe::makeProbe(iProbeType);
            if( nullptr != m_pProbe ) {
                bRet &= m_pProbe->loadParam(p_pParent);
            }
        }
        bRet &= (nullptr != m_pProbe);
    } else {
        bRet = false;
    }

    if(nullptr == m_pCoupling) {
        m_pCoupling = new Coupling();
    }
    bRet &= m_pCoupling->loadParam(p_pParent);

    return bRet;
}

bool Equipment::customLoadDefaultParam()
{
    bool bRet = true;
    m_type = EquipmentTypeStd::TYPE_UTPOLE_USKEY;
    m_resolutionMax = QPointF(1,1);
    m_frequencyAcqMax = 1;
    m_zoneScanningMax = QSize(1,1);
    m_speedMaxMMS = 1;      

    if( nullptr != m_pVector ) {
        delete m_pVector;
    }
    m_pVector = Vector::makeVector(Vector::TYPE_UTPOLE);
    m_pVector->customLoadDefaultParam();

    if( nullptr != m_pProbe ) {
        delete m_pProbe;
    }
    m_pProbe = Probe::makeProbe(Probe::TYPE_USKEY);
    m_pProbe->customLoadDefaultParam();

    if( nullptr != m_pCoupling ) {
        delete m_pCoupling;
    }
    m_pCoupling = new Coupling();
    m_pCoupling->customLoadDefaultParam();

    return bRet;
}

bool Equipment::customApplyParam()
{
    bool bRet = true;

    // compute property values
    bRet &= computeProperties();

    if( nullptr != m_pVector ) {
        bRet &= m_pVector->customApplyParam();
    }
    if( nullptr != m_pProbe ) {
        bRet &= m_pProbe->customApplyParam();
    }
    if( nullptr != m_pCoupling ) {
        bRet &= m_pCoupling->customApplyParam();
    }
    return bRet;
}

bool Equipment::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    QString resolutionMax, zoneScanningMax;
    bRet &= XML_WRITE_UNIQUE(m_type);
    zoneScanningMax = XmlHelper::QSizeFToString(m_zoneScanningMax);
    bRet &= XML_WRITE_UNIQUE(zoneScanningMax);
    bRet &= XML_WRITE_UNIQUE(m_frequencyAcqMax);   
    resolutionMax = XmlHelper::QPointFToString(m_resolutionMax);
    bRet &= XML_WRITE_UNIQUE(resolutionMax);
    bRet &= XML_WRITE_UNIQUE(m_speedMaxMMS);            

    if(nullptr != m_pVector) {
        bRet &= m_pVector->saveParam(p_pDoc, p_pParent);
    }
    if(nullptr != m_pProbe) {
        bRet &= m_pProbe->saveParam(p_pDoc, p_pParent);
    }
    if(nullptr != m_pCoupling) {
        bRet &= m_pCoupling->saveParam(p_pDoc, p_pParent);
    }
    return bRet;
}

// -- Behaviour
Equipment * Equipment::makeNewEquipmentStd(quint32 p_type)
{
    Equipment * pEqu = nullptr;

    switch(p_type) {
        case TYPE_UTPOLE_USKEY :
            pEqu = makeNewEquipment(Vector::VectorType::TYPE_UTPOLE, Probe::ProbeType::TYPE_USKEY);
            break;
        case TYPE_ALTISCAN_USKEY :
            pEqu = makeNewEquipment(Vector::VectorType::TYPE_ALTISCAN, Probe::ProbeType::TYPE_USKEY);
            break;
        default :
            Logger::logError(QString("Equipment * Equipment::makeNewEquipmentStd : unknow type %1").arg(p_type));
    }
    return pEqu;
}

Equipment * Equipment::makeNewEquipment(quint32 p_typeVector, quint32 p_typeProbe)
{
    Equipment * pEqu = nullptr;
    Vector * pVector = Vector::makeVector(p_typeVector);
    Probe * pProbe = Probe::makeProbe(p_typeProbe);

    if( nullptr != pVector &&  nullptr != pProbe) {
        pEqu = new Equipment(pVector, pProbe, new Coupling());
    } else {
        Logger::logError(QString("Equipment * Equipment::makeNewEquipment fail to instanciate Equipment (%1,%2)").arg(p_typeVector).arg(p_typeProbe));
    }
    return pEqu;
}

// -- Behaviour
bool Equipment::computeProperties()
{
    // \todo
    // Compute :
    //    QPointF m_resolutionMax;
    //    qreal m_frequencyAcqMax;
    //    QSizeF m_zoneScanningMax;  ///< take care of the probe geometry and the vector sweeping zone.
    //    qreal m_speedMaxMMS;        ///< max speed, in millimeters/second
    return true;
}
