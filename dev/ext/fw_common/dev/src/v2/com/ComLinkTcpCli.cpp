/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpCli.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkTcpCli.h>

// -- Qt includes
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

ComLinkTcpCli::ComLinkTcpCli(IntComMsg* p_pIntComMsg) : GenComLink(p_pIntComMsg)
{
    setSettingsLoggableName("ComLinkTcpCli");
    m_pTcpSocket = nullptr;
}

ComLinkTcpCli::~ComLinkTcpCli()
{

}

void ComLinkTcpCli::getConfig(ComLinkTcpConfig& p_valueInOut) const
{
    p_valueInOut.setPortNumber(m_pTcpSocket->peerPort());
}


quint32 ComLinkTcpCli::getPortNumber() const
{
    return m_pTcpSocket->peerPort();
}

void ComLinkTcpCli::setLinkTcpConfig(ComLinkTcpConfig *p_conf)
{
    m_comLinkTcpConfig.setConfig(*p_conf);
    setName(QString("%1:%2").arg(p_conf->getHostAddress()).arg(p_conf->getPortNumber()));
}


QString ComLinkTcpCli::getConfigHelp() const
{
    QString sRet;
    if( nullptr != m_pTcpSocket ) {
        sRet = QString("Add. %1, Port %2").arg(m_pTcpSocket->peerAddress().toString()).arg(m_pTcpSocket->peerPort());
    }
    return sRet;
}

bool ComLinkTcpCli::tryConnect()
{
    bool bRet = false;
    if( m_comLinkTcpConfig.isHostAddressDefined() && nullptr != m_pTcpSocket) {
        if( m_pTcpSocket->state() != QAbstractSocket::ConnectedState ) {
            m_pTcpSocket->connectToHost(m_comLinkTcpConfig.getHostAddress(), m_comLinkTcpConfig.getPortNumber());
            if( m_pTcpSocket->state() == QAbstractSocket::ConnectedState || m_pTcpSocket->waitForConnected(100) ) {
                connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &ComLinkTcpCli::onSocketClosed, Qt::QueuedConnection);
                QThread::msleep(100);
                bRet = true;
            } else {
                m_pTcpSocket->close();
                delete m_pTcpSocket;
                m_pTcpSocket = nullptr;
                m_pInterface = nullptr;
            }
        }else {
            bRet = true;
        }
    }
    return bRet;
}

quint32 ComLinkTcpCli::getType() const
{
    return LINK_TCP;
}

ComLinkTcpCli * ComLinkTcpCli::getLinkTcpCli()
{
    return this;
}

void ComLinkTcpCli::onSocketClosed()
{
    linkError("Socket closed");
}

bool ComLinkTcpCli::customLoadSettings(QDomElement *)
{
    return true;
}

bool ComLinkTcpCli::customLoadDefaultSettings()
{
    return true;
}

bool ComLinkTcpCli::customApplySettings()
{
    return true;
}

bool ComLinkTcpCli::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_pTcpSocket->peerPort());
    return bRet;
}

bool ComLinkTcpCli::tryOpen()
{
    return tryConnect();
}

void ComLinkTcpCli::removeLink()
{
    if( nullptr != m_pTcpSocket ) {
        m_mInterface->lock();
        disconnect(m_pTcpSocket, &QTcpSocket::disconnected, this, &ComLinkTcpCli::onSocketClosed);
        m_pTcpSocket->disconnectFromHost();
        if( m_pTcpSocket->state() != QAbstractSocket::UnconnectedState ) {
            m_pTcpSocket->waitForDisconnected(200);
        }
        m_pTcpSocket->deleteLater();
        m_pTcpSocket = nullptr;
        m_pInterface = nullptr;
        m_mInterface->unlock();
    }
    GenComLink::removeLink();
}

// -- vecLink
QString ComLinkTcpCli::interfaceName() const
{
    return QString("TCP");
}

QIODevice * ComLinkTcpCli::makeInterface()
{
    // Socket has been created by ComLinkManager
     if( nullptr == m_pTcpSocket || nullptr == m_pInterface) {
        m_pTcpSocket = new QTcpSocket();
    }
    return m_pTcpSocket;
}
