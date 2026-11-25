/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : IntComMsg.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/IntComMsg.h>
#include <v2/com/ComLinkManager.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/prot/ProtMsg.h>
#include <QMetaType>

#define ALERT_TIMEOUT_MS 5000

IntComMsg::IntComMsg(QObject *parent) :QObject(parent), Loggable("IntComMsg"), XmlProject("IntComMsg")
{
    qRegisterMetaType<ComDeviceManager*>("ComDeviceManager*");
    qRegisterMetaType<ComLinkManager*>("ComLinkManager*");
    m_pComLinkManager = new ComLinkManager(this);
    m_pComDeviceManager = new ComDeviceManager(this, this);

    m_funcGeneral.m_paramDProdId = ProtMsg::PROD_UNDEF;

    for(int i = 0 ; i < 3 ; i++) {
        m_aAlertsTimeouts[i] = new QTimer(this);
        m_aAlertsTimeouts[i]->setSingleShot(true);
    }

    connect(m_aAlertsTimeouts[0], &QTimer::timeout, this, &IntComMsg::onTimeoutAlert0);
    connect(m_aAlertsTimeouts[1], &QTimer::timeout, this, &IntComMsg::onTimeoutAlert1);
    connect(m_aAlertsTimeouts[2], &QTimer::timeout, this, &IntComMsg::onTimeoutAlert2);
}

IntComMsg::~IntComMsg()
{
    // delete Components
    m_pComDeviceManager->clearDeviceFunctions();

    // delete links
    m_pComLinkManager->deleteLinks();

    delete m_pComDeviceManager;
    m_pComDeviceManager = nullptr;
    delete m_pComLinkManager;
    m_pComLinkManager = nullptr;

    for(int i = 0 ; i < 3 ; i++) {
        if(m_aAlertsTimeouts[i]->isActive()) {
            m_aAlertsTimeouts[i]->stop();
        }
        delete m_aAlertsTimeouts[i];
    }
}

// -- Configure the system
ComLinkManager * IntComMsg::getLinkManager() const
{
    return m_pComLinkManager;
}

ComDeviceManager * IntComMsg::getDeviceManager() const
{
    return m_pComDeviceManager;
}

// Interface with devices :
void IntComMsg::sendMsg(ProtMsg* p_pMsg, bool p_isEcho)
{
    m_pComDeviceManager->sendMsg(p_pMsg, p_isEcho);
}

qint32 IntComMsg::getProdId() const
{
    return m_funcGeneral.m_paramDProdId;
}

qint32 IntComMsg::getDeviceAd() const
{
    return m_funcGeneral.m_paramDDeviceAd;
}

void IntComMsg::setProdId(qint32 p_value)
{
    if( p_value != m_funcGeneral.m_paramDProdId ) {
        setProdIdDeviceAd(p_value, m_funcGeneral.m_paramDDeviceAd);
    }
}

void IntComMsg::setProdIdDeviceAd(qint32 p_prodId, qint32 p_deviceAd)
{
    if( m_funcGeneral.m_paramDProdId != p_prodId || m_funcGeneral.m_paramDDeviceAd != p_deviceAd ) {
        m_funcGeneral.m_paramDProdId = p_prodId;
        m_funcGeneral.m_paramDDeviceAd = p_deviceAd;
        emit prodIdChanged(m_funcGeneral.m_paramDProdId);
        emit deviceAdChanged(m_funcGeneral.m_paramDDeviceAd);
        configureProduct(m_funcGeneral.m_paramDProdId, m_funcGeneral.m_paramDDeviceAd);
    }
    // auto start Discover for MPC
    if( ProtMsg::DEVICE_AD_MPC == p_deviceAd ) {
        m_pComDeviceManager->startDeviceDiscover();
    }
}

void IntComMsg::setDeviceAd(qint32 p_value)
{
    if( m_funcGeneral.m_paramDDeviceAd != p_value ) {
        m_funcGeneral.m_paramDDeviceAd = p_value;
        emit deviceAdChanged(m_funcGeneral.m_paramDDeviceAd);
        // need reboot ...
    }
}

bool IntComMsg::isPanelControl() const
{
    return (ProtMsg::DEVICE_AD_MPC == m_funcGeneral.m_paramDDeviceAd) || (ProtMsg::DEVICE_AD_SECONDARY_CONTROL_PANEL < m_funcGeneral.m_paramDDeviceAd);
}

bool IntComMsg::isMpc() const
{
    return (ProtMsg::DEVICE_AD_MPC == m_funcGeneral.m_paramDDeviceAd);
}

void IntComMsg::setFuncGeneralParam(const VectorFuncGeneralAuth& p_value)
{
    m_funcGeneral = p_value;
    m_pComDeviceManager->resetComOnGeneralParamChanged();
}

const VectorFuncGeneralAuth& IntComMsg::getFuncGeneralParam() const
{
    return m_funcGeneral;
}

void IntComMsg::signalEmergencyStopFromDevice(ComDevice *p_pDevice)
{
    fLogInfo(QString("Received Emergency Stop from : %1").arg(p_pDevice->getDeviceName()));
}

void IntComMsg::timeoutAlert(quint32 alertLevel)
{
    m_funcGeneral.m_alerts[alertLevel] = QString();
    emit alertChanged(alertLevel);
}

void IntComMsg::setAlert(quint32 alertLevel, QString alertMsg)
{
    m_funcGeneral.m_alerts[alertLevel] = alertMsg;
    fLogWarning(QString("ALERT LEVEL [%1] : %2").arg(alertLevel).arg(alertMsg));
    QTimer* timer = m_aAlertsTimeouts[alertLevel];
    if(timer->isActive()) {
        timer->stop();
    }
    timer->start(ALERT_TIMEOUT_MS);
    emit alertChanged(alertLevel);
}

QString IntComMsg::getAlertMsg(quint8 alertLevel) const
{
    if(alertLevel > 2) {
        fLogError("ERROR : ALERT LEVEL OUT OF RANGE !");
        return "";
    }
    return m_funcGeneral.m_alerts[alertLevel];
}

void IntComMsg::stopAlertsTimeouts()
{
    for(int i = 0 ; i < 3 ; i++) {
        if(m_aAlertsTimeouts[i]->isActive()) {
            m_aAlertsTimeouts[i]->stop();
        }
    }
}

void IntComMsg::onTimeoutAlert0()
{
    timeoutAlert(0);
}

void IntComMsg::onTimeoutAlert1()
{
    timeoutAlert(1);
}

void IntComMsg::onTimeoutAlert2()
{
    timeoutAlert(2);
}

// -- Inherited from XmlProject
bool IntComMsg::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    m_funcGeneral.loadParam(p_pParent);
    return bRet;
}

bool IntComMsg::customLoadDefaultParam()
{
    bool bRet = true;
    m_funcGeneral.loadDefaultParam();
    m_pComLinkManager->loadDefaultParam();
    return bRet;
}

bool IntComMsg::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool IntComMsg::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    m_funcGeneral.saveParam(p_pDoc, p_pParent);
    return bRet;
}

//bool IntComMsg::customLoadSettings(QDomElement * p_pParent)

//bool IntComMsg::customLoadDefaultSettings()

//bool IntComMsg::customApplySettings()

//bool IntComMsg::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)

