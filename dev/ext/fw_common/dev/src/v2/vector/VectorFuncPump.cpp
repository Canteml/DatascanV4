/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPump.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncPump.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncPump

VectorFuncPump::VectorFuncPump(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncPump", p_pVectorStd, ProtMsg::DEVICE_FUNC_PUMP)
{
    connect(this, &VectorFuncPump::paramLPumpEnableChanged, this, &VectorFuncPump::onLPumpEnableChanged, Qt::DirectConnection);
    connect(this, &VectorFuncPump::paramLPumpLevelChanged, this, &VectorFuncPump::onLPumpLevelChanged, Qt::DirectConnection);
}

VectorFuncPump::~VectorFuncPump()
{

}

void VectorFuncPump::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DPump, quint8, 1, FUNC_PUMP_PARAM_D_PUMP)
    DECODE_MSG_PARAM(DTankLevel, quint8, 1, FUNC_PUMP_PARAM_D_TANK_LVL)
    DECODE_MSG_PARAM(LPumpEnable, quint8, 1, FUNC_PUMP_PARAM_L_PUMP_ENABLE)
    DECODE_MSG_PARAM(LPumpLevel, quint8, 1, FUNC_PUMP_PARAM_L_PUMP_LEVEL)
}

void VectorFuncPump::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    // For all products :
    // reset parameters :
    if( !m_pVectorStd->isMpc() ) {
        setParamDPump(0);
        setParamDTankLevel(0);
        setParamLPumpEnable(0);
        setParamLPumpLevel(0);
    }

    onDeviceConnected();
}

void VectorFuncPump::onLPumpEnableChanged(quint8)
{
    setPumpWithLogical();
}

void VectorFuncPump::onLPumpLevelChanged(quint8)
{
    setPumpWithLogical();
}

// -- Inherited from XmlProject
bool VectorFuncPump::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDPump, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDTankLevel, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLPumpEnable, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLPumpLevel, 0);
    return bRet;
}

bool VectorFuncPump::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDPump = 0;
    m_paramDTankLevel = 0;
    m_paramLPumpEnable = 0;
    m_paramLPumpLevel = 0;
    return bRet;
}

bool VectorFuncPump::customApplyParam()
{
    bool bRet = true;    
    emit paramDPumpChanged(m_paramDPump);
    emit paramDTankLevelChanged(m_paramDTankLevel);
    emit paramLPumpEnableChanged(m_paramLPumpEnable);
    emit paramLPumpLevelChanged(m_paramLPumpLevel);
    return bRet;
}

bool VectorFuncPump::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDPump);
    bRet &= XML_WRITE_UNIQUE(m_paramDTankLevel);
    bRet &= XML_WRITE_UNIQUE(m_paramLPumpEnable);
    bRet &= XML_WRITE_UNIQUE(m_paramLPumpLevel);
    return bRet;
}


void VectorFuncPump::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_PUMP_PARAM_D_PUMP));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_PUMP_PARAM_D_TANK_LVL));
    }
}

void VectorFuncPump::setPumpWithLogical()
{
    if( m_paramLPumpEnable ) {
        setParamDPump(m_paramLPumpLevel);
    } else {
        setParamDPump(0);
    }
}

