/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncMove.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncMove.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes

const qreal VectorFuncMove::MOVE_KEY_FACTOR = 0.5;
const quint32 VectorFuncMove::NO_MOVEMENT_UPDATE_MS_INTERVAL = 25;

VectorFuncMove::VectorFuncMove(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncMove", p_pVectorStd, ProtMsg::DEVICE_FUNC_MOVE)
{
    qRegisterMetaType<VectorFuncMove*>();
    m_tCmdAnsTimeout = new QTimer();
    m_tCmdAnsTimeout->setSingleShot(true);
    connect(m_tCmdAnsTimeout, &QTimer::timeout, this, &VectorFuncMove::onCmdAnsTimeout);

    m_tNoMovementUpdate = new QTimer();
    m_tNoMovementUpdate->setInterval(NO_MOVEMENT_UPDATE_MS_INTERVAL);
    connect(m_tNoMovementUpdate, &QTimer::timeout, this, &VectorFuncMove::onNoMovementUpdate);

    m_pendingCmd = MOVE_NO_CMD;
    m_pendingJoyX = 0;
    m_pendingJoyY = 0;
    m_joyInit = false;
    m_moveDir = MOVE_NO_DIRECTION;
    m_moveKeyDir = MOVE_NO_DIRECTION;
    m_moveBoost = false;
    m_manualMode = true;

    m_tNoMovementUpdate->start();
}

VectorFuncMove::~VectorFuncMove()
{
    if( m_tCmdAnsTimeout->isActive() ) {
        m_tCmdAnsTimeout->stop();
    }
    delete m_tCmdAnsTimeout;
    m_tCmdAnsTimeout = nullptr;

    if(m_tNoMovementUpdate->isActive()) {
        m_tNoMovementUpdate->stop();
    }
    delete m_tNoMovementUpdate;
    m_tNoMovementUpdate = nullptr;
}

void VectorFuncMove::setJoyXY(qint8 p_x, qint8 p_y)
{
    if( m_pendingJoyX != p_x || m_pendingJoyY !=  p_y || !m_joyInit) {
        m_joyInit = true;
        if(m_manualMode == true)
        {
           m_pendingCmd = MOVE_CMD_JOY_X_Y;
        }
        else
        {
           m_pendingCmd = MOVE_CMD_JOY_X_Y_AUTO;
        }
        m_pendingJoyX = p_x;
        m_pendingJoyY = p_y;        

        // compute new dir
        quint8 dir = MOVE_NO_DIRECTION;
        if( m_pendingJoyX > 0 ) {
            dir += MOVE_DIR_RIGHT;
        }
        if( m_pendingJoyX < 0 ) {
            dir += MOVE_DIR_LEFT;
        }
        if( m_pendingJoyY > 0 ) {
            dir += MOVE_DIR_UP;
        }
        if( m_pendingJoyY < 0 ) {
            dir += MOVE_DIR_DOWN;
        }
        m_moveDir = dir;
        emit moveDirChanged(dir);
        emit evLocalJoyXY(p_x, p_y);

// PATCH : Do not manage CMD ANSWER because of no answers sometimes from vector
//        if( !m_tCmdAnsTimeout->isActive() ) {
//            m_tCmdAnsTimeout->start(ProtMsg::TIMEOUT_MS);
        //processPendingCmd();
//        }

        /*
        if(m_pendingJoyX == 0 && m_pendingJoyY == 0) {
            m_tNoMovementUpdate->start();
        } else {
            if(m_tNoMovementUpdate->isActive()) {
                m_tNoMovementUpdate->stop();
            }
        }
        */
    }
}

void VectorFuncMove::setManualMode(bool ismanual)
{
   m_manualMode = ismanual;
   emit manualModeChanged(m_manualMode);
}

void VectorFuncMove::processPendingCmd()
{
   switch(m_pendingCmd)
   {
      case MOVE_CMD_JOY_X_Y:
      {
        // send request
        ProtMsg* pMsg = newMsgParam(ProtMsg::MSG_TYPE_CMD, ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y);
        pMsg->m_payload[0] = 127+(qint32)m_pendingJoyX;
        pMsg->m_payload[1] = 127+(qint32)m_pendingJoyY;
        pMsg->m_len = 2;
        sendMsg(pMsg);
        break;
      }
      case MOVE_CMD_JOY_X_Y_AUTO:
      {
         ProtMsg* pMsg = newMsgParam(ProtMsg::MSG_TYPE_CMD, ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y_ANGLE_CONTROL);
         pMsg->m_payload[0] = 127+(qint32)m_pendingJoyX;
         pMsg->m_payload[1] = 127+(qint32)m_pendingJoyY;
         pMsg->m_payload[2] = 1;
         qint16 angle = 90;
         pMsg->m_payload[3] = (angle << 8u) & 0xFF;
         pMsg->m_payload[4] = angle & 0xFF;
         pMsg->m_len = 5;
         sendMsg(pMsg);
         break;
      }
      default:
      break;
    }
    m_pendingCmd = MOVE_NO_CMD;
}

void VectorFuncMove::onCmdAnsTimeout()
{
    fLogError("Timeout");
    processPendingCmd();
}

void VectorFuncMove::onNoMovementUpdate()
{
    //if(m_pendingJoyX != 0 || m_pendingJoyY != 0) return;
    //m_pendingCmd = MOVE_CMD_JOY_X_Y;
   if(m_manualMode == true)
   {
      m_pendingCmd = MOVE_CMD_JOY_X_Y;
   }
   else
   {
      m_pendingCmd = MOVE_CMD_JOY_X_Y_AUTO;
   }
    processPendingCmd();
}

void VectorFuncMove::processReceivedMsg(ProtMsg *p_pMsg)
{
    if( ProtMsg::MSG_TYPE_CMD == p_pMsg->m_msgType ) {

        // -- MPC
        if( ProtMsg::FUNC_MOVE_CMD_C_ANS_JOY_X_Y == p_pMsg->m_msgObj ) {
            // recevied an ack !
            if( m_tCmdAnsTimeout->isActive() ) {
                m_tCmdAnsTimeout->stop();
            }
            // process pending cmd if needed
            // processPendingCmd(); // PATCH : Do not manage CMD ANSWER because of no answers sometimes from vector
        }

        // -- Device
        if( ProtMsg::FUNC_MOVE_CMD_C_REQ_JOY_X_Y == p_pMsg->m_msgObj ) {
            // Received a cmd !
            m_pendingJoyX = (qint32)p_pMsg->m_payload[0]-127;
            m_pendingJoyY = (qint32)p_pMsg->m_payload[1]-127;
            emit evCmdJoyXY(m_pendingJoyX, m_pendingJoyY);

            // Send a ack
            ProtMsg * pMsgAck = newMsgCmd(ProtMsg::FUNC_MOVE_CMD_C_ANS_JOY_X_Y);
            pMsgAck->m_payload[0] = m_pendingJoyX;
            pMsgAck->m_payload[1] = m_pendingJoyY;
            pMsgAck->m_len = 2;
            sendMsg(pMsgAck);
        }
    }
}

void VectorFuncMove::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
}

