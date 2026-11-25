/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : GenComLink.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/com/GenComLink.h>
#include <v2/prot/ProtMsg.h>
#include <v2/prot/ProtD4.h>
#include <v2/com/IntComMsg.h>

// -- Qt includes
#include <QThread>
#include <QMutex>
#include <QIODevice>
#include <QTimer>
#include <QRandomGenerator>

#include <VectorManager.h>
#include <VectorStd.h>
#include <v2/com/ComLinkManager.h>

#define UDP_FRAME_INTERVAL_MIN_MS 10

// ----------------------------------------------------------------------------
// -- GenComLink

GenComLink::GenComLink(IntComMsg* p_pIntComMsg) : XmlSettingsLoggable("GenComLink")
{
   qRegisterMetaType<ComLinkSerial*>("ComLinkSerial*");
   qRegisterMetaType<ComLinkTcpCli*>("ComLinkTcpCli*");
   qRegisterMetaType<ComLinkUdpCli*>("ComLinkUdpCli*");
   qRegisterMetaType<ProtMsg*>("ProtMsg*");
   m_pIntComMsg = p_pIntComMsg;
   m_funcGeneral = m_pIntComMsg->getFuncGeneralParam();
   m_linkState = LINK_STATE_DISCONNECTED;
   m_connectionCommand = CONNECTION_COMMAND_IDLE;
   m_pInterface = nullptr;
   m_pNewMsg = nullptr;
   m_isWriting = false;
   m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
   m_pMappedDevice = nullptr;

   m_mBufferWriteMsg = new QRecursiveMutex();
   m_mBufferReadMsg = new QRecursiveMutex();
   m_mInterface = new QRecursiveMutex();

   m_timAutoConnect = new QTimer();
   m_timAutoConnect->setSingleShot(true);

   m_timSendUdpPacket = new QTimer();
   m_timSendUdpPacket->setInterval(UDP_FRAME_INTERVAL_MIN_MS);

   m_pThread = new QThread();
   m_pThread->start(QThread::HighPriority);

   m_PktHead = 0;
   m_PktTail = 0;
   m_PktSize = 0;

   m_PktHeadCmd = 0;
   m_PktTailCmd = 0;
   m_PktSizeCmd = 0;

   connect(m_timAutoConnect, &QTimer::timeout, this, &GenComLink::restartConnection, Qt::QueuedConnection);
   connect(this, &GenComLink::evRequestStartConnection, this, &GenComLink::onRequestStartConnection, (Qt::ConnectionType)(Qt::UniqueConnection | Qt::QueuedConnection));
   connect(this, &GenComLink::evRequestRestartConnection, this, &GenComLink::onRequestRestartConnection, Qt::QueuedConnection);
   connect(this, &GenComLink::evRequestStopConnection, this, &GenComLink::onRequestStopConnection, Qt::QueuedConnection);
   connect(this, &GenComLink::evSendMsg, this, &GenComLink::onSendMsg, (Qt::ConnectionType)(Qt::UniqueConnection | Qt::QueuedConnection));
   connect(this, &GenComLink::evRequestQuitThread, this, &GenComLink::onRequestQuitThread, Qt::QueuedConnection);
   connect(this, &GenComLink::evContinueDecodeMsg, this, &GenComLink::onReadData, Qt::QueuedConnection);
   connect(this, &GenComLink::evReceivedAuthReqMsg, this, &GenComLink::onReceivedAuthReqMsg, Qt::QueuedConnection);
   connect(this, &GenComLink::evReceivedAuthAnsMsg, this, &GenComLink::onReceivedAuthAnsMsg, Qt::QueuedConnection);
   connect(this, &GenComLink::stateChanged, this, &GenComLink::onStateChanged, Qt::QueuedConnection);

   connect(m_timSendUdpPacket, &QTimer::timeout, this, &GenComLink::onSendMsg2);

   m_timAutoConnect->moveToThread(m_pThread);
   this->moveToThread(m_pThread);

   m_timRequestTimeout = new QTimer();
   m_timRequestTimeout->setSingleShot(true);
   connect(m_timRequestTimeout, &QTimer::timeout, this, &GenComLink::onRequestTimeout, Qt::QueuedConnection);
   m_timRequestTimeout->moveToThread(m_pThread);

   m_timHeartbeatGenerator = new QTimer();
   connect(m_timHeartbeatGenerator, &QTimer::timeout, this, &GenComLink::onHeartbeatGenerator, Qt::QueuedConnection);
   m_timHeartbeatGenerator->moveToThread(m_pThread);

   m_timHeartbeatTimeout = new QTimer();
   m_timHeartbeatTimeout->setSingleShot(true);
   connect(m_timHeartbeatTimeout, &QTimer::timeout, this, &GenComLink::onHeartbeatTimeout, Qt::QueuedConnection);
   m_timHeartbeatTimeout->moveToThread(m_pThread);

   m_timProtoRetry = new QTimer();
   connect(m_timProtoRetry, &QTimer::timeout, this, &GenComLink::onProtoRetry, Qt::QueuedConnection);
   m_timProtoRetry->moveToThread(m_pThread);

   connect(this, &GenComLink::evReceivedAlertMsg, m_pIntComMsg, &IntComMsg::setAlert, Qt::QueuedConnection);
}

GenComLink::~GenComLink()
{  
   fLogDebug1(" GenComLink ");
}

void GenComLink::startConnection()
{

   if( LINK_STATE_CONNECTING != m_linkState && (!isConnected())
       && CONNECTION_COMMAND_START != m_connectionCommand) {
      m_connectionCommand = CONNECTION_COMMAND_START;
      // init
      m_funcGeneral = m_pIntComMsg->getFuncGeneralParam();
      setLinkState(LINK_STATE_CONNECTING);
      emit evRequestStartConnection();
   } else {
      if(  CONNECTION_COMMAND_START == m_connectionCommand ) {
         fLogWarning("A start connection is already requested or done.");
      }
   }
}

