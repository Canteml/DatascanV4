#include <v2/com/ComLinkUdpCli.h>
#include <v2/com/ComLinkUdpServer.h>

ComLinkUdpCli::ComLinkUdpCli(IntComMsg* p_pIntComMsg) : GenComLink(p_pIntComMsg)
{
   setSettingsLoggableName("ComLinkUdpCli");
   m_pUdpSocket = nullptr;
   m_pReceptionServer = nullptr;
}


ComLinkUdpCli::~ComLinkUdpCli()
{
   if(m_pReceptionServer != nullptr) {
      m_pReceptionServer->removeSenderForAddress(m_config.getHostAddress());
   }
}

void ComLinkUdpCli::getConfig(ComLinkUdpConfig& p_valueInOut) const
{
   p_valueInOut.setHostAddress(m_config.getHostAddress());
   p_valueInOut.setPortNumber(m_config.getPortNumber());
}

QString ComLinkUdpCli::getConfigHelp() const
{
   QString sRet;
   if( nullptr != m_pUdpSocket ) {

      sRet = QString("Add. %1, Port %2 (udp)").arg(m_pUdpSocket->peerAddress().toString()).arg(m_pUdpSocket->peerPort());
   }
   return sRet;
}

void ComLinkUdpCli::setLinkUdpConfig(ComLinkUdpConfig* p_conf)
{
   m_config.setConfig(*p_conf);
}

void ComLinkUdpCli::receivedRawData(QByteArray p_baData)
{
    if(nullptr == m_pUdpSocket) {
        return;
    }
    if( isProtocolD4() && p_baData.size()>= ProtMsg::COM_HEADER_SIZE
        && quint8(p_baData.at(4)) == ProtMsg::MSG_TYPE_EVENT
        && quint8(p_baData.at(5)) == ProtMsg::DEVICE_FUNC_ASCAN_0
        && p_baData.size() != ProtMsg::COM_MSG_SIZE ) {
        return;
    }
    m_pUdpSocket->bufferData(p_baData);
}

void ComLinkUdpCli::setReceptionServer(ComLinkUdpServer* p_pServer)
{
   m_pReceptionServer = p_pServer;
}

int ComLinkUdpCli::getLinkTypeTimeout()
{
   return UDP_TIMEOUT_MS;
}

int ComLinkUdpCli::getLinkTypeHeartBeatDelay()
{
   return UDP_HEARTBEAT_DELAY_MS;
}

quint32 ComLinkUdpCli::getType() const
{
   return LINK_UDP;
}

ComLinkUdpCli* ComLinkUdpCli::getLinkUdpCli()
{
   return this;
}

bool ComLinkUdpCli::customLoadSettings(QDomElement* p_pParent)
{
   return true;
}

bool ComLinkUdpCli::customLoadDefaultSettings()
{
   return true;
}

bool ComLinkUdpCli::customApplySettings()
{
   return true;
}

bool ComLinkUdpCli::customSaveSettings(QDomDocument* p_pDoc, QDomElement* p_pParent)
{
   return true;
}

bool ComLinkUdpCli::tryOpen()
{
   if( m_config.isHostAddressDefined() && nullptr != m_pUdpSocket) {
      m_pUdpSocket->connectToHost(m_config.getHostAddress(), ComLinkUdpConfig::UDP_PORT_DEFAULT);
      m_pReceptionServer->registerSenderForAddress(m_config.getHostAddress(), this);
   }
   return true;
}

void ComLinkUdpCli::removeLink()
{
   if( nullptr != m_pReceptionServer ) {
      m_pReceptionServer->removeSenderForAddress(m_config.getHostAddress());
   }
   GenComLink::removeLink();
}

QString ComLinkUdpCli::interfaceName() const
{
   return QString("UDP");
}

QIODevice* ComLinkUdpCli::makeInterface()
{
   // Socket has been created by ComLinkManager
   if( nullptr == m_pUdpSocket || nullptr == m_pInterface) {
      m_pUdpSocket = new BufferedReadUdpSocket();
   }
   return m_pUdpSocket;
}

void BufferedReadUdpSocket::bufferData(QByteArray& p_baData)
{
    {
        QMutexLocker locker(&m_mBufferedData);
        m_baBufferedData.append(p_baData);
    }
    emit readyRead();
}

qint64 BufferedReadUdpSocket::bytesAvailable() const
{
    QMutexLocker locker(&m_mBufferedData);
    return m_baBufferedData.size() + QIODevice::bytesAvailable();
}

qint64 BufferedReadUdpSocket::readData(char* data, qint64 maxSize)
{
    QMutexLocker locker(&m_mBufferedData);
    qint64 toCopy = qMin(maxSize, qint64(m_baBufferedData.size()));
    memcpy(data, m_baBufferedData.data(), toCopy);
    m_baBufferedData.remove(0, toCopy);
    return toCopy;
}
