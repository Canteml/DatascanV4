/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncAScan.cpp
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncAScan.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComDevice.h>
#include <v2/com/GenComLink.h>
#include <v2/scan/AScanWorker.h>
#include <v2/vector/VectorFuncPosition.h>


// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncAScan

VectorFuncAScan::VectorFuncAScan(VectorStd *p_pVectorStd, quint32 p_instance) : GenVectorFunc(QString("VectorFuncAScan%1").arg(p_instance), p_pVectorStd, ProtMsg::DEVICE_FUNC_ASCAN_0 + p_instance)
{    
    m_pAScanWorker = new AScanWorker();
    m_ascan.m_source = p_instance;
    m_clearPipe = false;
    connect(m_pAScanWorker, &AScanWorker::evAScanTreated, this, &VectorFuncAScan::onAScanTreated, Qt::QueuedConnection);
    connect(this, &VectorFuncAScan::evPrivateAScanTreated, this, &VectorFuncAScan::onAScanTreated, Qt::QueuedConnection);

    m_AScanRxPart = 0;
    m_pAScanRx = nullptr;
}

VectorFuncAScan::~VectorFuncAScan()
{
    clearAScan();
}

void VectorFuncAScan::sendAScanRawSample(AScanRawSample *p_pAScan)
{
    // send msg to device
    ProtMsg * pMsg = newMsgEvent(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE);
    pMsg->m_len = AScanRawSample::ASCAN_SAMPLE_SIZE;
    memcpy(pMsg->m_payload, p_pAScan->m_rawSamples, pMsg->m_len);
    delete p_pAScan;
    sendMsg(pMsg);
}

//bool VectorFuncAScan::isPendingAScan() const
//{
//    bool bRet = false;
//    m_mAScanToTreat.lock();
//    bRet = m_lAScanToTreat.size();
//    m_mAScanToTreat.unlock();
//    return bRet;
//}

//AScan* VectorFuncAScan::takeAScan(void)
//{
//    AScan * pRet = nullptr;
//    m_mAScanToTreat.lock();
//    if( m_lAScanToTreat.size() ) {
//        pRet = m_lAScanToTreat.takeFirst(); // take oldest ascan (first added)
//    }
//    m_mAScanToTreat.unlock();
//    return pRet;
//}

void VectorFuncAScan::processReceivedMsg(ProtMsg *p_pMsg)
{
    if( ProtMsg::MSG_TYPE_PARAM_GET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_SET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType) {
        // -- PARAM
        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DGain, quint8, 1, FUNC_ASCAN_PARAM_D_GAIN);
        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DVoltage, quint8, 1, FUNC_ASCAN_PARAM_D_VOLTAGE);
        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DUtElement, quint8, 1, FUNC_ASCAN_PARAM_D_UT_ELEMENT);
        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DSampleFreq, quint8, 1, FUNC_ASCAN_PARAM_D_SAMPLE_FREQ);
        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, DPulseWidth, quint16, 2, FUNC_ASCAN_PARAM_D_PULSE_WIDTH);

        DECODE_MSG_PARAM_PRE(m_ascan.getSensorSettings()->m_param, LCelerity, quint32, 4, FUNC_ASCAN_PARAM_L_CELERITY);
        DECODE_MSG_PARAM_PRE(m_ascan.getAScanMeasure()->m_param, LProbeDelayMm, qreal, 8, FUNC_ASCAN_PARAM_L_PROBE_DELAY);
    } else if(ProtMsg::MSG_TYPE_EVENT == p_pMsg->m_msgType) {
        if( static_cast<quint16>(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE) == p_pMsg->m_msgObj ) {
            if( AScanRawSample::ASCAN_SAMPLE_SIZE == p_pMsg->m_len ) {
                AScan* pAScan = new AScan();
                // -- Copy params of actual ASCAN
                pAScan->copyParam(m_ascan);
                // copy sample
                memcpy(pAScan->getAScanRawSample()->m_rawSamples, p_pMsg->m_payload, p_pMsg->m_len);
                pAScan->getAScanRawSample()->m_epocTimeMs = QDateTime::currentMSecsSinceEpoch();
                pAScan->getAScanRawSample()->m_position.setX(m_pVectorStd->funcPosition()->getParamLBscanPosMmX());

                // -- complet raw data
                pAScan->getAScanRawSample()->m_epocTimeMs = QDateTime::currentMSecsSinceEpoch();
                pAScan->getAScanRawSample()->m_position = QPointF(m_pVectorStd->funcPosition()->getParamLBscanPosMmX(),0);

                // -- send the new sample to the Worker
                m_pAScanWorker->pushAScanToTreat(pAScan);
            }
        } else if( static_cast<quint16>(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE_PART_1_4) == p_pMsg->m_msgObj ) {
            if( 1000 == p_pMsg->m_len ) {
                if( 0 != m_AScanRxPart ) {
                    fLogError(QString("Receiving ASCAN PART 1/4 but current part is %1").arg(m_AScanRxPart));
                    resetAScanPart();
                } else {
                    if( nullptr != m_pAScanRx ) {
                        fLogError(QString("m_pAScanRx should be null at part 0 (current part is %1)").arg(m_AScanRxPart));
                    } else {
                        m_pAScanRx = new AScan();
                        // -- Copy params of actual ASCAN
                        m_pAScanRx->copyParam(m_ascan);
                        // copy sample
                        memcpy(m_pAScanRx->getAScanRawSample()->m_rawSamples, p_pMsg->m_payload, p_pMsg->m_len);
                        m_pAScanRx->getAScanRawSample()->m_epocTimeMs = QDateTime::currentMSecsSinceEpoch();
                        m_pAScanRx->getAScanRawSample()->m_position.setX(m_pVectorStd->funcPosition()->getParamLBscanPosMmX());

                        // -- complet raw data
                        m_pAScanRx->getAScanRawSample()->m_epocTimeMs = QDateTime::currentMSecsSinceEpoch();
                        m_pAScanRx->getAScanRawSample()->m_position = QPointF(m_pVectorStd->funcPosition()->getParamLBscanPosMmX(),0);

                        // -- Continue
                        m_AScanRxPart = 1;
                    }
                }
            }
        } else if( (static_cast<quint16>(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE_PART_2_4) == p_pMsg->m_msgObj ) ) {
            if( 1000 == p_pMsg->m_len ) {
                if( 1 != m_AScanRxPart ) {
                    fLogError(QString("Receiving ASCAN PART 2/4 but current part is %1").arg(m_AScanRxPart));
                    resetAScanPart();
                } else {
                    // Copy sample
                    memcpy(&m_pAScanRx->getAScanRawSample()->m_rawSamples[1000], p_pMsg->m_payload, p_pMsg->m_len);
                    // Continue
                    m_AScanRxPart = 2;
                }
            }
        } else if( (static_cast<quint16>(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE_PART_3_4) == p_pMsg->m_msgObj ) ) {
            if( 1000 == p_pMsg->m_len ) {
                if( 2 != m_AScanRxPart ) {
                    fLogError(QString("Receiving ASCAN PART 3/4 but current part is %1").arg(m_AScanRxPart));
                    resetAScanPart();
                } else {
                    // copy sample
                    memcpy(&m_pAScanRx->getAScanRawSample()->m_rawSamples[2000], p_pMsg->m_payload, p_pMsg->m_len);
                    // Continue
                    m_AScanRxPart = 3;
                }
            }
        } else if( (static_cast<quint16>(ProtMsg::FUNC_ASCAN_EVENT_D_SAMPLE_PART_4_4) == p_pMsg->m_msgObj ) ) {
            if( 1000 == p_pMsg->m_len ) {
                if( 3 != m_AScanRxPart ) {
                    fLogError(QString("Receiving ASCAN PART 3/4 but current part is %1").arg(m_AScanRxPart));
                    resetAScanPart();
                } else {
                    // copy sample
                    memcpy(&m_pAScanRx->getAScanRawSample()->m_rawSamples[3000], p_pMsg->m_payload, p_pMsg->m_len);
                    // -- send the new sample to the Worker
                    m_pAScanWorker->pushAScanToTreat(m_pAScanRx);
                    // Continue
                    m_AScanRxPart = 0;
                    m_pAScanRx = nullptr;
                }
            }
        }
    }
}

