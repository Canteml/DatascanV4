/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : VectorLinear.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#include <scan/vector/VectorLinear.h>

// ----------------------------------------------------------------------------
// -- Class VectorLinearParam


VectorLinearParam::VectorLinearParam()
{

}

VectorLinearParam::VectorLinearParam(const VectorLinearParam & p_value) : VectorParam(p_value)
{
    (*this = p_value);
}

///< copy constructor
VectorLinearParam::~VectorLinearParam()
{

}

VectorParam* VectorLinearParam::copy() const
{
    return new VectorLinearParam(*this);
}


quint32 VectorLinearParam::getCoderResolution(void) const
{
    return m_coderResolution;
}

void VectorLinearParam::setCoderResolution(quint32 p_value)
{
    m_coderResolution = p_value;
}

qreal VectorLinearParam::getWheelDiameter(void) const
{
    return m_wheelDiameter;
}

void VectorLinearParam::setWheelDiameter(qreal p_value)
{
    m_wheelDiameter = p_value;
}

quint32 VectorLinearParam::getVectorType() const
{
    return Vector::TYPE_UTPOLE;
}

// -- XmlProject
bool VectorLinearParam::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(quint32, m_coderResolution);
    bRet &= XML_READ_UNIQUE(qreal, m_wheelDiameter);
    bRet &= VectorParam::customLoadParam(p_pParent);
    return bRet;
}

bool VectorLinearParam::customLoadDefaultParam()
{
    bool bRet = true;
    m_coderResolution = 1;
    m_wheelDiameter = 1;
    bRet &= VectorParam::customLoadDefaultParam();
    return bRet;
}

bool VectorLinearParam::customApplyParam()
{
    bool bRet = true;
    bRet &= VectorParam::customApplyParam();
    return bRet;
}

bool VectorLinearParam::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_coderResolution);
    bRet &= XML_WRITE_UNIQUE(m_wheelDiameter);
    bRet &= VectorParam::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


// -- operator

VectorLinearParam & VectorLinearParam::operator= (const VectorLinearParam & p_value)
{
    if(this != &p_value) {
        m_coderResolution = p_value.getCoderResolution();
        m_wheelDiameter = p_value.getWheelDiameter();
        VectorParam::operator=(p_value);
    }
    return (*this);
}

bool VectorLinearParam::operator== (const VectorLinearParam & p_value)
{
    if( m_coderResolution == p_value.getCoderResolution()  && m_wheelDiameter == p_value.getWheelDiameter() ) {
        if( VectorParam::operator==(p_value) ) {
            return true;
        }
    }
    return false;
}


// ----------------------------------------------------------------------------
// -- Class VectorLinearCalibrate


VectorLinearCalibrate::VectorLinearCalibrate()
{

}

VectorLinearCalibrate::~VectorLinearCalibrate()
{

}

// -- XmlProject
bool VectorLinearCalibrate::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorCalibrate::customLoadParam(p_pParent);
    return bRet;
}

bool VectorLinearCalibrate::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= VectorCalibrate::customLoadDefaultParam();
    return bRet;
}

bool VectorLinearCalibrate::customApplyParam()
{
    bool bRet = true;
    bRet &= VectorCalibrate::customApplyParam();
    return bRet;
}

bool VectorLinearCalibrate::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorCalibrate::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


// ----------------------------------------------------------------------------
// -- Class VectorLinear

VectorLinear::VectorLinear() : Vector( &p_vectorLinearParam, &p_vectorLinearCalibrate)
{

}

VectorLinear::VectorLinear(VectorLinearParam * m_pVectorParam, VectorLinearCalibrate * m_pVectorCalibrate) :
    Vector(m_pVectorParam, m_pVectorCalibrate)
{
    p_vectorLinearParam = *m_pVectorParam;
    p_vectorLinearCalibrate = *m_pVectorCalibrate;
}

VectorLinear::~VectorLinear()
{

}

VectorLinearParam* VectorLinear::getParamLinear()
{
    return &p_vectorLinearParam;
}

VectorLinearCalibrate* VectorLinear::getCalibrateLinear()
{
    return &p_vectorLinearCalibrate;
}

// -- Parameters

// -- XmlProject
bool VectorLinear::customLoadParam(QDomElement * p_pParent)
{
    return Vector::customLoadParam(p_pParent);
}

bool VectorLinear::customLoadDefaultParam()
{
    return Vector::customLoadDefaultParam();
}

bool VectorLinear::customApplyParam()
{
    return Vector::customApplyParam();
}

bool VectorLinear::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    return Vector::customSaveParam(p_pDoc, p_pParent);
}


