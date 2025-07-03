/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkTcpServer.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/ComLinkTcpServer.h>

// -- Qt includes
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>

ComLinkTcpServer::ComLinkTcpServer(IntComMsg* p_pIntComMsg) : GenComLink(p_pIntComMsg)
{
    setSettingsLoggableName("ComLinkTcpServer");

    // make TCP server (for device & simu)

    m_pTcpServer =  nullptr;
    m_tcpServerPort = ComLinkTcpConfig::TCP_PORT_DEFAULT;
    m_pTcpSocketCli = nullptr;

}

ComLinkTcpServer::~ComLinkTcpServer()
{
    stopServer();
}


quint16 ComLinkTcpServer::getPortNumber() const
{
    return m_tcpServerPort;
}

QString ComLinkTcpServer::getConfigHelp() const
{
    return QString("Port %2").arg(m_tcpServerPort);
}

bool ComLinkTcpServer::startServer(quint16 p_port)
{
    if( m_tcpServerPort != p_port ) {
        m_tcpServerPort = p_port;
    }

    return true;
}

void ComLinkTcpServer::stopServer()
{
    if( nullptr != m_pTcpSocketCli ) {
        disconnect(m_pTcpSocketCli, &QTcpSocket::disconnected, this, &ComLinkTcpServer::onTcpSocketCliDisconnected);
        disconnect(m_pInterface, &QIODevice::readyRead, this, &GenComLink::onReadData);
    }
    stopConnection();
}

quint32 ComLinkTcpServer::getType() const
{
    return LINK_TCP;
}

void ComLinkTcpServer::onTcpSocketCliDisconnected()
{
    linkError("Lost client");
}

bool ComLinkTcpServer::customLoadSettings(QDomElement *p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_tcpServerPort, ComLinkTcpConfig::TCP_PORT_DEFAULT);
    return bRet;
}

bool ComLinkTcpServer::customLoadDefaultSettings()
{
    m_tcpServerPort = ComLinkTcpConfig::TCP_PORT_DEFAULT;
    return true;
}

bool ComLinkTcpServer::customApplySettings()
{
    return true;
}

bool ComLinkTcpServer::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_tcpServerPort);
    return bRet;
}

bool ComLinkTcpServer::tryOpen()
{
    return m_pTcpServer->isListening();
}

// -- vecLink
QString ComLinkTcpServer::interfaceName() const
{
    return QString("TCP");
}

QIODevice * ComLinkTcpServer::makeInterface()
{
    if( nullptr == m_pTcpServer ) {
        m_pTcpServer = new QTcpServer();
        m_pTcpServer->moveToThread(m_pThread);
        connect(m_pTcpServer, &QTcpServer::newConnection, this, &ComLinkTcpServer::onNewTcpServerConnection, Qt::QueuedConnection);
        m_pTcpServer->listen(QHostAddress::Any, m_tcpServerPort);
    }
    return nullptr;
}

void ComLinkTcpServer::removeLink()
{
    m_mInterface->lock();
    if( nullptr != m_pTcpSocketCli ) {
        disconnect(m_pTcpSocketCli, &QTcpSocket::disconnected, this, &ComLinkTcpServer::onTcpSocketCliDisconnected);
        disconnect(m_pInterface, &QIODevice::readyRead, this, &GenComLink::onReadData);
        if( m_pTcpSocketCli->state() != QAbstractSocket::UnconnectedState ) {
            m_pTcpSocketCli->disconnectFromHost();
            if( m_pTcpSocketCli->state() != QAbstractSocket::UnconnectedState ) {
                m_pTcpSocketCli->waitForDisconnected(100);
            }
        }
        //delete m_pTcpSocketCli; <= Server will delete the socket (I think..)
        m_pTcpSocketCli = nullptr;
    }
    if( nullptr != m_pTcpServer ) {
        disconnect(m_pTcpServer, &QTcpServer::newConnection, this, &ComLinkTcpServer::onNewTcpServerConnection);
        m_pTcpServer->close();
        //delete m_pTcpServer; <= do not delete now because some messages can stil be in the pipe
        m_pTcpServer->deleteLater();
        m_pTcpServer = nullptr;
    }
    m_pInterface = nullptr;
    m_mInterface->unlock();
    GenComLink::removeLink();
}


void ComLinkTcpServer::onNewTcpServerConnection()
{
    QTcpSocket* pSocket = m_pTcpServer->nextPendingConnection();
    // It can happen that the socket is here no more connected ... test it before to accept it
    if( pSocket->state() == QAbstractSocket::ConnectedState ) {
        if( nullptr != m_pTcpSocketCli ) {
            fLogWarning("Accept a client while an other one is already used");
        }

        if( m_timAutoConnect->isActive() ) {
            m_timAutoConnect->stop();
        }

        m_pTcpSocketCli = pSocket;
        m_pInterface = m_pTcpSocketCli;

        // connect slots
        connect(m_pTcpSocketCli, &QTcpSocket::disconnected, this, &ComLinkTcpServer::onTcpSocketCliDisconnected);
        connect(m_pInterface, &QIODevice::readyRead, this, &GenComLink::onReadData, Qt::QueuedConnection);
        m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
        setLinkState(LINK_STATE_CONNECTED);
    }
}
