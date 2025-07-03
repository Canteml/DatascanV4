/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncEndStop.cpp
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncEndStop.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

VectorFuncEndStop::VectorFuncEndStop(VectorStd* p_pVectorStd)
    : GenVectorFunc(QString("VectorFuncEndStop"), p_pVectorStd, ProtMsg::DEVICE_FUNC_ENDSTOP)
{

}

VectorFuncEndStop::~VectorFuncEndStop()
{

}

void VectorFuncEndStop::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(LCollision, quint8, 1, FUNC_ENDSTOP_PARAM_L_COLLISION)
    if( ProtMsg::MSG_TYPE_EVENT != p_pMsg->m_msgType ) {
        return;
    }
    if( static_cast<quint16>(ProtMsg::FUNC_ENDSTOP_EVENT_E_ENDSTOP) != p_pMsg->m_msgObj ) {
        return;
    }
    if( true == p_pMsg->m_payload[0] ) {
        setParamLCollision(true);
    } else {
        setParamLCollision(false);
    }
}

void VectorFuncEndStop::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();
}

bool VectorFuncEndStop::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    m_paramLCollision = false;
    return bRet;
}

bool VectorFuncEndStop::customLoadDefaultParam()
{
    bool bRet = true;
    return bRet;
}

bool VectorFuncEndStop::customApplyParam()
{
    bool bRet = true;
    return bRet;
}

bool VectorFuncEndStop::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    return bRet;
}

void VectorFuncEndStop::onDeviceConnected()
{

}



