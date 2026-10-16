/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPosition.cpp
# Author      : Thierry Lechevallier
# Date        : 08/07/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncPosition.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>
#include <v2/com/ComDeviceManager.h>
#include <v2/com/ComDevice.h>
#include <v2/com/GenComLink.h>

VectorFuncPosition::VectorFuncPosition(VectorStd *p_pVectorStd) : GenVectorFunc("VectorFuncPosition", p_pVectorStd, ProtMsg::DEVICE_FUNC_POSITION)
{
    qRegisterMetaType<VectorFuncPosition*>();
    connect(this, &VectorFuncPosition::paramDOdoPosMmXChanged, this, &VectorFuncPosition::onDOdoPosMmXChanged);
    connect(this, &VectorFuncPosition::paramLBscanPosMmXChanged, this, &VectorFuncPosition::onLBscanPosMmXChanged);
    m_refOdoDone = false;
    m_firstConnected = false;
}

VectorFuncPosition::~VectorFuncPosition()
{

}

void VectorFuncPosition::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DOdoPosMmX, qint32, 4, FUNC_POSITION_PARAM_D_ODO_POS_MM_X)
    DECODE_MSG_PARAM(DOdoDiamMmX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X)
    DECODE_MSG_PARAM(DOdoResX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_RES_X)
    DECODE_MSG_PARAM(DOdoFactorX, qreal, 8, FUNC_POSITION_PARAM_D_ODO_FACTOR_X)
    DECODE_MSG_PARAM(DWheelSpacingMm, quint16, 2, FUNC_POSITION_PARAM_D_WHEEL_SPACING_MM)
    DECODE_MSG_PARAM(DMinTurningRadius, quint16, 2, FUNC_POSITION_PARAM_D_MIN_TURNING_RADIUS_MM)
    DECODE_MSG_PARAM(LBscanPosMmX, qreal, 8, FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X)
    DECODE_MSG_PARAM(LVectorDirX, quint8, 1, FUNC_POSITION_PARAM_L_VECTOR_DIR_X)
}

void VectorFuncPosition::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    if( m_pVectorStd->isMpc() ) {
        setParamLBscanPosMmX(0);    // reset position
    }

    // For all products :
    // reset parameters :
//    setParamDOdoPosMmX(0);
//    setParamDOdoDiamMmX(0);
//    setParamDOdoResX(1024);
//    setParamDOdoFactorX(1);
//    setParamLBscanPosMmX(0);
//    setParamLVectorDirX(0);

//    emit paramDOdoPosMmXChanged(m_paramDOdoPosMmX);
//    emit paramDOdoDiamMmXChanged(m_paramDOdoDiamMmX);
//    emit paramDOdoResXChanged(m_paramDOdoResX);
//    emit paramDOdoFactorXChanged(m_paramDOdoFactorX);
//    emit paramLBscanPosMmXChanged(m_paramLBscanPosMmX);
//    emit paramLVectorDirXChanged(m_paramLVectorDirX);
}

void VectorFuncPosition::bscanPosMmXAddMm(qint32 p_value)
{
    if( 0 == m_paramLVectorDirX ) {
        m_refBscanPosMmX += p_value;
    } else {
        m_refBscanPosMmX -= p_value;
    }
    computeBScanPosMmX();
}

QString VectorFuncPosition::getBScanPosMeterWith2Decimals() const
{
    qreal p = m_paramLBscanPosMmX/1000.0;
    return QString("%1").arg(p, 0, 'F', 2, QChar('0'));
}

void VectorFuncPosition::setParamLBscanPosMmXFromString(const QString &p_value)
{
    bool bOk = true;
    qreal v = p_value.toDouble(&bOk);
    if( bOk ) {
        setParamLBscanPosMmX(v*1000);   ///< M convert to mm
    } else {
        fLogError("fail to convert string to double : " + p_value);
    }
}

