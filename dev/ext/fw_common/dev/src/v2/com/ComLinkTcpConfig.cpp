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
#include <v2/com/ComLinkTcpConfig.h>

const QString ComLinkTcpConfig::TCP_BASE_IP = "192.168.0.";

ComLinkTcpConfig::ComLinkTcpConfig() : XmlSettingsLoggable("ComLinkTcpConfig")
{
    useDefaultConfig();
}

ComLinkTcpConfig::~ComLinkTcpConfig()
{

}

void ComLinkTcpConfig::useDefaultConfig()
{
    m_portNumber = TCP_PORT_DEFAULT;
    m_hostAddress = TCP_BASE_IP;
}

void ComLinkTcpConfig::setConfig(const ComLinkTcpConfig& p_valueIn)
{
    setPortNumber(p_valueIn.getPortNumber());    
    setHostAddress(p_valueIn.getHostAddress());
}

void ComLinkTcpConfig::getConfig(ComLinkTcpConfig& p_valueInOut) const
{
    p_valueInOut.setPortNumber(m_portNumber);
    p_valueInOut.setHostAddress(m_hostAddress);
}

bool ComLinkTcpConfig::isHostAddressDefined() const
{
    return m_hostAddress != "0.0.0.0";
}

// -- Properties
quint32 ComLinkTcpConfig::getPortNumber() const
{
    return m_portNumber;
}

void ComLinkTcpConfig::setPortNumber(quint32 p_value)
{
    if( m_portNumber != p_value ) {
        m_portNumber = p_value;
        emit portNumberChanged(m_portNumber);
    }
}

QString ComLinkTcpConfig::getHostAddress() const
{
    return m_hostAddress;
}

void ComLinkTcpConfig::setHostAddress(QString p_value)
{
    if( m_hostAddress != p_value ) {
        m_hostAddress = p_value;
        emit hostAddressChanged(m_hostAddress);
    }
}

// -- XmlSettings behaviour
bool ComLinkTcpConfig::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint32, m_portNumber, TCP_PORT_DEFAULT);
    bRet &= XML_READ_UNIQUE_DEFAULT(QString, m_hostAddress, "0.0.0.0");
    return bRet;
}

bool ComLinkTcpConfig::customLoadDefaultSettings()
{
    useDefaultConfig();
    return true;
}

bool ComLinkTcpConfig::customApplySettings()
{
    return true;
}

bool ComLinkTcpConfig::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_portNumber);
    bRet &= XML_WRITE_UNIQUE(m_hostAddress);
    return bRet;
}
