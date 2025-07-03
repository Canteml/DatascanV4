/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkSerial.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkSerial.h>

// -- Qt includes

// ----------------------------------------------------------------------------
// -- ComLinkSerialConfig

ComLinkSerialConfig::ComLinkSerialConfig() : XmlSettingsLoggable("ComLinkSerialConfig")
{
    useDefaultConfig();
}

ComLinkSerialConfig::~ComLinkSerialConfig()
{

}

void ComLinkSerialConfig::useDefaultConfig()
{
    m_portNumber = 0;
    m_baudrate = 0;
}

void ComLinkSerialConfig::setConfig(const ComLinkSerialConfig& p_valueIn)
{
    setPortNumber(p_valueIn.getPortNumber());
    setBaudrate(p_valueIn.getBaudrate());
}
void ComLinkSerialConfig::getConfig(ComLinkSerialConfig& p_valueInOut) const
{
    p_valueInOut.setPortNumber(m_portNumber);
    p_valueInOut.setBaudrate(m_baudrate);
}

quint32 ComLinkSerialConfig::getPortNumber() const
{
    return m_portNumber;
}

void ComLinkSerialConfig::setPortNumber(quint32 p_value)
{
    if( m_portNumber != p_value ) {
        m_portNumber = p_value;
        emit portNumberChanged();
    }
}

quint32 ComLinkSerialConfig::getBaudrate() const
{
    return m_baudrate;
}

void ComLinkSerialConfig::setBaudrate(quint32 p_value)
{
    if( m_baudrate != p_value ) {
        m_baudrate = p_value;
        emit baudrateChanged();
    }
}

// -- XmlSettings behaviour
bool ComLinkSerialConfig::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_portNumber, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_baudrate, 9600);
    return bRet;
}

bool ComLinkSerialConfig::customLoadDefaultSettings()
{
    useDefaultConfig();
    return true;
}

bool ComLinkSerialConfig::customApplySettings()
{
    return true;
}

bool ComLinkSerialConfig::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_portNumber);
    bRet &= XML_WRITE_UNIQUE(m_baudrate);
    return bRet;
}


// ----------------------------------------------------------------------------
// -- vecLinkSerial

ComLinkSerial::ComLinkSerial(IntComMsg* p_pIntComMsg) : GenComLink(p_pIntComMsg)
{
    setSettingsLoggableName("ComLinkSerial");
}

ComLinkSerial::~ComLinkSerial()
{
    fLogDebug1("delete serial");
}

void ComLinkSerial::getConfig(ComLinkSerialConfig &p_valueInOut) const
{
    m_config.getConfig(p_valueInOut);
}

quint32 ComLinkSerial::getPortNumber() const
{
    return m_config.getPortNumber();
}

quint32 ComLinkSerial::getBaudrate() const
{
    return m_config.getBaudrate();
}


void ComLinkSerial::setConfig(const ComLinkSerialConfig & p_value)
{
    setLogName(QString("ComLinkSerial_COM%1").arg(p_value.getPortNumber()));
    m_config.setConfig(p_value);
    fLogInfo("New config : " + getConfigHelp());
    emit evConfigChanged();
    if( p_value.getPortNumber() != 0 ) {
        restartConnection();
    } else {
        stopConnection();
    }
}

void ComLinkSerial::setConfig(quint32 p_port, quint32 p_bauds)
{
    ComLinkSerialConfig conf;
    conf.setPortNumber(p_port);
    conf.setBaudrate(p_bauds);
    setConfig(conf);
}

QString ComLinkSerial::getConfigHelp() const
{
    return QString("COM%1, %2 bauds").arg(m_config.getPortNumber()).arg(m_config.getBaudrate());
}

// -- Type

quint32 ComLinkSerial::getType() const
{
    return LINK_SERIAL;
}

ComLinkSerial *ComLinkSerial::getLinkSerial()
{
    return this;
}

void ComLinkSerial::setLinkSerialConfig(ComLinkSerialConfig *p_conf)
{
    setConfig(*p_conf);
}

void ComLinkSerial::onErrorOccured(QSerialPort::SerialPortError err)
{
    if( QSerialPort::NoError != err ) {
        linkError(QString("%1 (%2)").arg(tr("Serial port error")).arg(err));
    }
}

// -- Inherited behaviour
bool ComLinkSerial::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = GenComLink::customLoadSettings(p_pParent);
    m_config.loadSettings(p_pParent);
    return bRet;
}

bool ComLinkSerial::customLoadDefaultSettings()
{
    bool bRet = GenComLink::customLoadDefaultSettings();
    m_config.useDefaultConfig();
    return bRet;
}

bool ComLinkSerial::customApplySettings()
{
    bool bRet = GenComLink::customApplySettings();
    setConfig(m_config);
    return bRet;
}

bool ComLinkSerial::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = GenComLink::customSaveSettings(p_pDoc, p_pParent);
    m_config.saveSettings(p_pParent);
    return bRet;
}

bool ComLinkSerial::tryOpen()
{
    bool bRet = false;
    if( !m_pInterface->isOpen() ) {
        bRet =  m_pInterface->open(QIODevice::ReadWrite);
    }
    return bRet;
}


// -- ComLink
QString ComLinkSerial::interfaceName() const
{
    return QString("Serial Port");
}

QIODevice * ComLinkSerial::makeInterface()
{
    QSerialPort * sp = new QSerialPort(QString("COM%1").arg(m_config.getPortNumber()));
//    sp->setReadBufferSize(1000000);
    connect(sp, &QSerialPort::errorOccurred, this, &ComLinkSerial::onErrorOccured, Qt::QueuedConnection);
    sp->setBaudRate(m_config.getBaudrate());
    return sp;
}