void VectorFuncPosition::onDOdoPosMmXChanged(qint32 p_value)
{
    if( !m_refOdoDone ) {
        // initialize references
        m_refOdoMmX = p_value;
        m_refBscanPosMmX = m_paramLBscanPosMmX;
        m_refOdoDone = true;
    }
    computeBScanPosMmX();
}

void VectorFuncPosition::onLBscanPosMmXChanged(qreal p_value)
{
    // fLogDebug1(QString("Set New PosX : %1").arg(p_value));
    // for this giver position, set references :
    m_refOdoMmX = m_paramDOdoPosMmX;
    m_refBscanPosMmX = p_value;
}


// -- Inherited from XmlProject
bool VectorFuncPosition::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;

    bRet &= XML_READ_UNIQUE_DEFAULT(qint32, m_paramDOdoPosMmX, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDOdoDiamMmX, 56);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDOdoResX, 4096);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramDOdoFactorX, 1);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDWheelSpacingMm, 50);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDMinTurningRadius, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLBscanPosMmX, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLVectorDirX, 0);

    return bRet;
}

bool VectorFuncPosition::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDOdoPosMmX = 0;
    m_paramDOdoDiamMmX = 56;
    m_paramDOdoResX = 4096;
    m_paramDOdoFactorX = 1;
    m_paramDWheelSpacingMm = 50;
    m_paramDMinTurningRadius = 0;
    m_paramLBscanPosMmX = 0;
    m_paramLVectorDirX = 0;
    return bRet;
}

bool VectorFuncPosition::customApplyParam()
{
    bool bRet = true;

    emit paramDOdoPosMmXChanged(m_paramDOdoPosMmX);
    emit paramDOdoDiamMmXChanged(m_paramDOdoDiamMmX);
    emit paramDOdoResXChanged(m_paramDOdoResX);
    emit paramDOdoFactorXChanged(m_paramDOdoFactorX);
    emit paramDWheelSpacingMmChanged(m_paramDWheelSpacingMm);
    emit paramDMinTurningRadiusChanged(m_paramDMinTurningRadius);
    emit paramLBscanPosMmXChanged(m_paramLBscanPosMmX);
    emit paramLVectorDirXChanged(m_paramLVectorDirX);

    return bRet;
}

bool VectorFuncPosition::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDOdoPosMmX);
    bRet &= XML_WRITE_UNIQUE(m_paramDOdoDiamMmX);
    bRet &= XML_WRITE_UNIQUE(m_paramDOdoResX);
    bRet &= XML_WRITE_UNIQUE(m_paramDOdoFactorX);
    bRet &= XML_WRITE_UNIQUE(m_paramDWheelSpacingMm);
    bRet &= XML_WRITE_UNIQUE(m_paramDMinTurningRadius);
    bRet &= XML_WRITE_UNIQUE(m_paramLBscanPosMmX);
    bRet &= XML_WRITE_UNIQUE(m_paramLVectorDirX);
    return bRet;
}


void VectorFuncPosition::onDeviceConnected()
{
    // reset function
    m_refOdoDone = false || (m_firstConnected && m_refOdoDone);
    m_firstConnected = true;
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_ODO_POS_MM_X));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_ODO_RES_X));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_ODO_FACTOR_X));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_WHEEL_SPACING_MM));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_D_MIN_TURNING_RADIUS_MM));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X));
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_POSITION_PARAM_L_VECTOR_DIR_X));
    }
}

void VectorFuncPosition::computeBScanPosMmX()
{
    // compute the new BScan position :
    qreal delta_odo =  m_paramDOdoPosMmX - m_refOdoMmX;
    qreal newBscanPos;
    if( 0 == m_paramLVectorDirX ) {
        newBscanPos = m_refBscanPosMmX + delta_odo;
    } else {
        newBscanPos = m_refBscanPosMmX - delta_odo;
    }
    setParamLBscanPosMmX(newBscanPos);
}

void VectorFuncPosition::injectEncoderPositionMm(qreal mm)
{
    if(mm == m_paramLBscanPosMmX)
        return;
    m_paramLBscanPosMmX = mm;
    emit paramLBscanPosMmXChanged(mm);
}