void VectorFuncAScan::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();
}

void VectorFuncAScan::clearAScan()
{
    m_clearPipe = true;
    disconnect(m_pAScanWorker, &AScanWorker::evAScanTreated, this, &VectorFuncAScan::onAScanTreated);
    m_pAScanWorker->clearAScan();
    resetAScanPart();
    connect(m_pAScanWorker, &AScanWorker::evAScanTreated, this, &VectorFuncAScan::onAScanTreated, Qt::QueuedConnection);
}

void VectorFuncAScan::resetAScanPart()
{
    m_AScanRxPart = 0;
    if( nullptr != m_pAScanRx ) {
        delete m_pAScanRx;
        m_pAScanRx = nullptr;
    }
}

// -- Inherited from XmlProject
bool VectorFuncAScan::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_ascan.getSensorSettings()->loadParam(p_pParent);
    bRet &= m_ascan.getAScanEnhancedSample()->loadParam(p_pParent);
    bRet &= m_ascan.getAScanMeasure()->loadParam(p_pParent);
    return bRet;
}

bool VectorFuncAScan::customLoadDefaultParam()
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncAScan::customApplyParam()
{
    bool bRet = true;

    return bRet;
}

bool VectorFuncAScan::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= m_ascan.getSensorSettings()->saveParam(p_pDoc, p_pParent);
    bRet &= m_ascan.getAScanEnhancedSample()->saveParam(p_pDoc, p_pParent);
    bRet &= m_ascan.getAScanMeasure()->saveParam(p_pDoc, p_pParent);
    return bRet;
}


void VectorFuncAScan::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_ASCAN_PARAM_D_GAIN));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_ASCAN_PARAM_D_VOLTAGE));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_ASCAN_PARAM_D_UT_ELEMENT));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_ASCAN_PARAM_D_SAMPLE_FREQ));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_ASCAN_PARAM_D_PULSE_WIDTH));
    }
    resetAScanPart();
}

void VectorFuncAScan::onAScanTreated()
{
    if( m_clearPipe ) {
        m_clearPipe = false;
    } else {
        AScan* pAScan = m_pAScanWorker->takeAScanTreated();
        if( nullptr != pAScan ) {
            emit evNewAScanTreated(pAScan);

            if( m_pAScanWorker->hasAscanTreated() ) {
                emit evPrivateAScanTreated();
            }
        }
    }
}
