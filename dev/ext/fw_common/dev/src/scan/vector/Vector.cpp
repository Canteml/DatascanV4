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
#include <scan/vector/Vector.h>
#include <scan/vector/VectorLinear.h>
#include <scan/vector/VectorAltiscan.h>


// ----------------------------------------------------------------------------
// -- Class VectorParam

VectorParam::VectorParam() : XmlProjectLoggable("VectorParam")
{

}

VectorParam::VectorParam(const VectorParam & p_value) : QObject(), XmlProjectLoggable("VectorParam")
{
    (*this) = p_value;
}

VectorParam::~VectorParam()
{

}

quint32 VectorParam::getId(void) const
{
    return m_ID;
}

void VectorParam::setId(quint32 p_value)
{
    m_ID = p_value;
}


// -- XmlProject
bool VectorParam::customLoadParam(QDomElement * )
{
    return true;
}

bool VectorParam::customLoadDefaultParam()
{
    return true;
}

bool VectorParam::customApplyParam()
{
    return true;
}

bool VectorParam::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    quint32 _type = getVectorType();
    XML_WRITE_UNIQUE(_type);  // save type to identify the item when loading project
    return true;
}


// -- operator
VectorParam & VectorParam::operator= (const VectorParam & p_value)
{
    if (this != &p_value) {
        m_ID = p_value.getId();
    }
    return *this;
}

bool VectorParam::operator== (const VectorParam & p_value)
{
    return m_ID == p_value.getId();
}

bool VectorParam::operator!=(const VectorParam &p_value)
{
    return !(*this == p_value);
}


// ----------------------------------------------------------------------------
// -- Class VectorCalibrate

VectorCalibrate::VectorCalibrate()
{

}

VectorCalibrate::~VectorCalibrate()
{

}

// -- XmlProject
bool VectorCalibrate::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= Calibrate::customLoadParam(p_pParent);
    return bRet;
}

bool VectorCalibrate::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= Calibrate::customLoadDefaultParam();
    return bRet;
}

bool VectorCalibrate::customApplyParam()
{
    bool bRet = true;
    bRet &= Calibrate::customApplyParam();
    return bRet;
}

bool VectorCalibrate::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= Calibrate::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}

// -- operator
VectorCalibrate & VectorCalibrate::operator= (const VectorCalibrate & p_value)
{
    if( this != &p_value ) {

    }
    return (*this);
}

bool VectorCalibrate::operator== (const VectorCalibrate &)
{
    return true;
}

bool VectorCalibrate::operator!= (const VectorCalibrate & p_value)
{
    return !((*this) == p_value);
}

// ----------------------------------------------------------------------------
// -- Class Vector

QString Vector::getVectorTypeName(quint32 e)
{
    const std::map<quint32,const char*> MyEnumStrings {
           { Vector::VectorType::TYPE_UNDEF, tr("UNDEFINED").toStdString().data() },
           { Vector::VectorType::TYPE_UTPOLE, "UT POLE" },
           { Vector::VectorType::TYPE_ALTISCAN, "ALTISCAN" }
    };
    auto   it  = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? QString("Out of range") : QString(it->second);
}


Vector::Vector(VectorParam * p_pVectorParam, VectorCalibrate * p_pVectorCalibrate) : QObject(), XmlProjectLoggable("Vector")
{
    m_pVectorParam = p_pVectorParam;
    m_pVectorCalibrate = p_pVectorCalibrate;
}

Vector::~Vector()
{
    if( nullptr != m_pVectorParam ) {
        //delete m_pVectorParam;
        m_pVectorParam = nullptr;
    }

    if( nullptr != m_pVectorCalibrate ) {
        //delete m_pVectorCalibrate;
        m_pVectorCalibrate = nullptr;
    }
}

void Vector::init()
{
    customLoadDefaultParam();
}

quint32 Vector::getVectorType(void) const
{
    return m_pVectorParam->getVectorType();
}

QString Vector::getVectorName(void) const
{
    return getVectorTypeName(m_pVectorParam->getVectorType());
}


VectorParam* Vector::getParam()
{
    return m_pVectorParam;
}

VectorCalibrate* Vector::getCalibrate()
{
    return m_pVectorCalibrate;
}


