/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncTemperatureSensor.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncTemperatureSensor.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncTemperatureSensor

VectorFuncTemperatureSensor::VectorFuncTemperatureSensor(VectorStd *p_pVectorStd, quint32 p_instance) : GenVectorFunc(QString("VectorFuncTemperatureSensor%1").arg(p_instance), p_pVectorStd, ProtMsg::DEVICE_FUNC_TEMPERATURE_SENSOR_0 + p_instance)
{

}

VectorFuncTemperatureSensor::~VectorFuncTemperatureSensor()
{

}

void VectorFuncTemperatureSensor::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DTemperatureC, qint16, 2, FUNC_TEMPERATURE_SENSOR_PARAM_D_TEMPERATURE_C)
}

void VectorFuncTemperatureSensor::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();
}

// -- Inherited from XmlProject
bool VectorFuncTemperatureSensor::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(qint16, m_paramDTemperatureC, 0);
    return bRet;
}

bool VectorFuncTemperatureSensor::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDTemperatureC = 0;
    return bRet;
}

bool VectorFuncTemperatureSensor::customApplyParam()
{
    bool bRet = true;
    emit paramDTemperatureCChanged(m_paramDTemperatureC);
    return bRet;
}

bool VectorFuncTemperatureSensor::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDTemperatureC);
    return bRet;
}


void VectorFuncTemperatureSensor::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_TEMPERATURE_SENSOR_PARAM_D_TEMPERATURE_C));
    }
}