void VectorFuncMove::moveKeyLeft()
{
    m_moveKeyDir = (m_moveKeyDir & 0b1100) + MOVE_DIR_LEFT;
    moveKeyUpdate();
}

void VectorFuncMove::moveKeyRight()
{
    m_moveKeyDir = (m_moveKeyDir & 0b1100) + MOVE_DIR_RIGHT;
    moveKeyUpdate();
}

void VectorFuncMove::moveKeyUp()
{
    m_moveKeyDir = (m_moveKeyDir & 0b0011) + MOVE_DIR_UP;
    moveKeyUpdate();
}

void VectorFuncMove::moveKeyDown()
{
    m_moveKeyDir = (m_moveKeyDir & 0b0011) + MOVE_DIR_DOWN;
    moveKeyUpdate();
}

void VectorFuncMove::moveBoost(bool p_value)
{
    if( p_value != m_moveBoost ) {
        m_moveBoost = p_value;
        moveKeyUpdate();
    }
}

void VectorFuncMove::moveKeyUpState(bool state)
{
   if(state) {
      m_moveKeyDir |= MOVE_DIR_UP;
   } else {
      m_moveKeyDir &= ~MOVE_DIR_UP;
   }
   moveKeyUpdate();
}

void VectorFuncMove::moveKeyDownState(bool state)
{
   if(state) {
      m_moveKeyDir |= MOVE_DIR_DOWN;
   } else {
      m_moveKeyDir &= ~MOVE_DIR_DOWN;
   }
   moveKeyUpdate();
}

void VectorFuncMove::moveKeyLeftState(bool state)
{
   if(state) {
      m_moveKeyDir |= MOVE_DIR_LEFT;
   } else {
      m_moveKeyDir &= ~MOVE_DIR_LEFT;
   }
   moveKeyUpdate();
}

void VectorFuncMove::moveKeyRightState(bool state)
{
   if(state) {
      m_moveKeyDir |= MOVE_DIR_RIGHT;
   } else {
      m_moveKeyDir &= ~MOVE_DIR_RIGHT;
   }
   moveKeyUpdate();
}

void VectorFuncMove::moveKeyUpdate()
{
    qint8 vx = 0;
    qint8 vy = 0;
    qint8 speed = m_moveBoost ? 2 : 1;

    if( m_moveKeyDir & MOVE_DIR_LEFT ) {
        vx += -127*MOVE_KEY_FACTOR*speed;
    }
    if( m_moveKeyDir & MOVE_DIR_RIGHT ) {
        vx += 127*MOVE_KEY_FACTOR*speed;
    }

    if( m_moveKeyDir & MOVE_DIR_UP ) {
        vy += 127*MOVE_KEY_FACTOR*speed;
    }
    if( m_moveKeyDir & MOVE_DIR_DOWN ) {
        vy += -127*MOVE_KEY_FACTOR*speed;
    }

    vx = qBound(-127,vx*speed,127);
    vy = qBound(-127,vy*speed,127);

    setJoyXY(vx, vy);

}

void VectorFuncMove::stopMove()
{
    m_moveKeyDir = MOVE_NO_DIRECTION;
    moveKeyUpdate();
}


void VectorFuncMove::moveStopX()
{
    m_moveKeyDir = (m_moveKeyDir & 0b1100);
    moveKeyUpdate();
}

void VectorFuncMove::moveStopY()
{
    m_moveKeyDir = (m_moveKeyDir & 0b0011);
    moveKeyUpdate();
}

quint8 VectorFuncMove::getMoveDir() const
{
    return m_moveDir;
}

bool VectorFuncMove::getManualMode() const
{
    return m_manualMode;
}

// -- Inherited from XmlProject
bool VectorFuncMove::customLoadParam(QDomElement *)
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncMove::customLoadDefaultParam()
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncMove::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncMove::customSaveParam(QDomDocument *, QDomElement *)
{
    bool bRet = true;

    return bRet;
}


void VectorFuncMove::onDeviceConnected()
{
    if( m_tCmdAnsTimeout->isActive() ) {
        m_tCmdAnsTimeout->stop();
    }
}

