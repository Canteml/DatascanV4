/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncCleaning.cpp
# Author      : Tanguy Dubois
# Date        : 04/08/2022
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncCleaning.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncCleaning

VectorFuncCleaning::VectorFuncCleaning(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncCleaning", p_pVectorStd, ProtMsg::DEVICE_FUNC_CLEANING)
{
    connect(this, &VectorFuncCleaning::paramLBrushEnableChanged, this, &VectorFuncCleaning::onLBrushEnableChanged, Qt::DirectConnection);
    connect(this, &VectorFuncCleaning::paramLBrushLevelChanged, this, &VectorFuncCleaning::onLBrushLevelChanged, Qt::DirectConnection);
    connect(this, &VectorFuncCleaning::paramLPumpEnableChanged, this, &VectorFuncCleaning::onLPumpEnableChanged, Qt::DirectConnection);
    connect(this, &VectorFuncCleaning::paramLPumpLevelChanged, this, &VectorFuncCleaning::onLPumpLevelChanged, Qt::DirectConnection);
}

VectorFuncCleaning::~VectorFuncCleaning()
{

}

void VectorFuncCleaning::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DBrush, quint8, 1, FUNC_CLEANING_PARAM_D_CLEANING_BRUSH)
    DECODE_MSG_PARAM(DPump, quint8, 1, FUNC_CLEANING_PARAM_D_CLEANING_PUMP)
    DECODE_MSG_PARAM(DTankLevel, quint8, 1, FUNC_CLEANING_PARAM_D_TANK_LVL)
    DECODE_MSG_PARAM(LBrushEnable, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_ENABLE)
    DECODE_MSG_PARAM(LBrushLevel, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_LEVEL)
    DECODE_MSG_PARAM(LPumpEnable, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_PUMP_ENABLE)
    DECODE_MSG_PARAM(LPumpLevel, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_PUMP_LEVEL)
}

void VectorFuncCleaning::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    // For all products :
    // reset parameters :
    if( !m_pVectorStd->isMpc() ) {
        setParamDBrush(0);
        setParamDPump(0);
        setParamDTankLevel(0);
        setParamLBrushEnable(0);
        setParamLBrushLevel(0);
        setParamLPumpEnable(0);
        setParamLPumpLevel(0);
    }

    onDeviceConnected();
}

void VectorFuncCleaning::onLBrushEnableChanged(quint8)
{
    setBrushWithLogical();
}

void VectorFuncCleaning::onLBrushLevelChanged(quint8)
{
    setBrushWithLogical();
}

void VectorFuncCleaning::onLPumpEnableChanged(quint8)
{
    setPumpWithLogical();
}

void VectorFuncCleaning::onLPumpLevelChanged(quint8)
{
    setPumpWithLogical();
}

// -- Inherited from XmlProject
bool VectorFuncCleaning::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDBrush, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDPump, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDTankLevel, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLBrushEnable, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLBrushLevel, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLPumpEnable, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLPumpLevel, 0);
    return bRet;
}

bool VectorFuncCleaning::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDBrush = 0;
    m_paramDPump = 0;
    m_paramDTankLevel = 0;
    m_paramLBrushEnable = 0;
    m_paramLBrushLevel = 0;
    m_paramLPumpEnable = 0;
    m_paramLPumpLevel = 0;
    return bRet;
}

bool VectorFuncCleaning::customApplyParam()
{
    bool bRet = true;
    emit paramDBrushChanged(m_paramDBrush);
    emit paramDPumpChanged(m_paramDPump);
    emit paramDTankLevelChanged(m_paramDTankLevel);
    emit paramLBrushEnableChanged(m_paramLBrushEnable);
    emit paramLBrushLevelChanged(m_paramLBrushLevel);
    emit paramLPumpEnableChanged(m_paramLPumpEnable);
    emit paramLPumpLevelChanged(m_paramLPumpLevel);
    return bRet;
}

bool VectorFuncCleaning::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDBrush);
    bRet &= XML_WRITE_UNIQUE(m_paramDPump);
    bRet &= XML_WRITE_UNIQUE(m_paramDTankLevel);
    bRet &= XML_WRITE_UNIQUE(m_paramLBrushEnable);
    bRet &= XML_WRITE_UNIQUE(m_paramLBrushLevel);
    bRet &= XML_WRITE_UNIQUE(m_paramLPumpEnable);
    bRet &= XML_WRITE_UNIQUE(m_paramLPumpLevel);
    return bRet;
}


void VectorFuncCleaning::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_CLEANING_PARAM_D_CLEANING_BRUSH));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_CLEANING_PARAM_D_CLEANING_PUMP));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_CLEANING_PARAM_D_TANK_LVL));
    }
}

void VectorFuncCleaning::setBrushWithLogical()
{
    if( m_paramLBrushEnable ) {
        setParamDBrush(m_paramLBrushLevel);
    } else {
        setParamDBrush(0);
    }
}

void VectorFuncCleaning::setPumpWithLogical()
{
    if( m_paramLPumpEnable ) {
        setParamDPump(m_paramLPumpLevel);
    } else {
        setParamDPump(0);
    }
}
