/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncBattery.cpp
# Author      : Thierry Lechevallier
# Date        : 02/11/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncBattery.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncPump

VectorFuncBattery::VectorFuncBattery(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncBattery", p_pVectorStd, ProtMsg::DEVICE_FUNC_BATTERY)
{

}

VectorFuncBattery::~VectorFuncBattery()
{

}

void VectorFuncBattery::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DBatteryLvlP, quint8, 1, FUNC_BATTERY_PARAM_D_BATTERY_LVL_P)
}

void VectorFuncBattery::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    setParamDBatteryLvlP(NO_BATTETY);

    onDeviceConnected();
}

// -- Inherited from XmlProject
bool VectorFuncBattery::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDBatteryLvlP, 0);
    return bRet;
}

bool VectorFuncBattery::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDBatteryLvlP = 0;
    return bRet;
}

bool VectorFuncBattery::customApplyParam()
{
    bool bRet = true;    
    emit paramDBatteryLvlPChanged(m_paramDBatteryLvlP);
    return bRet;
}

bool VectorFuncBattery::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDBatteryLvlP);
    return bRet;
}


void VectorFuncBattery::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_BATTERY_PARAM_D_BATTERY_LVL_P));
    }
}

