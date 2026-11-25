/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncLicence.cpp
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncLicence.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/GenComLink.h>

// -- Qt includes
#include <QDateTime>


VectorFuncLicence::VectorFuncLicence(VectorStd *p_pVectorStd)
    : GenVectorFunc("VectorFuncLicence", p_pVectorStd, ProtMsg::DEVICE_FUNC_LICENCE)
{

}

VectorFuncLicence::~VectorFuncLicence()
{

}

void VectorFuncLicence::processReceivedMsg(ProtMsg *p_pMsg)
{
    //DECODE_MSG_PARAM(DStatusLicenceP, quint8, 1, FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P)
    if( nullptr != m_pComLink && m_pComLink->isProtocolD4() ) {
        if( ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType
            && ProtMsg::FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P == p_pMsg->m_msgObj ) {
            setParamDStatusLicenceP(p_pMsg->m_payload[0], true);
        }
    } else {
        setParamDStatusLicenceP(100, true);
    }
    if(ProtMsg::MSG_TYPE_PARAM_SET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType) {
        if(ProtMsg::FUNC_LICENCE_PARAM_D_LICENCE_KEY_EQUIPMENT == p_pMsg->m_msgObj) {
            m_keyEquipment = QString().fromLatin1((char*)p_pMsg->m_payload, p_pMsg->m_len);
            emit keyEquipmentChanged(m_keyEquipment);
        }
    }

}

void VectorFuncLicence::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    // For all products :
    // reset parameters :
    // setParamDStatusLicenceP(0);

    onDeviceConnected();
}

void VectorFuncLicence::configureNewLicence(QString p_licenceKey)
{
    const QByteArray key = p_licenceKey.toLatin1();
    if( key.size() != LICENCE_KEY_LENGTH ) {
        fLogError(QString("Chiave di licenza rifiutata : %1 caratteri invece di %2").arg(key.size()).arg(LICENCE_KEY_LENGTH));
        return;
    }
    ProtMsg *msgNewLicence = newMsgCmd(ProtMsg::FUNC_LICENCE_CMD_D_NEW_LICENCE);
    msgNewLicence->m_len = LICENCE_KEY_LENGTH;
    memcpy(msgNewLicence->m_payload, key.constData(), LICENCE_KEY_LENGTH);
    sendMsg(msgNewLicence);
}

void VectorFuncLicence::askForKeyEquipment()
{
    sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_LICENCE_PARAM_D_LICENCE_KEY_EQUIPMENT));
}

void VectorFuncLicence::askForStatusLicence()
{
    sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P));
}

QString VectorFuncLicence::getKeyEquipment()
{
    return m_keyEquipment;
}

bool VectorFuncLicence::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramDStatusLicenceP, 0);
    return bRet;
}

bool VectorFuncLicence::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDStatusLicenceP = 0;
    return bRet;
}

bool VectorFuncLicence::customApplyParam()
{
    bool bRet = true;
    emit paramDStatusLicencePChanged(m_paramDStatusLicenceP);
    return bRet;
}

bool VectorFuncLicence::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDStatusLicenceP);
    return bRet;
}

void VectorFuncLicence::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_LICENCE_PARAM_D_STATUS_LICENCE_P));
    }
}