void GenComLink::restartConnection()
{
   VectorStd* v = VectorManager::getInstance()->getVectorStd();
   if(VectorStd::FUNC_CONNECTION_CONNECTED == v->getFuncStateConnection()
         || ( (!hasBeenConnected()) && m_pIntComMsg->getLinkManager()->isAnyLinkReconnecting())) {
      m_timAutoConnect->start(AUTO_CONNECT_DELAY);
      return;
   }
   if( CONNECTION_COMMAND_RESTART == m_connectionCommand ) {
      fLogError("We are already doing a restart.");
   } else {
      m_connectionCommand = CONNECTION_COMMAND_RESTART;
      emit evRequestRestartConnection();
   }
}

void GenComLink::stopConnection()
{
   if( CONNECTION_COMMAND_STOP != m_connectionCommand ) {
      m_connectionCommand = CONNECTION_COMMAND_STOP;
      if( LINK_STATE_DISCONNECTING != m_linkState && LINK_STATE_DISCONNECTED != m_linkState && LINK_STATE_ERROR != m_linkState ) {
         setLinkState(LINK_STATE_DISCONNECTING);

      }
      emit evRequestStopConnection();
   }
}

bool GenComLink::isConnected() const
{
   return LINK_STATE_CONNECTED == m_linkState || LINK_STATE_AUTHENTIFIED == m_linkState;
}

bool GenComLink::isAuthentified() const
{
   return LINK_STATE_AUTHENTIFIED == m_linkState;
}

bool GenComLink::hasMappedDevice() const
{
   return m_pMappedDevice != nullptr;
}

ComDevice *GenComLink::getMappedDevice() const
{
   return m_pMappedDevice;
}

void GenComLink::setMappedDevice(ComDevice *p_value)
{
   m_pMappedDevice = p_value;
}

ComLinkSerial *GenComLink::getLinkSerial()
{
   return nullptr;
}

ComLinkTcpCli *GenComLink::getLinkTcpCli()
{
   return nullptr;
}

ComLinkUdpCli *GenComLink::getLinkUdpCli()
{
   return nullptr;
}

void GenComLink::sendMsg(ProtMsg *p_msg)
{
    if( isProtocolD4() && !ProtD4::toWire(p_msg)) {
        delete p_msg;
        return;
    }
    if( BUFFER_SIZE_MSG_MAX > m_bufferWriteMsg.size() ) {
        m_mBufferWriteMsg->lock();
        m_bufferWriteMsg.append(p_msg);
        m_mBufferWriteMsg->unlock();
        if( !m_isWriting ) {
            m_isWriting = true;
            emit evSendMsg();
        }
    } else {
        delete p_msg;
        fLogWarning("Msg buffer write full !!");
    }
}

ProtMsg *GenComLink::takeMsg()
{
   ProtMsg * pRet = nullptr;
   m_mBufferReadMsg->lock();
   if( m_bufferReadMsg.size() ) {
      pRet = m_bufferReadMsg.takeFirst();
   }
   m_mBufferReadMsg->unlock();
   return pRet;
}

bool GenComLink::isPendingMessage() const
{
   bool bRet = false;
   m_mBufferReadMsg->lock();
   bRet = m_bufferReadMsg.size();
   m_mBufferReadMsg->unlock();
   return bRet;
}

void GenComLink::setLinkSerialConfig(ComLinkSerialConfig *)
{
   // implemented empty
}

void GenComLink::setLinkTcpConfig(ComLinkTcpConfig *)
{
   // implemented empty
}

void GenComLink::setLinkUdpConfig(ComLinkUdpConfig *p_conf)
{

}

QString GenComLink::getName() const
{
   return m_linkName;
}

void GenComLink::setName(const QString &p_value)
{
   if( p_value != m_linkName ) {
      m_linkName = p_value;
      setLogName("GenComLink-"+p_value);
      emit nameChanged(m_linkName);
   }
}

const VectorFuncGeneralAuth &GenComLink::getFuncGeneral() const
{
   return m_funcGeneral;
}

void GenComLink::setFuncGeneral(const VectorFuncGeneralAuth &p_funcGeneral)
{
   m_funcGeneral = p_funcGeneral;
}

quint32 GenComLink::getLinkState() const
{
   return m_linkState;
}

quint32 GenComLink::getCurrentCommand() const
{
   return m_connectionCommand;
}

void GenComLink::onRequestStartConnection()
{
   if( nullptr != m_pInterface ) {
      // should never happen
      fLogError("An interface is already instanciated !");
   } else {
      bool retry = false;
      if( CONNECTION_COMMAND_START == m_connectionCommand && (LINK_STATE_CONNECTING == m_linkState)) {
         if( m_timAutoConnect->isActive() ) {
            m_timAutoConnect->stop();
         }

         // Open the interface
         m_mInterface->lock();
         m_pInterface = makeInterface();

         bool isOpen = false;

         if( nullptr != m_pInterface ) {
            isOpen = tryOpen();
         }
         m_mInterface->unlock();

         if( !isOpen ) {
            // auto retry
            m_bHasBeenConnected = false;
            retry = true;
         } else {
            // connection successfull !!
            // connect slots
            connect(m_pInterface, &QIODevice::readyRead, this, &GenComLink::onReadData, Qt::QueuedConnection);
            // reset decode
            m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
            // update state
            setLinkState(LINK_STATE_CONNECTED);

         }

         if( retry ) {
            m_timAutoConnect->start(AUTO_CONNECT_DELAY);
         }
      } else {
         fLogError(QString("Bad state : linkState(%1) command(%2)").arg(m_linkState).arg(m_connectionCommand));
      }
   }
}

void GenComLink::onRequestStopConnection()
{
   // -- stop timers
   if( m_timAutoConnect->isActive() ) {
      m_timAutoConnect->stop();
   }

   stopRequestTimeout();
   stopHeartbeatGenerator();
   stopHeartbeatTimeout();

   // -- kill interface

   if( nullptr != m_pInterface ) {
      // stop timer for auto connection
      removeLink();
   }
   setLinkState(LINK_STATE_DISCONNECTED);
   if( m_connectionCommand == CONNECTION_COMMAND_RESTART ) {
      startConnection();
   }
   //                // start new connection
   ////                setLinkState(LINK_STATE_CONNECTING);
   ////                emit evRequestStartConnection();
   //                // The command has been treated

}

