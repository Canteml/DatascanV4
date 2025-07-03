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

#include <scan/vector/VectorAltiscan.h>


// ----------------------------------------------------------------------------
// -- Class VectorAltiscanParam


VectorAltiscanParam::VectorAltiscanParam()
{

}

VectorAltiscanParam::VectorAltiscanParam(const VectorAltiscanParam & p_value) : VectorLinearParam(p_value)
{
    (*this = p_value);
}

VectorAltiscanParam::~VectorAltiscanParam()
{

}

VectorParam* VectorAltiscanParam::copy() const
{
    return new VectorAltiscanParam(*this);
}

quint32 VectorAltiscanParam::getMotorReduction(void) const
{
    return m_motorReduction;
}

void VectorAltiscanParam::setMotorReduction(quint32 p_value)
{
    m_motorReduction = p_value;
}

quint32 VectorAltiscanParam::getVectorType() const
{
    return Vector::TYPE_ALTISCAN;
}


// -- XmlProject
bool VectorAltiscanParam::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    XML_READ_UNIQUE(quint32, m_motorReduction);
    bRet &= VectorLinearParam::customLoadParam(p_pParent);
    return bRet;
}

bool VectorAltiscanParam::customLoadDefaultParam()
{
    bool bRet = true;
    m_motorReduction = 1;
    bRet &= VectorLinearParam::customLoadDefaultParam();
    return bRet;
}

bool VectorAltiscanParam::customApplyParam()
{
    bool bRet = true;
    bRet &= VectorLinearParam::customApplyParam();
    return bRet;
}

bool VectorAltiscanParam::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_motorReduction);
    bRet &= VectorLinearParam::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


// -- operator
VectorAltiscanParam & VectorAltiscanParam::operator= (const VectorAltiscanParam & p_value)
{
    if(this != &p_value) {
        m_motorReduction = p_value.getMotorReduction();
        VectorLinearParam::operator=(p_value);
    }
    return (*this);
}

bool VectorAltiscanParam::operator== (const VectorAltiscanParam & p_value)
{
    if( m_motorReduction == p_value.getMotorReduction() ) {
        if( VectorLinearParam::operator==(p_value) ) {
            return true;
        }
    }
    return false;
}



// ----------------------------------------------------------------------------
// -- Class VectorAltiscanCalibrate


VectorAltiscanCalibrate::VectorAltiscanCalibrate()
{

}

VectorAltiscanCalibrate::~VectorAltiscanCalibrate()
{

}


// -- XmlProject
bool VectorAltiscanCalibrate::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorLinearCalibrate::customLoadParam(p_pParent);
    return bRet;
}

bool VectorAltiscanCalibrate::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= VectorLinearCalibrate::customLoadDefaultParam();
    return bRet;
}

bool VectorAltiscanCalibrate::customApplyParam()
{
    bool bRet = true;
    bRet &= VectorLinearCalibrate::customApplyParam();
    return bRet;
}

bool VectorAltiscanCalibrate::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorLinearCalibrate::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}



// ----------------------------------------------------------------------------
// -- Class VectorLinear


VectorAltiscan::VectorAltiscan() : VectorLinear( new VectorAltiscanParam(), new VectorAltiscanCalibrate())
{

}

VectorAltiscan::~VectorAltiscan()
{

}


VectorAltiscanParam* VectorAltiscan::getParamAltiscan()
{
    return static_cast<VectorAltiscanParam*>(m_pVectorParam);
}

VectorAltiscanCalibrate* VectorAltiscan::getCalibrateAltiscan()
{
    return static_cast<VectorAltiscanCalibrate*>(m_pVectorCalibrate);
}


// -- Parameters

// -- XmlProject
bool VectorAltiscan::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorLinear::customLoadParam(p_pParent);
    return bRet;
}

bool VectorAltiscan::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= VectorLinear::customLoadDefaultParam();
    return bRet;
}

bool VectorAltiscan::customApplyParam()
{
    bool bRet = true;
    bRet &= VectorLinear::customApplyParam();
    return bRet;
}

bool VectorAltiscan::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= VectorLinear::customSaveParam(p_pDoc, p_pParent);
    return bRet;
}


