/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : DomDevice.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComDevice.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/GenComLink.h>
#include <v2/com/IntComMsg.h>
#include <v2/com/ComLinkManager.h>

// -- Qt includes
#include <QDateTime>


// ----------------------------------------------------------------------------
// -- ComDevice

ComDevice::ComDevice(IntComMsg* p_pIntComMsg, GenComLink * p_pComLink, const VectorFuncGeneralAuth &p_param): XmlProjectLoggable("ComDevice")
{
    m_pComLink = nullptr;
    m_mLink = new QMutex();
    m_pIntComMsg = p_pIntComMsg;
    m_comState = COM_LINKED_CONNECTED;
    m_paramFuncGeneralAuth = p_param;
    setLink(p_pComLink);
}

ComDevice::~ComDevice()
{
    if( nullptr != m_pComLink ) {
        GenComLink* oldComLink = m_pComLink;
        m_pComLink = nullptr;
        m_mLink->lock();
        oldComLink->setMappedDevice(nullptr);
        disconnect(oldComLink, &GenComLink::evReceivedNewMsg, this, &ComDevice::onNewLinkMsg);
        m_pIntComMsg->getLinkManager()->deleteLink(oldComLink);
        m_mLink->unlock();
    }
    delete m_mLink;
    m_mLink = nullptr;
}

quint32 ComDevice::getComState() const
{
    return m_comState;
}


QString ComDevice::getDeviceName() const
{
    return m_deviceName;
}

void ComDevice::setDeviceName(const QString& p_value)
{
    m_deviceName = p_value;
}

// -- Properties

qint32 ComDevice::getParamDProdId() const
{
    return m_paramFuncGeneralAuth.m_paramDProdId;
}