void GenComLink::onRequestRestartConnection()
{
   removeLink();
   if( LINK_STATE_CONNECTING == m_linkState || isConnected() ) {
      // We are starting, so request a stop then a start
      setLinkState(LINK_STATE_DISCONNECTING);
      emit evRequestStopConnection();
   } else if( LINK_STATE_DISCONNECTING == m_linkState || LINK_STATE_DISCONNECTED == m_linkState || LINK_STATE_ERROR == m_linkState ) {
      // We are not connected, request a start
      startConnection();
   } else {
      fLogWarning("Nothing to do to restart.");
   }
}

void GenComLink::onRequestQuitThread()
{
   // delete timer owned by this thread
   if( nullptr != m_timAutoConnect ) {
      if( m_timAutoConnect->isActive() ) {
         m_timAutoConnect->stop();
      }
      delete m_timAutoConnect;
      m_timAutoConnect = nullptr;
   }

   removeLink();

   // Request quit thread
   m_pThread->quit();
}

void GenComLink::onReceivedAuthReqMsg(ProtMsg *pMsg)
{
   stopRequestTimeout();
   // We are a DEVICE
   if( sendAuthAnswer((qint8*)pMsg->m_payload) ) {
      setLinkState(LINK_STATE_AUTHENTIFIED);
   } else {
      linkError("Authentification failed");
   }
   delete pMsg;
}

void GenComLink::onReceivedAuthAnsMsg(ProtMsg * pMsg)
{
   // We are MPC
   if( isAuthentified() || m_d4WaitingFunctions ) {
       delete pMsg;
       return;
   }
   stopRequestTimeout();
   m_timProtoRetry->stop();

   bool bOk = false;
   switch( pMsg->m_len ){
   case ProtMsg::ANS_AUTH_LEN_LEGACY:
       bOk = checkAuth((qint8*)pMsg->m_payload) && decodeAuthAnsLegacy(pMsg);
       break;
   case ProtMsg::ANS_AUTH_LEN_D4:
       bOk = ProtD4::verifyAuthAnswer(m_auth, pMsg->m_payload) && decodeAuthAnsD4(pMsg);
       break;
   default:
      fLogError(QString("C_GENERAL_ANS_AUTH : unknown protocol (LEN = %1, expected %2 or %3)")
                .arg(pMsg->m_len).arg(ProtMsg::ANS_AUTH_LEN_LEGACY).arg(ProtMsg::ANS_AUTH_LEN_D4));
      bOk = false;
      break;
   }

   m_bHasBeenConnected = bOk;
   if( !bOk ) {
      linkError("Unsupported authentification answer !");
   } else {
      fLogInfo(QString("Protocol detected : %1 (generation %2), product %3")
               .arg(ProtMsg::protocolName(m_funcGeneral.m_protocolVersion))
               .arg(m_funcGeneral.m_protocolGeneration)
               .arg(m_funcGeneral.m_paramDProdId));
       if( isProtocolD4() ) {
           m_d4WaitingFunctions = true;
           startHeartbeatGenerator();
           startHeartbeatTimeout();
           onHeartbeatGenerator();
           sendRequestFunc();
           m_timProtoRetry->start(ProtMsg::D4_RETRY_MS);
       } else {
           setLinkState(LINK_STATE_AUTHENTIFIED);
       }
   }
   delete pMsg;
}

quint8 GenComLink::getProtocolVersion() const
{
   return m_funcGeneral.m_protocolVersion;
}

bool GenComLink::isProtocolD4() const
{
   return ProtMsg::PROTOCOL_D4 == m_funcGeneral.m_protocolVersion;
}

bool GenComLink::decodeAuthAnsLegacy(const ProtMsg *pMsg)
{
   {
      // decode device info
      qint32 offset = AUTH_DATA_SIZE;
      m_funcGeneral.m_paramDProdId = pMsg->m_payload[offset];
      offset++;
      m_funcGeneral.m_paramDDeviceAd = pMsg->m_payload[offset];
      offset++;
      m_funcGeneral.m_lFunctions.clear();
      for(int i=0; i<20; i++) {
         qint8 f = pMsg->m_payload[offset];
         if( 0 != f ) {
            m_funcGeneral.m_lFunctions.append(f);
         }
         offset++;
      }
      m_funcGeneral.m_paramDProdUid = QByteArray((char*)(&pMsg->m_payload[offset]), 16);
      offset+=16;
      m_funcGeneral.m_paramDVerHw = QByteArray((char*)(&pMsg->m_payload[offset]), 12);
      offset+=12;
      m_funcGeneral.m_paramDVerSw = QByteArray((char*)(&pMsg->m_payload[offset]), 12);
      offset+=12;
      memcpy(&m_funcGeneral.m_paramDDateBuild, &pMsg->m_payload[offset],8);
      offset+=8;
      memcpy(&m_funcGeneral.m_paramDDateRev, &pMsg->m_payload[offset],8);
      offset+=8;

      if( offset != pMsg->m_len ) {
         fLogError(QString("Offset = %1, payload len = %2").arg(offset).arg(pMsg->m_len));
      }
   }
   m_funcGeneral.m_protocolVersion = ProtMsg::PROTOCOL_LEGACY;
   m_funcGeneral.m_protocolGeneration = 0;
   return true;
}

