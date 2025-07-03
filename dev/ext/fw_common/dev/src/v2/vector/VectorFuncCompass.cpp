/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncCompass.cpp
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncCompass.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncCompass

VectorFuncCompass::VectorFuncCompass(VectorStd *p_pVectorStd) : GenVectorFunc(QString("VectorFuncCompass"), p_pVectorStd, ProtMsg::DEVICE_FUNC_COMPASS)
{

}

VectorFuncCompass::~VectorFuncCompass()
{

}

void VectorFuncCompass::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DCompassDegr, quint16, 2, FUNC_COMPASS_PARAM_D_COMPASS_DEGR)
}

void VectorFuncCompass::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();
}

// -- Inherited from XmlProject
bool VectorFuncCompass::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDCompassDegr, 0);
    return bRet;
}

bool VectorFuncCompass::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDCompassDegr = 0;
    return bRet;
}

bool VectorFuncCompass::customApplyParam()
{
    bool bRet = true;
    emit paramDCompassDegrChanged(m_paramDCompassDegr);
    return bRet;
}

bool VectorFuncCompass::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDCompassDegr);
    return bRet;
}


void VectorFuncCompass::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_COMPASS_PARAM_D_COMPASS_DEGR));
    }
}
