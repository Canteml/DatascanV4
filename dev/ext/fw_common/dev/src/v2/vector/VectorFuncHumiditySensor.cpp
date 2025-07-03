/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncHumiditySensor.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncHumiditySensor.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- VectorFuncHumiditySensor

VectorFuncHumiditySensor::VectorFuncHumiditySensor(VectorStd *p_pVectorStd, quint32 p_instance) : GenVectorFunc(QString("VectorFuncHumiditySensor%1").arg(p_instance), p_pVectorStd, ProtMsg::DEVICE_FUNC_HUMIDITY_SENSOR_0 + p_instance)
{

}

VectorFuncHumiditySensor::~VectorFuncHumiditySensor()
{

}

void VectorFuncHumiditySensor::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DHumidityC, quint16, 2, FUNC_HUMIDITY_SENSOR_PARAM_D_HUMIDITY_P)
}

void VectorFuncHumiditySensor::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)
    onDeviceConnected();
}

// -- Inherited from XmlProject
bool VectorFuncHumiditySensor::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDHumidityC, 0);
    return bRet;
}

bool VectorFuncHumiditySensor::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDHumidityC = 0;
    return bRet;
}

bool VectorFuncHumiditySensor::customApplyParam()
{
    bool bRet = true;
    emit paramDHumidityCChanged(m_paramDHumidityC);
    return bRet;
}

bool VectorFuncHumiditySensor::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDHumidityC);
    return bRet;
}


void VectorFuncHumiditySensor::onDeviceConnected()
{
    // request Params
    if( m_pVectorStd->isMpc() ) {
        sendMsg(newMsgParam(ProtMsg::MSG_TYPE_PARAM_GET, ProtMsg::FUNC_HUMIDITY_SENSOR_PARAM_D_HUMIDITY_P));
    }
}