bool GenComLink::decodeAuthAnsD4(const ProtMsg *pMsg)
{
   const quint8 * p = pMsg->m_payload;

   // -- PROTOCOL_KEY : family must be 0xD4 and the generation must be supported
   const quint8 family = p[ProtMsg::ANS_AUTH_D4_OFS_PROTO_KEY];
   const quint8 generation = p[ProtMsg::ANS_AUTH_D4_OFS_PROTO_KEY + 1];
   if( ProtMsg::PROTOCOL_KEY_FAMILY_D4 != family || ProtMsg::PROTOCOL_KEY_GENERATION_D4 != generation ) {
      fLogError(QString("C_GENERAL_ANS_AUTH D4 : unsupported PROTOCOL_KEY 0x%1 0x%2 (expected 0x%3 0x%4)")
                .arg(family, 2, 16, QLatin1Char('0')).arg(generation, 2, 16, QLatin1Char('0'))
                .arg(ProtMsg::PROTOCOL_KEY_FAMILY_D4, 2, 16, QLatin1Char('0'))
                .arg(ProtMsg::PROTOCOL_KEY_GENERATION_D4, 2, 16, QLatin1Char('0')));
      return false;
   }

   // null terminated strings, bounded to their field size
   auto readCString = [p](int offset, int size) {
      const char * s = reinterpret_cast<const char *>(p + offset);
      return QString::fromLatin1(s, int(qstrnlen(s, size)));
   };

   m_funcGeneral.m_paramDProdId = p[ProtMsg::ANS_AUTH_D4_OFS_PROD_ID];
   m_funcGeneral.m_paramDDeviceAd = p[ProtMsg::ANS_AUTH_D4_OFS_DEVICE_AD];
   m_funcGeneral.m_paramDProdUid = readCString(ProtMsg::ANS_AUTH_D4_OFS_PROD_UID, ProtMsg::ANS_AUTH_D4_PROD_UID_SIZE);
   m_funcGeneral.m_paramDVerHw = readCString(ProtMsg::ANS_AUTH_D4_OFS_VER_HW, ProtMsg::ANS_AUTH_D4_VER_SIZE);
   m_funcGeneral.m_paramDVerSw = readCString(ProtMsg::ANS_AUTH_D4_OFS_VER_SW, ProtMsg::ANS_AUTH_D4_VER_SIZE);
   memcpy(&m_funcGeneral.m_paramDDateBuild, &p[ProtMsg::ANS_AUTH_D4_OFS_DATE_BUILD], 8);
   memcpy(&m_funcGeneral.m_paramDDateRev, &p[ProtMsg::ANS_AUTH_D4_OFS_DATE_REV], 8);

   // In D4 the function list is NOT in the auth answer :
   // it is requested afterwards with C_GENERAL_REQUEST_FUNC / C_GENERAL_ANS_FUNC
   m_funcGeneral.m_lFunctions.clear();

   m_funcGeneral.m_protocolVersion = ProtMsg::PROTOCOL_D4;
   m_funcGeneral.m_protocolGeneration = generation;
   return true;
}

void GenComLink::onStateChanged(quint32 state)
{
   // -- post actions after state changed
    if( state == LINK_STATE_CONNECTED ) {
        if( m_funcGeneral.m_paramDDeviceAd == ProtMsg::DEVICE_AD_MPC ) {
            // Send a authentification request
            QThread::msleep(500);   ///< THEO request a time between the connection and the send of AuthRequest
            sendAuthRequest();
            if(LINK_UDP == getType()) {
                m_authRetryCount = 0;
                m_timProtoRetry->start(ProtMsg::D4_RETRY_MS);
            }else{
                // Start timeout for Auth.
                startRequestTimeout();
            }
        }
    }

    if( state == LINK_STATE_AUTHENTIFIED ) {
        startHeartbeatTimeout();
    }
}

void GenComLink::linkError(const QString & p_msg)
{
   Q_UNUSED(p_msg)
   emit evLinkError();
   connectionError();
   fLogError(QString("Link error : %1").arg(p_msg)); // flood
}

void GenComLink::onRequestTimeout()
{
   m_bHasBeenConnected = false;
   linkError("Timeout on request");
}

void GenComLink::onHeartbeatGenerator()
{
    // Send msg
    if( nullptr != m_mInterface && (LINK_STATE_AUTHENTIFIED == m_linkState || m_d4WaitingFunctions)) {
        ProtMsg * pMsg = new ProtMsg();
        pMsg->m_deviceAd = m_funcGeneral.m_paramDDeviceAd;
        pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
        pMsg->m_msgType = ProtMsg::MSG_TYPE_EVENT;
        pMsg->m_msgObj = ProtMsg::FUNC_GENERAL_EVENT_E_HEART_BEAT;
        sendMsg(pMsg);
    }
}

void GenComLink::onProtoRetry()
{
    if( m_d4WaitingFunctions ) {
        sendRequestFunc();
    } else if( LINK_STATE_CONNECTED == m_linkState ) {
        sendAuthRequestFrame();
        if( ++m_authRetryCount >= AUTH_RETRY_BEFORE_RESET ) {
            m_bHasBeenConnected = false;
        }
    } else {
        m_timProtoRetry->stop();
    }
}

void GenComLink::sendRequestFunc()
{
    ProtMsg * pMsg = new ProtMsg();
    pMsg->m_deviceAd = m_funcGeneral.m_paramDDeviceAd;
    pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
    pMsg->m_msgType = ProtMsg::MSG_TYPE_CMD;
    pMsg->m_msgObj = ProtMsg::FUNC_GENERAL_CMD_C_REQUEST_FUNC;
    sendMsg(pMsg);
}

void GenComLink::processAnsFunc(const ProtMsg *pMsg)
{
    if( !m_d4WaitingFunctions ) {
        return;
    }
    bool bOk = false;
    const QList<quint32> functions = ProtD4::decodeFunctionList(pMsg->m_payload, pMsg->m_len, &bOk);
    if( !bOk ) {
        fLogError(QString("C_GENERAL_ANS_FUNC non valido (LEN = %1)").arg(pMsg->m_len));
        return;
    }
    m_funcGeneral.m_lFunctions = functions;
    m_d4WaitingFunctions = false;
    m_timProtoRetry->stop();
    fLogInfo(QString("D4 : %1 funzioni disponibili").arg(functions.size()));
    setLinkState(LINK_STATE_AUTHENTIFIED);
}

void GenComLink::onHeartbeatTimeout()
{
   linkError("Timeout on heartbeat");
}

