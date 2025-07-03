/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncInertialSensor.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncInertialSensor.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncInertialSensor

VectorFuncInertialSensor::VectorFuncInertialSensor(VectorStd *p_pVectorStd) :
    GenVectorFunc("VectorFuncInertialSensor", p_pVectorStd, ProtMsg::DEVICE_FUNC_INERTIAL_SENSOR)
{
    connect(this, &VectorFuncInertialSensor::paramDInertialSensorChanged, this, &VectorFuncInertialSensor::refreshDirection);
    connect(this, &VectorFuncInertialSensor::paramLHorizontalChanged, this, &VectorFuncInertialSensor::refreshDirection);
}

VectorFuncInertialSensor::~VectorFuncInertialSensor()
{

}

void VectorFuncInertialSensor::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DInertialSensor, InertialSensorXYZ, 6, FUNC_INERTIAL_SENSOR_PARAM_D_ACC_X_Y_Z)
    DECODE_MSG_PARAM(LDirection, qint16, 2, FUNC_INERTIAL_SENSOR_PARAM_L_DIRECTION)
    DECODE_MSG_PARAM(LHorizontal, quint8, 1, FUNC_INERTIAL_SENSOR_PARAM_L_HORIZONTAL)
}

void VectorFuncInertialSensor::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();

    // For all products :
    // reset parameters :
    setParamDInertialSensor(InertialSensorXYZ(0,0,0));
    setParamLDirection(0);
}

void VectorFuncInertialSensor::refreshDirection()
{
    qreal angle;
    if(1 == m_paramLHorizontal) {
        angle = -m_paramDInertialSensor.getXYAngle();
    } else {
        angle = m_paramDInertialSensor.getXZAngle();
    }
    setParamLDirection(angle);
}

// -- Inherited from XmlProject
bool VectorFuncInertialSensor::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    qint16 x,y,z;
    x = m_paramDInertialSensor.m_x;
    y = m_paramDInertialSensor.m_y;
    z = m_paramDInertialSensor.m_z;
    bRet &= XML_READ_UNIQUE_DEFAULT(qint16, x, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qint16, y, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qint16, z, 0);
    m_paramDInertialSensor.m_x = x;
    m_paramDInertialSensor.m_y = y;
    m_paramDInertialSensor.m_z = z;
    bRet &= XML_READ_UNIQUE_DEFAULT(qint16, m_paramLDirection, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint8, m_paramLHorizontal, 1);
    return bRet;
}

bool VectorFuncInertialSensor::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDInertialSensor = InertialSensorXYZ(0,0,0);
    m_paramLDirection = 0;
    m_paramLHorizontal = 1;
    return bRet;
}

bool VectorFuncInertialSensor::customApplyParam()
{
    bool bRet = true;    
    emit paramDInertialSensorChanged(m_paramDInertialSensor);
    emit paramLDirectionChanged(m_paramLDirection);
    emit paramLHorizontalChanged(m_paramLHorizontal);
    return bRet;
}

bool VectorFuncInertialSensor::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    qint16 x,y,z;
    x = m_paramDInertialSensor.m_x;
    y = m_paramDInertialSensor.m_y;
    z = m_paramDInertialSensor.m_z;
    bRet &= XML_WRITE_UNIQUE(x);
    bRet &= XML_WRITE_UNIQUE(y);
    bRet &= XML_WRITE_UNIQUE(z);
    bRet &= XML_WRITE_UNIQUE(m_paramLHorizontal);
    bRet &= XML_WRITE_UNIQUE(m_paramLDirection);
    return bRet;
}


void VectorFuncInertialSensor::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_INERTIAL_SENSOR_PARAM_D_ACC_X_Y_Z));
    }
}
