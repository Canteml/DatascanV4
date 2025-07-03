/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncLight.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncLight.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncLight

VectorFuncLight::VectorFuncLight(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncLight", p_pVectorStd, ProtMsg::DEVICE_FUNC_LIGHT)
{
    connect(this, &VectorFuncLight::paramLLightEnableChanged, this, &VectorFuncLight::onLLightEnableChanged, Qt::DirectConnection);
    connect(this, &VectorFuncLight::paramLLightLevelChanged, this, &VectorFuncLight::onLLightLevelChanged, Qt::DirectConnection);
}

VectorFuncLight::~VectorFuncLight()
{

}

void VectorFuncLight::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DLight, quint8, 1, FUNC_LIGHT_PARAM_D_LIGHT)
    DECODE_MSG_PARAM(LLightEnable, quint8, 1, FUNC_LIGHT_PARAM_L_LIGHT_ENABLE)
    DECODE_MSG_PARAM(LLightLevel, quint8, 1, FUNC_LIGHT_PARAM_L_LIGHT_LEVEL)
}

void VectorFuncLight::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    // For all products :
    // reset parameters :
    if( !m_pVectorStd->isMpc() ) {
        setParamDLight(0);
        setParamLLightEnable(0);
        setParamLLightLevel(0);
    }

    onDeviceConnected();
}

void VectorFuncLight::onLLightEnableChanged(quint8)
{
    setLightWithLogical();
}

void VectorFuncLight::onLLightLevelChanged(quint8)
{
    setLightWithLogical();
}

// -- Inherited from XmlProject
bool VectorFuncLight::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDLight, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLLightEnable, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLLightLevel, 0);
    return bRet;
}

bool VectorFuncLight::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDLight = 0;
    m_paramLLightEnable = 0;
    m_paramLLightLevel = 0;
    return bRet;
}

bool VectorFuncLight::customApplyParam()
{
    bool bRet = true;    
    emit paramDLightChanged(m_paramDLight);
    emit paramLLightEnableChanged(m_paramLLightEnable);
    emit paramLLightLevelChanged(m_paramLLightLevel);
    return bRet;
}

bool VectorFuncLight::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDLight);
    bRet &= XML_WRITE_UNIQUE(m_paramLLightEnable);
    bRet &= XML_WRITE_UNIQUE(m_paramLLightLevel);
    return bRet;
}


void VectorFuncLight::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_LIGHT_PARAM_D_LIGHT));
    }
}

void VectorFuncLight::setLightWithLogical()
{
    if( m_paramLLightEnable ) {
        setParamDLight(m_paramLLightLevel);
    } else {
        setParamDLight(0);
    }
}