void GenComLink::connectionError()
{
   setLinkState(LINK_STATE_ERROR);
   removeLink();
   if( CONNECTION_COMMAND_STOP != m_connectionCommand ) {
      m_timAutoConnect->start(AUTO_CONNECT_DELAY);
   }
}

void GenComLink::setLinkState(quint32 p_state)
{
   if( m_linkState != p_state ) {
      if( m_linkState == LINK_STATE_CONNECTED && p_state != LINK_STATE_AUTHENTIFIED) {
         fLogInfo(QString("CONNECTION LOST : %1").arg(getConfigHelp()));
      } else if( p_state == LINK_STATE_CONNECTED ) {
         fLogInfo(QString("CONNECTION ESTABLISHED : %1").arg(getConfigHelp()));
      } else if( p_state == LINK_STATE_AUTHENTIFIED ) {
         fLogInfo(QString("CONNECTION AUTH : %1").arg(getConfigHelp()));
      }
      qint32 old = m_linkState;
      m_linkState = p_state;
      emit evLinkStateChanged(this, old, m_linkState);
      emit stateChanged(m_linkState);
   }
}

void GenComLink::removeLink()
{
    if( m_timProtoRetry->isActive() ) {
        m_timProtoRetry->stop();
    }
    m_d4WaitingFunctions = false;
    stopRequestTimeout();
    stopHeartbeatGenerator();
    stopHeartbeatTimeout();

    // get mutex to be sure nobody is using buffer while deleting
    m_mInterface->lock();
    // close interface
    if( nullptr != m_pInterface ) {
        m_pInterface->close();
        m_pInterface->deleteLater();
        m_pInterface = nullptr;
    }
    // delete buffers
    m_bufferRead.clear();
    m_mInterface->unlock();

    m_mBufferReadMsg->lock();
    while(m_bufferReadMsg.size()) {
        delete m_bufferReadMsg.takeLast();
    }
    m_mBufferReadMsg->unlock();

    m_mBufferWriteMsg->lock();
    while(m_bufferWriteMsg.size()) {
        delete m_bufferWriteMsg.takeLast();
    }
    m_mBufferWriteMsg->unlock();
    m_isWriting = false;
}

void GenComLink::deleteLink()
{
   // -- delete objects

   emit evRequestQuitThread();
   m_pThread->wait(1000);
   if( m_pThread->isRunning() ) {
      fLogError("Thread still running");
      m_pThread->terminate();
   }

   delete m_mBufferWriteMsg;
   m_mBufferWriteMsg = nullptr;
   delete m_mBufferReadMsg;
   m_mBufferReadMsg = nullptr;
   delete m_mInterface;
   m_mInterface = nullptr;

   delete m_pThread;
   m_pThread = nullptr;

}

int GenComLink::getLinkTypeTimeout()
{
   return ProtMsg::TIMEOUT_MS;
}

int GenComLink::getLinkTypeHeartBeatDelay()
{
   return ProtMsg::HEARTBEAT_DELAY_MS;
}

bool GenComLink::hasBeenConnected() const
{
   return m_bHasBeenConnected;
}