// -- XmlProject
bool Vector::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;

    bRet &= XML_READ_UNIQUE(QString, m_vectorBrand);
    bRet &= XML_READ_UNIQUE(QString, m_vectorBrandType);
    bRet &= XML_READ_UNIQUE(QString, m_vectorSerialNumber);

    QString _vectorProbeOffset;
    QString _vectorZoneSweeping;
    QString _vectorSize;

    bRet &= XML_READ_UNIQUE(QString, _vectorProbeOffset);
    m_vectorProbeOffset = XmlHelper::QPointFFromString(_vectorProbeOffset);
    bRet &= XML_READ_UNIQUE(QString, _vectorZoneSweeping);
    m_vectorZoneSweeping = XmlHelper::QRectFFromString(_vectorZoneSweeping);
    bRet &= XML_READ_UNIQUE(QString, _vectorSize);
    m_vectorSize = XmlHelper::QSizeFFromString(_vectorSize);

    bRet &= m_pVectorParam->loadParam(p_pParent);
    bRet &= m_pVectorCalibrate->loadParam(p_pParent);
    return bRet;
}

bool Vector::customLoadDefaultParam()
{
    bool bRet = true;

    // -- Param References
    m_vectorBrand = "ROBOPLANET";
    m_vectorBrandType = "";
    m_vectorSerialNumber = "";

    // -- Param scan
    m_vectorProbeOffset = QPointF(0,0);
    m_vectorZoneSweeping = QRectF(0,0,0,0);
    m_vectorSize = QSizeF(0,0);

    m_position = QPointF(0,0);

    bRet &= m_pVectorParam->customLoadDefaultParam();
    bRet &= m_pVectorCalibrate->customLoadDefaultParam();
    return bRet;
}

bool Vector::customApplyParam()
{
    bool bRet = true;
    bRet &= m_pVectorParam->customApplyParam();
    bRet &= m_pVectorCalibrate->customApplyParam();
    return bRet;
}

bool Vector::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;    
    quint32 _type = m_pVectorParam->getVectorType();
    bRet &= XML_WRITE_UNIQUE(_type);    // write type to be loaded next time
    bRet &= XML_WRITE_UNIQUE(m_vectorBrand);
    bRet &= XML_WRITE_UNIQUE(m_vectorBrandType);
    bRet &= XML_WRITE_UNIQUE(m_vectorSerialNumber);

    QString _vectorProbeOffset;
    QString _vectorZoneSweeping;
    QString _vectorSize;

    _vectorProbeOffset = XmlHelper::QPointFToString(m_vectorProbeOffset);
    bRet &= XML_WRITE_UNIQUE(_vectorProbeOffset);
    _vectorZoneSweeping = XmlHelper::QRectFToString(m_vectorZoneSweeping);
    bRet &= XML_WRITE_UNIQUE(_vectorZoneSweeping);
    _vectorSize = XmlHelper::QSizeFToString(m_vectorSize);
    bRet &= XML_WRITE_UNIQUE(_vectorSize);

    bRet &= m_pVectorParam->saveParam(p_pDoc, p_pParent);
    bRet &= m_pVectorCalibrate->saveParam(p_pDoc, p_pParent);
    return bRet;
}


// -- Properties
qreal Vector::getX() const
{
    return m_position.x();
}

void Vector::setX(qreal p_value)
{
    m_position.setX(p_value);
}

qreal Vector::getY() const
{
    return m_position.y();
}

void Vector::setY(qreal p_value)
{
    m_position.setY(p_value);
}

QPointF Vector::getPos() const
{
    return m_position;
}

void Vector::setPos(const QPointF & p_value)
{
    m_position = p_value;
}

// -- Composition
VectorParam *Vector::getVectorParam() const
{
    return m_pVectorParam;
}

void Vector::setVectorParam(const VectorParam & p_value)
{
    if( m_pVectorParam->getVectorType() != p_value.getVectorType() ) {
        fLogWarning(QString("Type mismatch %1 != %2").arg(m_pVectorParam->getVectorType()).arg(p_value.getVectorType()));
    }

    (*m_pVectorParam) = p_value;
}

VectorCalibrate *Vector::getVectorCalibrate() const
{
    return m_pVectorCalibrate;
}

void Vector::setVectorCalibrate(const VectorCalibrate & p_value)
{
    (*m_pVectorCalibrate) = p_value;
}

