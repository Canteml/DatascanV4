#include <v2/com/ComLinkUdpServer.h>
#include <QNetworkDatagram>

#include <QDebug>

ComLinkUdpServer::ComLinkUdpServer(QObject* p_pParent) : QObject(p_pParent)
{

}

ComLinkUdpServer::~ComLinkUdpServer()
{
   delete m_pUdpServer;
}

void ComLinkUdpServer::registerSenderForAddress(const QString p_sAddress, ComLinkUdpCli* p_pUdp)
{
   m_mSendersMutex.lock();
   if(m_hUdpSenders.contains(p_sAddress)) {
      ComLinkUdpCli* old_cli = m_hUdpSenders[p_sAddress];
      m_hUdpSenders.remove(p_sAddress);
      delete old_cli;
   }
   m_hUdpSenders[p_sAddress] = p_pUdp;
   m_mSendersMutex.unlock();
}

void ComLinkUdpServer::removeSenderForAddress(const QString p_sAddress)
{
   m_mSendersMutex.lock();
   if(m_hUdpSenders.contains(p_sAddress)) {
      m_hUdpSenders.remove(p_sAddress);
   }
   m_mSendersMutex.unlock();
}

void ComLinkUdpServer::startServer()
{
   m_pUdpServer = new QUdpSocket(this);
   m_pUdpServer->bind(ComLinkUdpConfig::UDP_PORT_DEFAULT);
   m_pUdpServer->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 4 * 1024 * 1024);
   connect(m_pUdpServer, &QUdpSocket::readyRead, this, &ComLinkUdpServer::readData);
   qDebug() << "Listening udp...";
}

void ComLinkUdpServer::readData()
{
   while(m_pUdpServer->hasPendingDatagrams()) {
      QNetworkDatagram datagram = m_pUdpServer->receiveDatagram();
      quint32 ipv4 = datagram.senderAddress().toIPv4Address();
      QString sender = QString("%4.%3.%2.%1").arg(ipv4 % 256).arg((ipv4 >> 8) % 256).arg((ipv4 >> 16) % 256).arg((ipv4 >> 24) % 256);
      quint32 port = datagram.senderPort();
      if(ComLinkUdpConfig::UDP_PORT_DEFAULT != port) {
         continue;
      }
      if(m_hUdpSenders.contains(sender)) {
         ComLinkUdpCli* client = m_hUdpSenders[sender];
         client->receivedRawData(datagram.data());
      }
   }
}