void GenComLink::onReadData()
{
   if( isConnected() && CONNECTION_COMMAND_START == m_connectionCommand ) {
      if( BUFFER_SIZE_MAX < m_bufferRead.size() ) {
         // Sheet, buffer full => loosing data
         fLogWarning("Buffer READ is full !");
      } else {
         // copy new data into our buffer
         m_mInterface->lock();
         if( nullptr != m_pInterface ) {
            const QByteArray & ba = m_pInterface->readAll();
            if( 0 != ba.size() ) {
               m_bufferRead.append(ba);
            }
         } else {
            fLogError("m_pInterface is NULL.");
         }
         m_mInterface->unlock();
      }
   }

   bool continueDecode = true;

   while(continueDecode && m_bufferRead.size()  && isConnected() ) {
      if( MSG_DECODE_STEP_MAGIC_NUMBER == m_msgDecodeStep ) {
         while(m_bufferRead.size() && ProtMsg::MAGIC_NUMBER != (quint8)m_bufferRead[0] && isConnected() ) {
            m_bufferRead.remove(0,1);
         }

         if( m_bufferRead.size() && ProtMsg::MAGIC_NUMBER == (quint8)m_bufferRead[0] ) {
            m_bufferRead.remove(0,1);
            m_msgDecodeStep = MSG_DECODE_STEP_HEADER;
         } else {
            // magic number not found
            continueDecode = false;
         }
      }

      if( MSG_DECODE_STEP_HEADER == m_msgDecodeStep && isConnected() ) {
         if( m_bufferRead.size() >= 7 ) {
            // Header can be decoded
            m_pNewMsg = new ProtMsg();
            m_pNewMsg->m_len = *(reinterpret_cast<quint16*>(&m_bufferRead.data()[0]));
            m_pNewMsg->m_deviceAd = m_bufferRead[2];
            m_pNewMsg->m_msgType = m_bufferRead.data()[3];
            m_pNewMsg->m_deviceFunc = m_bufferRead.data()[4];
            m_pNewMsg->m_msgObj = *(reinterpret_cast<quint16*>(&m_bufferRead.data()[5]));
            m_bufferRead.remove(0,7);

            // check the header is correct :
            bool bOk = true;
            const qint32 maxPayload = isProtocolD4() ? qint32(ProtMsg::COM_PAYLOAD_SIZE) : qint32(ProtMsg::COM_PAYLOAD_SIZE_LEGACY);
            if( m_pNewMsg->m_len > maxPayload ) {
               fLogError(QString("MSG ERR : bad len (%1)").arg(m_pNewMsg->m_len));
               bOk = false;
            } else if( m_pNewMsg->m_msgType >= ProtMsg::MSG_TYPE_END_ENUM) {
               fLogError(QString("MSG ERR : bad msg type (%1)").arg(m_pNewMsg->m_msgType));
               bOk = false;
            } else if( !isProtocolD4() && m_pNewMsg->m_deviceFunc >= ProtMsg::DEVICE_FUNC_END_ENUM) {
               fLogError(QString("MSG ERR : bad msg funcId (%1)").arg(m_pNewMsg->m_deviceFunc));
               bOk = false;
            }

            if( bOk ) {
               m_msgDecodeStep = MSG_DECODE_STEP_PAYLOAD;
            } else {
               m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
            }
         } else {
            // not enought data to decode the header
            continueDecode = false;
         }
      }

      if( MSG_DECODE_STEP_PAYLOAD == m_msgDecodeStep && isConnected() ) {
         if( m_bufferRead.size() >= m_pNewMsg->m_len ) {
            // Payload can be decoded
            if( m_pNewMsg->m_len ) {
                memcpy( m_pNewMsg->m_payload, m_bufferRead.data(), m_pNewMsg->m_len);
                m_bufferRead.remove(0,m_pNewMsg->m_len);
            }

            if( isProtocolD4() ) {
                const bool isAuthAnswer = ProtMsg::DEVICE_FUNC_GENERAL == m_pNewMsg->m_deviceFunc
                                          && ProtMsg::MSG_TYPE_CMD == m_pNewMsg->m_msgType
                                          && ProtMsg::FUNC_GENERAL_CMD_C_ANS_AUTH == m_pNewMsg->m_msgObj;
                bool accepted = (isAuthAnswer || m_pNewMsg->m_deviceAd == quint8(m_funcGeneral.m_paramDDeviceAd))
                                && ProtD4::fromWire(m_pNewMsg);
                if( accepted && m_pNewMsg->m_deviceFunc >= ProtMsg::DEVICE_FUNC_ASCAN_0
                    && m_pNewMsg->m_deviceFunc <= ProtMsg::DEVICE_FUNC_ASCAN_3 ) {
                    accepted = m_funcGeneral.m_lFunctions.contains(quint32(m_pNewMsg->m_deviceFunc));
                }
                if( !accepted ) {
                    delete m_pNewMsg;
                    m_pNewMsg = nullptr;
                    m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
                    continue;
                }
            }

            // special cases (low level) :
            bool msgTreated = false;

            if( ProtMsg::DEVICE_FUNC_GENERAL == m_pNewMsg->m_deviceFunc ) {
                if( ProtMsg::MSG_TYPE_CMD == m_pNewMsg->m_msgType &&
                    ProtMsg::FUNC_GENERAL_CMD_C_REQ_AUTH == m_pNewMsg->m_msgObj ) {
                    emit evReceivedAuthReqMsg(m_pNewMsg);
                    msgTreated = true;
                } else if( ProtMsg::MSG_TYPE_CMD == m_pNewMsg->m_msgType &&
                           ProtMsg::FUNC_GENERAL_CMD_C_ANS_AUTH == m_pNewMsg->m_msgObj ) {
                    emit evReceivedAuthAnsMsg(m_pNewMsg);
                    msgTreated = true;
                } else if( isProtocolD4() && ProtMsg::MSG_TYPE_CMD == m_pNewMsg->m_msgType &&
                            ProtMsg::FUNC_GENERAL_CMD_C_ANS_FUNC == m_pNewMsg->m_msgObj ) {
                    processAnsFunc(m_pNewMsg);
                    delete m_pNewMsg;
                    msgTreated = true;
                } else if( ProtMsg::MSG_TYPE_EVENT == m_pNewMsg->m_msgType &&
                           ProtMsg::FUNC_GENERAL_EVENT_E_HEART_BEAT == m_pNewMsg->m_msgObj ) {
                    startHeartbeatTimeout();    // restart heartbeat timer
                    delete m_pNewMsg;
                    msgTreated = true;
                } else if( ProtMsg::MSG_TYPE_EVENT == m_pNewMsg->m_msgType &&
                           ProtMsg::FUNC_GENERAL_EVENT_E_LOG == m_pNewMsg->m_msgObj ) {
                    quint8 logLevel = m_pNewMsg->m_payload[0];
                    QString logMsg = QByteArray((char*)&(m_pNewMsg->m_payload[1]),m_pNewMsg->m_len-1);
                    fLogInfo(QString("LOG FROM %1 : [%2] - %3").arg(getName()).arg(logLevel).arg(logMsg));
                    delete m_pNewMsg;
                    msgTreated = true;
                } else if ( ProtMsg::MSG_TYPE_EVENT == m_pNewMsg->m_msgType &&
                            ProtMsg::FUNC_GENERAL_EVENT_E_ERROR == m_pNewMsg->m_msgObj ) {
                    quint32 logLevel;
                    memcpy(&logLevel, &m_pNewMsg->m_payload[0], 4);
                    QString logMsg = QByteArray((char*)&(m_pNewMsg->m_payload[1]),m_pNewMsg->m_len-4);
                    fLogError(QString("ERROR FROM %1 : [%2] - %3").arg(getName()).arg(logLevel).arg(logMsg));
                    delete m_pNewMsg;
                    msgTreated = true;
                } else if ( ProtMsg::MSG_TYPE_EVENT == m_pNewMsg->m_msgType &&
                            ProtMsg::FUNC_GENERAL_EVENT_E_ALERT_MESSAGE == m_pNewMsg->m_msgObj ) {
                    quint8 alertLevel = m_pNewMsg->m_payload[0];
                    const char * alertText = reinterpret_cast<const char *>(&m_pNewMsg->m_payload[1]);
                    const int alertMax = qMax(0, int(m_pNewMsg->m_len) - 1);
                    QString alertMsg = QString::fromLatin1(alertText, int(qstrnlen(alertText, uint(alertMax))));
                    if(alertLevel > 2) {
                        fLogError(QString("ERROR : ALERT LEVEL OUT OF RANGE ! [%1]").arg(alertLevel));
                    } else {
                        emit evReceivedAlertMsg(alertLevel, alertMsg);
                    }
                    delete m_pNewMsg;
                    msgTreated = true;
                }
            }

            if( !msgTreated ){
                if( isAuthentified() ) {
                    m_mBufferReadMsg->lock();
                    if( BUFFER_SIZE_MSG_MAX > m_bufferReadMsg.size() ) {
                        m_bufferReadMsg.append(m_pNewMsg);
                        if( !isProtocolD4() ) {
                            startHeartbeatTimeout();    // restart heartbeat timer
                        }
                        emit evReceivedNewMsg();
                    } else {
                        delete m_pNewMsg;
                        fLogWarning("Msg buffer read full !!");
                    }
                    m_mBufferReadMsg->unlock();
                } else {
                    fLogInfo(QString("Link not authenified, message deleted (Type %1, Func %2, Obj %3).")
                            .arg(m_pNewMsg->m_msgType).arg(m_pNewMsg->m_deviceFunc).arg(m_pNewMsg->m_msgObj));
                    delete m_pNewMsg;
                }
            }
            m_pNewMsg = nullptr;

            // ready to decode a new message
            m_msgDecodeStep = MSG_DECODE_STEP_MAGIC_NUMBER;
            } else {
                // not enought data to decode the payload
                continueDecode = false;
            }
        }
    }
}