// -- References
QString Vector::getVectorBrand() const
{
    return m_vectorBrand;
}

void Vector::setVectorBrand(const QString & p_value)
{
    m_vectorBrand = p_value;
}

QString Vector::getVectorBrandType() const
{
    return m_vectorBrandType;
}

void Vector::setVectorBrandType(const QString & p_value)
{
    m_vectorBrandType = p_value;
}

QString Vector::getVectorSerialNumber() const
{
    return m_vectorSerialNumber;
}

void Vector::setVectorSerialNumber(const QString & p_value)
{
    m_vectorSerialNumber = p_value;
}

QPointF Vector::getVectorProbeOffset() const
{
    return m_vectorProbeOffset;
}

void Vector::setVectorProbeOffset(const QPointF & p_value)
{
    m_vectorProbeOffset = p_value;
}

QRectF Vector::getVectorZoneSweeping() const
{
    return m_vectorZoneSweeping;
}

void Vector::setZoneSweeping(const QRectF &p_value)
{
    m_vectorZoneSweeping = p_value;
}

QSizeF Vector::getVectorSize() const
{
    return m_vectorSize;
}

void Vector::setVectorSize(const QSizeF & p_value)
{
    m_vectorSize = p_value;
}


// -- Behaviour

Vector *Vector::makeVector(quint32 p_type)
{
    Vector * pVector = nullptr;
    switch(p_type) {
        case TYPE_UTPOLE :
            pVector = new VectorLinear();
            break;
        case TYPE_ALTISCAN :
            pVector = new VectorAltiscan();
            break;
        default :
            Logger::logError(QString("Vector *Vector::makeProbe type not defined : %1").arg(p_type));
    }

    if( nullptr != pVector ) {
        pVector->customLoadDefaultParam();
    }
    return pVector;
}

Vector &Vector::operator=(const Vector &p_value)
{
    if( p_value.getVectorType() != getVectorType() ) {
       fLogWarning(QString("Trying to compare 2 affect 2 vector with different type (%1,%2)").arg(getVectorType()).arg(p_value.getVectorType()));
    } else {
        if( this != &p_value ) {
            // -- Param References
            setVectorBrand(p_value.getVectorBrand());
            setVectorBrandType(p_value.getVectorBrandType());
            setVectorSerialNumber(p_value.getVectorSerialNumber());

            // -- Param scan
            m_vectorProbeOffset = p_value.getVectorProbeOffset();
            m_vectorZoneSweeping = p_value.getVectorZoneSweeping();
            m_vectorSize = p_value.getVectorSize();

            // -- Properties
            setPos(p_value.getPos());

            // -- Composition
            setVectorParam(*p_value.getVectorParam());
            setVectorCalibrate(*p_value.getVectorCalibrate());
        }
    }

    return (*this);
}

bool Vector::operator==(const Vector &p_value)
{
    bool bRet = true;
    if( p_value.getVectorType() != getVectorType() ) {
       fLogWarning(QString("Trying to compare 2 affect 2 vector with different type (%1,%2)").arg(getVectorType()).arg(p_value.getVectorType()));
       bRet = false;
    } else {
        if( this != &p_value ) {
            if( p_value.getVectorBrand() != m_vectorBrand ) {
                bRet = false;
            } else if( p_value.getVectorBrand() != getVectorBrand() ) {
                bRet = false;
            } else if( p_value.getVectorBrandType() != getVectorBrandType() ) {
                bRet = false;
            } else if( p_value.getVectorSerialNumber() != getVectorSerialNumber() ) {
                bRet = false;
            } else if( p_value.getVectorProbeOffset() != getVectorProbeOffset() ) {
                bRet = false;
            } else if( p_value.getVectorZoneSweeping() != getVectorZoneSweeping() ) {
                bRet = false;
            } else if( p_value.getVectorSize() != getVectorSize() ) {
                bRet = false;
            } else if( p_value.getPos() != getPos() ) {
                bRet = false;
            } else if( *(p_value.getVectorParam()) != (*getVectorParam()) ) {
                bRet = false;
            } else if( *p_value.getVectorCalibrate() != (*getVectorCalibrate()) ) {
                bRet = false;
            }
        }
    }
    return bRet;
}


