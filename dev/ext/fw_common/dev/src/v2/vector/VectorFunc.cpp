/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorStd.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFunc.h>
#include <VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComDevice.h>
#include <v2/com/GenComLink.h>

// ----------------------------------------------------------------------------
// -- GenVectorFunc

bool GenVectorFunc::isVectorMpc() const {
    if (m_pVectorStd == nullptr) return false;
    return m_pVectorStd->isMpc();
}

GenVectorFunc::GenVectorFunc(const QString &p_functionName, VectorStd * p_pVectorStd, quint32 p_funcId) : QObject(p_pVectorStd), XmlProjectLoggable(p_functionName)
{
    qRegisterMetaType<GenComLink*>();

    m_pComLink = nullptr;
    m_pVectorStd = p_pVectorStd;
    m_pComDevice = nullptr;
    m_funcId = p_funcId;
    m_isEnabled = false;

    connect(this, &GenVectorFunc::evUpdateLinkState, this, &GenVectorFunc::onLinkStateChanged);
}

GenVectorFunc::~GenVectorFunc()
{
    disconnect(this, &GenVectorFunc::evUpdateLinkState, this, &GenVectorFunc::onLinkStateChanged);
    setIsEnabled(false);
}

quint32 GenVectorFunc::getFuncId() const
{
    return m_funcId;
}

bool GenVectorFunc::getIsEnabled() const
{
    return m_isEnabled;
}

void GenVectorFunc::setIsEnabled(bool p_value)
{
    if( p_value != m_isEnabled ) {
        ComDevice * pDev = m_pVectorStd->getDeviceManager()->getDevice(m_funcId);
        if( nullptr != pDev ) {
            disconnect(pDev, &ComDevice::evLinkChanged, this, &GenVectorFunc::onDeviceLinkChanged);
            m_pComDevice = nullptr;
        }
        if( p_value ) {
            getConnectionState();
        } else {
            m_pVectorStd->getDeviceManager()->unmapFunctionToDevice(m_funcId);
        }
        m_isEnabled = p_value;
        emit isEnabledChanged(p_value);
    }
    updateConnectedState();
}

bool GenVectorFunc::updateConnectedState()
{
    bool bRet = false;
    if( m_isEnabled ) {
        bRet = getConnectionState();
        emit evConnectionStateChanged(bRet);
    }
    return bRet;
}

void GenVectorFunc::configure(bool p_isEnabled, quint32 p_prodId)
{
    setIsEnabled(p_isEnabled);
    if(m_hasReceivedMessage) {
        m_hasReceivedMessage = false;
        emit hasReceivedMessageChanged(m_hasReceivedMessage);
    }
    if( p_isEnabled ) {   
        configureFunction(p_prodId);
    }
}

void GenVectorFunc::pushMsg(ProtMsg* p_pMsg)
{
    processReceivedMsg(p_pMsg);
    if(false == m_hasReceivedMessage) {
        m_hasReceivedMessage = true;
        emit hasReceivedMessageChanged(m_hasReceivedMessage);
    }
}

ComDevice *GenVectorFunc::getComDevice() const
{
    return m_pComDevice;
}

QString GenVectorFunc::getFunctionName() const
{
    return getXmlSectionName().remove(0, 10); // remove "VectorFunc"
}

bool GenVectorFunc::getConnectionState()
{
    bool isConnected = false;
    if( m_isEnabled ) {
        m_pComDevice = m_pVectorStd->getDeviceManager()->getDevice(m_funcId);
        // check if their is a device for this function
        if( nullptr != m_pComDevice ) {
            connect(m_pComDevice, &ComDevice::evLinkChanged, this, &GenVectorFunc::onDeviceLinkChanged, Qt::UniqueConnection);
            m_pComLink = m_pComDevice->getLink();
            if( nullptr != m_pComLink && m_pComLink->isConnected() ) {
                isConnected = true;
            }
            emit evUpdateLinkState();
        }
    }
    return isConnected;
}

QString GenVectorFunc::getConnectionName() {
    QString sRet = "";
    m_pComDevice = m_pVectorStd->getDeviceManager()->getDevice(m_funcId);
    // check if their is a device for this function
    if( nullptr != m_pComDevice ) {
        GenComLink * pLink  = m_pComDevice->getLink();
        if( nullptr != pLink ) {
            sRet = pLink->getName();
        }
    }
    return sRet;
}

void GenVectorFunc::onDeviceLinkChanged(GenComLink *p_link)
{
    if( nullptr != m_pComLink ) {
        disconnect(m_pComLink, &GenComLink::evLinkStateChanged, this, &GenVectorFunc::onLinkStateChanged);
    }
    m_pComLink = p_link;

    if( nullptr != p_link ) {
        connect(m_pComLink, &GenComLink::evLinkStateChanged, this, &GenVectorFunc::onLinkStateChanged);
        onLinkStateChanged(); // force update
    }
}

void GenVectorFunc::onLinkStateChanged()
{
    if( nullptr != m_pComLink ) {
        if(m_pComLink->isAuthentified() && m_pVectorStd->isMpc()) {
            // request to update parameters
            onDeviceConnected();
        }
    }
}

ProtMsg * GenVectorFunc::newMsgEvent(quint32 p_msgCmdEvent)
{
    ProtMsg * pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_pVectorStd->getDeviceAd();
    pMsg->m_msgType = ProtMsg::MSG_TYPE_EVENT;
    pMsg->m_deviceFunc = m_funcId;
    pMsg->m_msgObj = (p_msgCmdEvent&0xff);
    return pMsg;
}

ProtMsg * GenVectorFunc::newMsgCmd(quint32 p_msgCmd)
{
    ProtMsg * pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_pVectorStd->getDeviceAd();
    pMsg->m_msgType = ProtMsg::MSG_TYPE_CMD;
    pMsg->m_deviceFunc = m_funcId;
    pMsg->m_msgObj = (p_msgCmd&0xff);
    return pMsg;
}

ProtMsg * GenVectorFunc::newMsgParam(quint32 p_msgType, quint32 p_msgParam)
{
    ProtMsg * pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_pVectorStd->getDeviceAd();
    pMsg->m_msgType = (p_msgType);
    pMsg->m_deviceFunc = m_funcId;
    pMsg->m_msgObj = (p_msgParam&0xffff);
    return pMsg;
}

void GenVectorFunc::sendMsg(ProtMsg *p_pMsg, bool p_isEcho)
{
    m_pVectorStd->sendMsg(p_pMsg, p_isEcho);
}

void GenVectorFunc::onDeviceConnected()
{
    // implemented empty
}

bool GenVectorFunc::getHasReceivedMessage()
{
    return m_hasReceivedMessage;
}