void GenComLink::onSendMsg()
{
   bool bContinue = true;
   ProtMsg * pMsg = nullptr;

   m_mBufferWriteMsg->lock();
   if( m_bufferWriteMsg.size() ) {
      pMsg = m_bufferWriteMsg.takeFirst();
   } else {
      bContinue = false;
   }
   m_mBufferWriteMsg->unlock();
   m_mInterface->lock();
   bContinue &= (m_mInterface != nullptr) && (isConnected());
   m_mInterface->unlock();

   while( bContinue ) {
      // fill the buffer
      m_buffSend[0] = ProtMsg::MAGIC_NUMBER;
      memcpy(&m_buffSend[1], &(pMsg->m_len), 2);
      m_buffSend[3] = static_cast<quint8>(pMsg->m_deviceAd);
      m_buffSend[4] = static_cast<quint8>(pMsg->m_msgType);
      m_buffSend[5] = static_cast<quint8>(pMsg->m_deviceFunc);
      memcpy(&m_buffSend[6], &(pMsg->m_msgObj), 2);
      memcpy(&m_buffSend[8], pMsg->m_payload, pMsg->m_len);

      // DEBUG

      // always connected ?
      m_mInterface->lock();
      bContinue &= (m_mInterface!=nullptr);
      qint32 buffSize;
      qint64 s;
      if( bContinue ) {
         buffSize = ProtMsg::COM_HEADER_SIZE + pMsg->m_len;
         s = m_pInterface->write(m_buffSend, buffSize);
      }
      m_mInterface->unlock();
      if( bContinue && s != buffSize ) {
         linkError(QString("Fail to write data (size = %1)").arg(buffSize));
      }

      // message treated
      delete pMsg;

      bContinue &= isConnected();

      if( bContinue ) {
         // test if we have more messages to send
         m_mBufferWriteMsg->lock();
         if( !m_bufferWriteMsg.size() ) {
            bContinue = false;
         } else {
            pMsg = m_bufferWriteMsg.takeFirst();
         }
         m_mBufferWriteMsg->unlock();
      }
   }
   m_isWriting = false;
}

void GenComLink::onSendMsg2()
{

   if(m_PktSizeCmd != 0)
   {

     m_PktHeadCmd = (m_PktHeadCmd+1) % 1024;

     m_PktSizeCmd--;
   }
   else
   {
     if(m_PktSize != 0)
     {

         m_PktHead = (m_PktHead+1) % 1024;

         m_PktSize--;
     }
   }

}

bool GenComLink::customLoadSettings(QDomElement * p_pParent)
{
   bool bRet = true;
   bRet &= XML_READ_UNIQUE(QString, m_linkName);
   return bRet;
}
bool GenComLink::customLoadDefaultSettings()
{
   bool bRet = true;

   return bRet;
}
bool GenComLink::customApplySettings()
{
   bool bRet = true;

   return bRet;
}
bool GenComLink::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
   bool bRet = true;
   bRet &= XML_WRITE_UNIQUE(m_linkName);
   return bRet;
}

void GenComLink::sendAuthRequest()
{
   // init data with random value
   for(int i=0; i<AUTH_DATA_SIZE; i++) {
      m_auth[i] = QRandomGenerator::global()->generate();
   }
   // add strange relations :
   m_auth[3] = m_auth[5] + m_auth[15] + 15;
   m_auth[9] = m_auth[1] - m_auth[17] - 9;
   m_auth[11] = m_auth[30] + m_auth[29] + 7;
   m_auth[18] = m_auth[7] + m_auth[14] - 2;
   m_auth[23] = m_auth[12] * m_auth[0] + 99;
   m_auth[27] = m_auth[10] - m_auth[20] - 56;
   m_auth[31] = m_auth[28] * m_auth[13] + 12;
   sendAuthRequestFrame();
}

void GenComLink::sendAuthRequestFrame()
{
   ProtMsg * pMsg = new ProtMsg();
   pMsg->m_deviceAd = m_funcGeneral.m_paramDDeviceAd;
   pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
   pMsg->m_msgType = ProtMsg::MSG_TYPE_CMD;
   pMsg->m_msgObj = ProtMsg::FUNC_GENERAL_CMD_C_REQ_AUTH;
   pMsg->m_len = AUTH_DATA_SIZE;
   memcpy(pMsg->m_payload, m_auth, AUTH_DATA_SIZE);
   sendMsg(pMsg);
}