void ComDevice::setParamDProdId(qint32 p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDProdId != p_value ) {
        m_paramFuncGeneralAuth.m_paramDProdId = p_value;
        emit paramDProdIdChanged(m_paramFuncGeneralAuth.m_paramDProdId);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_PROD_ID);
            pMsg->m_len = 4;
            memcpy(pMsg->m_payload, &p_value, pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

qint32 ComDevice::getParamDDeviceAd() const
{
    return m_paramFuncGeneralAuth.m_paramDDeviceAd;
}

void ComDevice::setParamDDeviceAd(qint32 p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDDeviceAd != p_value ) {
        m_paramFuncGeneralAuth.m_paramDDeviceAd = p_value;
        emit paramDDeviceAdChanged(m_paramFuncGeneralAuth.m_paramDDeviceAd);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_DEVICE_AD);
            pMsg->m_len = 4;
            memcpy(pMsg->m_payload, &p_value, pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

QString ComDevice::getParamDProdUid() const
{
    return m_paramFuncGeneralAuth.m_paramDProdUid;
}

void ComDevice::setParamDProdUid(const QString &p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDProdUid != p_value ) {
        m_paramFuncGeneralAuth.m_paramDProdUid = p_value;
        emit paramDProdUidChanged(m_paramFuncGeneralAuth.m_paramDProdUid);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_PROD_UID);
            pMsg->m_len = p_value.length();
            memcpy(pMsg->m_payload, p_value.toLatin1().data(), pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}


QString ComDevice::getParamDVerHw() const
{
    return m_paramFuncGeneralAuth.m_paramDVerHw;
}

void ComDevice::setParamDVerHw(const QString & p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDVerHw != p_value ) {
        m_paramFuncGeneralAuth.m_paramDVerHw = p_value;
        emit paramDVerHwChanged(m_paramFuncGeneralAuth.m_paramDVerHw);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_VER_HW);
            pMsg->m_len = p_value.length();
            memcpy(pMsg->m_payload, p_value.toLatin1().data(), pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

QString ComDevice::getParamDVerSw() const
{
    return m_paramFuncGeneralAuth.m_paramDVerSw;
}

void ComDevice::setParamDVerSw(const QString & p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDVerSw != p_value ) {
        m_paramFuncGeneralAuth.m_paramDVerSw = p_value;
        emit paramDVerSwChanged(m_paramFuncGeneralAuth.m_paramDVerSw);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_VER_SW);
            pMsg->m_len = p_value.length();
            memcpy(pMsg->m_payload, p_value.toLatin1().data(), pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

qint64 ComDevice::getParamDDateBuild() const
{
    return m_paramFuncGeneralAuth.m_paramDDateBuild;
}

void ComDevice::setParamDDateBuild(qint64 p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDDateBuild != p_value ) {
        m_paramFuncGeneralAuth.m_paramDDateBuild = p_value;
        emit paramDDateBuildChanged(m_paramFuncGeneralAuth.m_paramDDateBuild);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_DATE_BUILD);
            pMsg->m_len = 8;
            memcpy(pMsg->m_payload, &p_value, pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

qint64 ComDevice::getParamDDateRev() const
{
    return m_paramFuncGeneralAuth.m_paramDDateRev;
}

void ComDevice::setParamDDateRev(qint64 p_value, bool b_syncProt)
{
    if( m_paramFuncGeneralAuth.m_paramDDateRev != p_value ) {
        m_paramFuncGeneralAuth.m_paramDDateRev = p_value;
        emit paramDDateRevChanged(m_paramFuncGeneralAuth.m_paramDDateRev);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_DATE_REV);
            pMsg->m_len = 8;
            memcpy(pMsg->m_payload, &p_value, pMsg->m_len);
            sendMsg(pMsg);
        }
    }
}

void ComDevice::setFunctions(const QList<quint32> &p_list, bool b_syncProt)
{
    if( p_list != m_paramFuncGeneralAuth.m_lFunctions ) {
        resetFunctions();
        addFunctions(p_list);
        emit paramLFunctionsChanged(p_list);
        if( b_syncProt ) {
            ProtMsg * pMsg = newMsgSetParamGeneral(ProtMsg::FUNC_GENERAL_PARAM_D_FCTS);
            pMsg->m_len = 20;
            for(int i=0; i<20; i++) {
                if( i<m_paramFuncGeneralAuth.m_lFunctions.size() ) {
                    pMsg->m_payload[i] = m_paramFuncGeneralAuth.m_lFunctions.at(i);
                } else {
                    pMsg->m_payload[i] = 0;
                }

            }
            sendMsg(pMsg);
        }
    }
}

// -- Functions
const QList<quint32>& ComDevice::getFunctions(void)
{
    return m_paramFuncGeneralAuth.m_lFunctions;
}

void ComDevice::addFunctions(const QList<quint32> & p_list)
{
    m_paramFuncGeneralAuth.m_lFunctions.append(p_list);
}

void ComDevice::resetFunctions()
{
    m_paramFuncGeneralAuth.m_lFunctions.clear();
}

// -- Communications
void ComDevice::setLink(GenComLink * p_pComLink)
{    
    m_mLink->lock();
    if( nullptr != m_pComLink ) {
        m_pComLink->setMappedDevice(nullptr);
        disconnect(m_pComLink, &GenComLink::evReceivedNewMsg, this, &ComDevice::onNewLinkMsg);
        disconnect(m_pComLink, &GenComLink::evLinkStateChanged, this, &ComDevice::onLinkStateChanged);
    }
    m_pComLink = p_pComLink;
    quint32 state = m_pComLink->getLinkState();

    if( nullptr != m_pComLink ) {
        m_pComLink->setMappedDevice(this);
        connect(m_pComLink, &GenComLink::evReceivedNewMsg, this, &ComDevice::onNewLinkMsg, (Qt::ConnectionType)(Qt::UniqueConnection | Qt::QueuedConnection));
        connect(m_pComLink, &GenComLink::evLinkStateChanged, this, &ComDevice::onLinkStateChanged, Qt::QueuedConnection);        
        setDeviceName(QString("%1 - Add%2").arg(m_pComLink->getName()).arg(m_paramFuncGeneralAuth.m_paramDDeviceAd));
    } else {
        setDeviceName("-");
    }
    m_mLink->unlock();

    if( nullptr != m_pComLink ) {
        onLinkStateChanged(m_pComLink, state);
    }

    emit evLinkChanged(m_pComLink);
}

GenComLink* ComDevice::getLink(void) const
{
    return m_pComLink;
}

void ComDevice::sendMsg(ProtMsg * p_pMsg)
{
    m_mLink->lock();
    if( nullptr != m_pComLink ) {
        p_pMsg->m_deviceAd = m_paramFuncGeneralAuth.m_paramDDeviceAd;
        m_pComLink->sendMsg(p_pMsg);
    } else {
        fLogWarning(QString("No link, msg deleted (Type %1, Obj %2)").arg(p_pMsg->m_msgType).arg(p_pMsg->m_msgObj));
        delete p_pMsg;
    }
    m_mLink->unlock();
}

void ComDevice::emergencyStop()
{
    ProtMsg * pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_paramFuncGeneralAuth.m_paramDDeviceAd;
    pMsg->m_msgType = ProtMsg::MSG_TYPE_CMD;
    pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
    pMsg->m_msgObj = (ProtMsg::FUNC_GENERAL_CMD_C_EMERGENCY_STOP&0xff);
    sendMsg(pMsg);

}

void ComDevice::onNewLinkMsg()
{
    ProtMsg * pMsg = nullptr;
    m_mLink->lock();
    if( nullptr != m_pComLink ) {
        pMsg = m_pComLink->takeMsg();
    }
    m_mLink->unlock();

    while( nullptr != pMsg ) {
        bool msgTreated = false;

        if( ProtMsg::DEVICE_FUNC_GENERAL == pMsg->m_deviceFunc ) {
            if( ProtMsg::MSG_TYPE_CMD == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_CMD_C_EMERGENCY_STOP == pMsg->m_msgObj ) {
                m_pIntComMsg->signalEmergencyStopFromDevice(this);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_PROD_ID == pMsg->m_msgObj ) {
                setParamDProdId(*(qint32*)pMsg->m_payload, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_DEVICE_AD == pMsg->m_msgObj ) {
                setParamDDeviceAd(*(qint32*)pMsg->m_payload, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_PROD_UID == pMsg->m_msgObj ) {
                QString value = QString().fromLatin1((char*)pMsg->m_payload, pMsg->m_len);
                setParamDProdUid(value, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_VER_HW == pMsg->m_msgObj ) {
                QString value = QString().fromLatin1((char*)pMsg->m_payload, pMsg->m_len);
                setParamDVerHw(value, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_VER_SW == pMsg->m_msgObj ) {
                QString value = QString().fromLatin1((char*)pMsg->m_payload, pMsg->m_len);
                setParamDVerSw(value, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_DATE_BUILD == pMsg->m_msgObj ) {
                setParamDDateBuild(*(qint64*)pMsg->m_payload, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_DATE_REV == pMsg->m_msgObj ) {
                setParamDDateRev(*(qint64*)pMsg->m_payload, false);
                msgTreated = true;
            }
            if( ProtMsg::MSG_TYPE_PARAM_SET == pMsg->m_msgType &&
                ProtMsg::FUNC_GENERAL_PARAM_D_FCTS == pMsg->m_msgObj ) {
                QList<quint32> lFunc;
                for(int i=0; i<20; i++) {
                    if( pMsg->m_payload[i] != 0 ) {
                        lFunc.append(pMsg->m_payload[i]);
                    } else {
                        break;
                    }
                }
                setFunctions(lFunc, false);
                msgTreated = true;
            }
        }

        if( !msgTreated ) {
            // let the vector know where the message comes from (BScan source selection)
            m_pIntComMsg->setRxLinkName(nullptr != m_pComLink ? m_pComLink->getName() : QString());
            m_pIntComMsg->processReceivedMsg(pMsg);
            m_pIntComMsg->setRxLinkName(QString());
        }
        delete pMsg;

        m_mLink->lock();
        if( nullptr != m_pComLink ) {
            pMsg = m_pComLink->takeMsg();
        }
        m_mLink->unlock();
    }
}

void ComDevice::onLinkStateChanged(GenComLink *, quint32 p_value)
{
    if( p_value == GenComLink::LINK_STATE_CONNECTED ) {
        setComState(COM_LINKED_CONNECTED);
    } else {
        setComState(COM_LINKED_NOT_CONNECTED);
    }
}

void ComDevice::setComState(quint32 p_value)
{
    if( m_comState != p_value ) {
        m_comState = p_value;
        emit comStateChanged(m_comState);
    }
}

ProtMsg *ComDevice::newMsgSetParamGeneral(qint32 p_param)
{
    ProtMsg* pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_paramFuncGeneralAuth.m_paramDDeviceAd;
    pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
    pMsg->m_msgType = ProtMsg::MSG_TYPE_PARAM_SET;
    pMsg->m_msgObj = p_param;
    return pMsg;
}

// -- Inherited from XmlProject
bool ComDevice::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_paramFuncGeneralAuth.loadParam(p_pParent);
    return bRet;
}

bool ComDevice::customLoadDefaultParam()
{
    bool bRet = true;
    bRet &= m_paramFuncGeneralAuth.loadDefaultParam();
    return bRet;
}

bool ComDevice::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool ComDevice::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_paramFuncGeneralAuth.saveParam(p_pDoc, p_pParent);
    return bRet;
}


