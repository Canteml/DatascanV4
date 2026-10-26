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

const QString VectorFuncPosition::BSCAN_SOURCE_ALL    = QString();
const QString VectorFuncPosition::BSCAN_SOURCE_SERIAL = QStringLiteral("SERIAL");

void VectorFuncPosition::processReceivedMsg(ProtMsg *p_pMsg)
{
    const bool isPositionValue =
            ( static_cast<quint16>(ProtMsg::FUNC_POSITION_PARAM_D_ODO_POS_MM_X) == p_pMsg->m_msgObj
              || static_cast<quint16>(ProtMsg::FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X) == p_pMsg->m_msgObj )
            && ( ProtMsg::MSG_TYPE_PARAM_SET == p_pMsg->m_msgType || ProtMsg::MSG_TYPE_PARAM_VALUE == p_pMsg->m_msgType );
    const bool isOdoMsg = static_cast<quint16>(ProtMsg::FUNC_POSITION_PARAM_D_ODO_POS_MM_X) == p_pMsg->m_msgObj;
    if( isPositionValue && !m_bscanSource.isEmpty() && m_bscanSource != m_pVectorStd->getRxLinkName() ) {
        // DEBUG : position sent by a link which is not the BScan source
        if( 0 == (m_ignoredPosMsgCount++ % 50) ) {
            const qreal value = isOdoMsg ? *(reinterpret_cast<qint32*>(p_pMsg->m_payload))
                                         : *(reinterpret_cast<qreal*>(p_pMsg->m_payload));
            fLogInfo(QString("[BSCAN POS] ignored : link '%1' (source '%2') %3 = %4 (count %5)")
                     .arg(m_pVectorStd->getRxLinkName(), m_bscanSource, QString(isOdoMsg ? "ODO" : "BSCAN_POS"))
                     .arg(value, 0, 'f', 2).arg(m_ignoredPosMsgCount));
        }
        return;
    }

    if( isPositionValue ) {
        m_posOrigin = QString("RX link '%1' %2 type %3")
                .arg(m_pVectorStd->getRxLinkName(), QString(isOdoMsg ? "ODO" : "BSCAN_POS")).arg(p_pMsg->m_msgType);
    }

    DECODE_MSG_PARAM(DOdoPosMmX, qint32, 4, FUNC_POSITION_PARAM_D_ODO_POS_MM_X)
    DECODE_MSG_PARAM(DOdoDiamMmX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X)
    DECODE_MSG_PARAM(DOdoResX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_RES_X)
    DECODE_MSG_PARAM(DOdoFactorX, qreal, 8, FUNC_POSITION_PARAM_D_ODO_FACTOR_X)
    DECODE_MSG_PARAM(DWheelSpacingMm, quint16, 2, FUNC_POSITION_PARAM_D_WHEEL_SPACING_MM)
    DECODE_MSG_PARAM(DMinTurningRadius, quint16, 2, FUNC_POSITION_PARAM_D_MIN_TURNING_RADIUS_MM)
    DECODE_MSG_PARAM(LBscanPosMmX, qreal, 8, FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X)
    DECODE_MSG_PARAM(LVectorDirX, quint8, 1, FUNC_POSITION_PARAM_L_VECTOR_DIR_X)
    m_posOrigin.clear();
}

void VectorFuncPosition::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    if( m_pVectorStd->isMpc() ) {
        m_posOrigin = "configureFunction (reset)";
        setParamLBscanPosMmX(0);    // reset position
        m_posOrigin.clear();
    }
}

void VectorFuncPosition::bscanPosMmXAddMm(qint32 p_value)
{
    if( 0 == m_paramLVectorDirX ) {
        m_refBscanPosMmX += p_value;
    } else {
        m_refBscanPosMmX -= p_value;
    }
    m_posOrigin = QString("UI bscanPosMmXAddMm(%1)").arg(p_value);
    computeBScanPosMmX();
    m_posOrigin.clear();
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
        m_posOrigin = QString("UI setParamLBscanPosMmXFromString('%1')").arg(p_value);
        setParamLBscanPosMmX(v*1000);
        m_posOrigin.clear();
    } else {
        fLogError("fail to convert string to double : " + p_value);
    }
}

void VectorFuncPosition::onDOdoPosMmXChanged(qint32 p_value)
{
    if( !m_refOdoDone ) {
        m_refOdoMmX = p_value;
        m_refBscanPosMmX = m_paramLBscanPosMmX;
        m_refOdoDone = true;
    }
    computeBScanPosMmX();
}

void VectorFuncPosition::onLBscanPosMmXChanged(qreal p_value)
{
    // DEBUG : trace every change of the BScan position with its origin
    const qreal delta = p_value - m_lastTracedPosMmX;
    const QString trace = QString("[BSCAN POS] %1 -> %2 mm (delta %3) | origin : %4 | odo %5 refOdo %6 refBscan %7 dir %8")
            .arg(m_lastTracedPosMmX, 0, 'f', 2).arg(p_value, 0, 'f', 2).arg(delta, 0, 'f', 2)
            .arg(m_posOrigin.isEmpty() ? QString("direct set (QML / other)") : m_posOrigin)
            .arg(m_paramDOdoPosMmX).arg(m_refOdoMmX, 0, 'f', 2).arg(m_refBscanPosMmX, 0, 'f', 2).arg(static_cast<int>(m_paramLVectorDirX));
    if( qAbs(delta) >= 5.0 ) {
        fLogWarning(trace + "  <<< JUMP");
    } else {
        fLogInfo(trace);
    }
    m_lastTracedPosMmX = p_value;

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

    m_posOrigin = "customApplyParam (XML)";
    emit paramDOdoPosMmXChanged(m_paramDOdoPosMmX);
    emit paramDOdoDiamMmXChanged(m_paramDOdoDiamMmX);
    emit paramDOdoResXChanged(m_paramDOdoResX);
    emit paramDOdoFactorXChanged(m_paramDOdoFactorX);
    emit paramDWheelSpacingMmChanged(m_paramDWheelSpacingMm);
    emit paramDMinTurningRadiusChanged(m_paramDMinTurningRadius);
    emit paramLBscanPosMmXChanged(m_paramLBscanPosMmX);
    emit paramLVectorDirXChanged(m_paramLVectorDirX);
    m_posOrigin.clear();

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
    qreal delta_odo =  m_paramDOdoPosMmX - m_refOdoMmX;
    qreal newBscanPos;
    if( 0 == m_paramLVectorDirX ) {
        newBscanPos = m_refBscanPosMmX + delta_odo;
    } else {
        newBscanPos = m_refBscanPosMmX - delta_odo;
    }
    setParamLBscanPosMmX(newBscanPos);
}

void VectorFuncPosition::setBScanSource(const QString &p_value)
{
    if( m_bscanSource != p_value ) {
        m_bscanSource = p_value;
        m_refOdoDone = false;
        fLogInfo(QString("BScan source : '%1'").arg(m_bscanSource.isEmpty() ? "ALL" : m_bscanSource));
    }
}

void VectorFuncPosition::injectEncoderPositionMm(qreal mm)
{
    if( !m_bscanSource.isEmpty() && BSCAN_SOURCE_SERIAL != m_bscanSource )
        return;
    if(mm == m_paramLBscanPosMmX)
        return;
    m_paramLBscanPosMmX = mm;
    m_posOrigin = "SERIAL encoder (injectEncoderPositionMm)";
    emit paramLBscanPosMmXChanged(mm);
    m_posOrigin.clear();
}
