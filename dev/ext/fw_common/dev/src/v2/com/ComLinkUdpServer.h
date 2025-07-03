#pragma once

#include <QObject>

#include <v2/com/ComLinkUdpConfig.h>
#include <v2/com/ComLinkUdpCli.h>

#include <QUdpSocket>

class ComLinkUdpServer : public QObject {

   Q_OBJECT

public:

   ComLinkUdpServer(QObject* p_pParent = nullptr);
   ~ComLinkUdpServer();

   void registerSenderForAddress(const QString p_sAddress, ComLinkUdpCli* p_pUdp);
   void removeSenderForAddress(const QString p_sAddress);

   void startServer();

public slots:

   void readData();

private:

   QMap<QString, ComLinkUdpCli*> m_hUdpSenders;
   QUdpSocket* m_pUdpServer;
   QMutex m_mSendersMutex;

};
