/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpConfig.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkUdpConfig.h>

const QString ComLinkUdpConfig::UDP_BASE_IP = "192.168.0.";

ComLinkUdpConfig::ComLinkUdpConfig() : XmlSettingsLoggable("ComLinkUdpConfig")
{
   useDefaultConfig();
}

ComLinkUdpConfig::~ComLinkUdpConfig()
{

}

void ComLinkUdpConfig::useDefaultConfig()
{
    m_portNumber = UDP_PORT_DEFAULT;
    m_hostAddress = UDP_BASE_IP;
}

void ComLinkUdpConfig::setConfig(const ComLinkUdpConfig& p_valueIn)
{
    setPortNumber(p_valueIn.getPortNumber());
    setHostAddress(p_valueIn.getHostAddress());
}

void ComLinkUdpConfig::getConfig(ComLinkUdpConfig& p_valueInOut) const
{
    p_valueInOut.setPortNumber(m_portNumber);
    p_valueInOut.setHostAddress(m_hostAddress);
}

bool ComLinkUdpConfig::isHostAddressDefined() const
{
    return m_hostAddress != "0.0.0.0";
}

// -- Properties
quint32 ComLinkUdpConfig::getPortNumber() const
{
    return m_portNumber;
}

void ComLinkUdpConfig::setPortNumber(quint16 p_value)
{
    if( m_portNumber != p_value ) {
        m_portNumber = p_value;
        emit portNumberChanged(m_portNumber);
    }
}

QString ComLinkUdpConfig::getHostAddress() const
{
    return m_hostAddress;
}

void ComLinkUdpConfig::setHostAddress(QString p_value)
{
    if( m_hostAddress != p_value ) {
        m_hostAddress = p_value;
        emit hostAddressChanged(m_hostAddress);
    }
}

// -- XmlSettings behaviour
bool ComLinkUdpConfig::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_portNumber, UDP_PORT_DEFAULT);
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_hostAddress, "0.0.0.0");
    return bRet;
}

bool ComLinkUdpConfig::customLoadDefaultSettings()
{
    useDefaultConfig();
    return true;
}

bool ComLinkUdpConfig::customApplySettings()
{
    return true;
}

bool ComLinkUdpConfig::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_portNumber);
    bRet &= XML_WRITE_UNIQUE(m_hostAddress);
    return bRet;
}