bool GenComLink::generateAuthAnswer(const qint8* p_authIn, qint8* p_authAns) const
{
   bool bRet = true;

   // check if p_authIn is well generated
   bRet &= ((qint8)p_authIn[3] == (qint8)(p_authIn[5] + p_authIn[15] + 15));
   bRet &= ((qint8)p_authIn[9] == (qint8)(p_authIn[1] - p_authIn[17] - 9));
   bRet &= ((qint8)p_authIn[11] == (qint8)(p_authIn[30] + p_authIn[29] + 7));
   bRet &= ((qint8)p_authIn[18] == (qint8)(p_authIn[7] + p_authIn[14] - 2));
   bRet &= ((qint8)p_authIn[23] == (qint8)(p_authIn[12] * p_authIn[0] + 99));
   bRet &= ((qint8)p_authIn[27] == (qint8)(p_authIn[10] - p_authIn[20] - 56));
   bRet &= ((qint8)p_authIn[31] == (qint8)(p_authIn[28] * p_authIn[13] +12));

   if( bRet ) {
      // generate answer
      // init data with random value
      for(int i=0; i<AUTH_DATA_SIZE; i++) {
         p_authAns[i] = QRandomGenerator::global()->generate();
      }
      // process a strange behaviour :
      p_authAns[1] = p_authIn[6]+p_authIn[8] + 5;
      p_authAns[2] = p_authIn[2]-p_authIn[20] - 98;
      p_authAns[4] = p_authIn[30]*p_authIn[12] + 77;
      p_authAns[8] = p_authIn[9]+p_authIn[5] + 19;
      p_authAns[10] = p_authIn[6]-p_authIn[8] + 47;
      p_authAns[15] = p_authIn[25]+p_authIn[21] - 74;
      p_authAns[20] = p_authIn[1]*p_authIn[14] - 108;
      p_authAns[25] = p_authIn[18]-p_authIn[7] + 209;
      p_authAns[30] = p_authIn[11]+p_authIn[19] - 76;
   }

   return bRet;
}

bool GenComLink::sendAuthAnswer(const qint8* p_authIn)
{
   bool bRet = true;

   // -- create msg
   ProtMsg * pMsg = new ProtMsg();
   pMsg->m_deviceAd = m_funcGeneral.m_paramDDeviceAd;
   pMsg->m_deviceFunc = ProtMsg::DEVICE_FUNC_GENERAL;
   pMsg->m_msgType = ProtMsg::MSG_TYPE_CMD;
   pMsg->m_msgObj = ProtMsg::FUNC_GENERAL_CMD_C_ANS_AUTH;
   pMsg->m_len = 110;

   // -- fill payload
   quint32 offset = 0;
   // auth data
   bRet &= generateAuthAnswer(p_authIn, (qint8*)pMsg->m_payload);

   if( bRet ) {
      offset = AUTH_DATA_SIZE;

      // other data
      pMsg->m_payload[offset] = m_funcGeneral.m_paramDProdId;
      offset++;
      pMsg->m_payload[offset] = m_funcGeneral.m_paramDDeviceAd;
      offset++;
      for(int i=0; i<20; i++) {
         qint8 v = 0;
         if( i<m_funcGeneral.m_lFunctions.size() ) {
            v = m_funcGeneral.m_lFunctions.at(i);
         }
         pMsg->m_payload[offset] = v;
         offset++;
      }
      for(int i=0; i<16; i++) {
         qint8 v = 0;
         if( i<m_funcGeneral.m_paramDProdUid.size() ) {
            v = m_funcGeneral.m_paramDProdUid.toLatin1().at(i);
         }
         pMsg->m_payload[offset] = v;
         offset++;
      }
      for(int i=0; i<12; i++) {
         qint8 v = 0;
         if( i<m_funcGeneral.m_paramDVerHw.size() ) {
            v = m_funcGeneral.m_paramDVerHw.toLatin1().at(i);
         }
         pMsg->m_payload[offset] = v;
         offset++;
      }
      for(int i=0; i<12; i++) {
         qint8 v = 0;
         if( i<m_funcGeneral.m_paramDVerSw.size() ) {
            v = m_funcGeneral.m_paramDVerSw.toLatin1().at(i);
         }
         pMsg->m_payload[offset] = v;
         offset++;
      }
      memcpy(&pMsg->m_payload[offset], &m_funcGeneral.m_paramDDateBuild, 8);
      offset+=8;
      memcpy(&pMsg->m_payload[offset], &m_funcGeneral.m_paramDDateRev, 8);
      offset+=8;

      if( offset != pMsg->m_len ) {
         fLogError(QString("Offset = %1, payload len = %2").arg(offset).arg(pMsg->m_len));
      }

      sendMsg(pMsg);
   }
   return bRet;
}

bool GenComLink::checkAuth(const qint8 *p_answer) const
{
   qint8 answerExpected[AUTH_DATA_SIZE];
   generateAuthAnswer(m_auth, answerExpected);

   bool bRet = true;
   bRet &= (answerExpected[1] == p_answer[1]);
   bRet &= (answerExpected[2] == p_answer[2]);
   bRet &= (answerExpected[4] == p_answer[4]);
   bRet &= (answerExpected[8] == p_answer[8]);
   bRet &= (answerExpected[10] == p_answer[10]);
   bRet &= (answerExpected[15] == p_answer[15]);
   bRet &= (answerExpected[20] == p_answer[20]);
   bRet &= (answerExpected[25] == p_answer[25]);
   bRet &= (answerExpected[30] == p_answer[30]);

   return bRet;
}

// -- Timers
void GenComLink::startRequestTimeout()
{
   stopRequestTimeout();
   m_timRequestTimeout->start(getLinkTypeTimeout());
}

void GenComLink::stopRequestTimeout()
{
   if( m_timRequestTimeout->isActive() ) {
      m_timRequestTimeout->stop();
   }
}

void GenComLink::startHeartbeatGenerator()
{
   stopHeartbeatGenerator();
   m_timHeartbeatGenerator->start(isProtocolD4() ? int(ProtMsg::D4_HEARTBEAT_DELAY_MS) : getLinkTypeHeartBeatDelay());
}

void GenComLink::stopHeartbeatGenerator()
{
   if( m_timHeartbeatGenerator->isActive() ) {
      m_timHeartbeatGenerator->stop();
   }
}

void GenComLink::startHeartbeatTimeout()
{
   stopHeartbeatTimeout();
   m_timHeartbeatTimeout->start(isProtocolD4() ? int(ProtMsg::D4_TIMEOUT_MS) : getLinkTypeTimeout());
}

void GenComLink::stopHeartbeatTimeout()
{
   if( m_timHeartbeatTimeout->isActive() ) {
      m_timHeartbeatTimeout->stop();
   }
}

