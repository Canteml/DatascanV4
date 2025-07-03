/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPressureSensor.cpp
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/vector/VectorFuncPressureSensor.h>
#include <v2/vector/VectorStd.h>
#include <v2/prot/ProtMsg.h>

// -- Qt includes
#include <QDateTime>

#define PRESSURE_TO_DEPTH_WATER_COEFF 1.01974
#define PRESSURE_TO_DEPTH_AIR_COEFF 832.13713

// ----------------------------------------------------------------------------
// -- VectorFuncPressureSensor

VectorFuncPressureSensor::VectorFuncPressureSensor(VectorStd *p_pVectorStd, quint32 p_instance) : GenVectorFunc(QString("VectorFuncPressureSensor%1").arg(p_instance), p_pVectorStd, ProtMsg::DEVICE_FUNC_PRESSURE_SENSOR_0 + p_instance)
{
    connect(this, &VectorFuncPressureSensor::paramDPressureBChanged, this, &VectorFuncPressureSensor::onDPressureBChanged, Qt::DirectConnection);
    connect(this, &VectorFuncPressureSensor::paramLPressureOffsetChanged, this, &VectorFuncPressureSensor::onLPressureOffset, Qt::DirectConnection);
    connect(this, &VectorFuncPressureSensor::paramLDepthValueMChanged, this, &VectorFuncPressureSensor::onLDepthValueMChanged, Qt::DirectConnection);
}

VectorFuncPressureSensor::~VectorFuncPressureSensor()
{

}

void VectorFuncPressureSensor::processReceivedMsg(ProtMsg *p_pMsg)
{
    DECODE_MSG_PARAM(DPressureB, quint16, 2, FUNC_PRESSURE_SENSOR_PARAM_D_PRESSURE_B)
    DECODE_MSG_PARAM(LDepthValueM, qreal, 8, FUNC_PRESSURE_SENSOR_PARAM_L_DEPTH_VALUE_M)
    DECODE_MSG_PARAM(LPressureOffset, quint16, 2, FUNC_PRESSURE_SENSOR_PARAM_L_PRESSURE_OFFSET)
}

void VectorFuncPressureSensor::configureFunction(quint32 p_prodId)
{
    Q_UNUSED(p_prodId)

    m_bIsWaterProduct = (ProtMsg::PROD_ROVER_IP68 == p_prodId); // Check if product is a water product
    emit depthTypeChanged(getDepthTypeStr());

    //setParamDPressureB(0);        /// use saved value
    setParamLDepthValueM(0);
    //setParamLPressureOffset(0);   /// use saved value

    onDeviceConnected();
}

void VectorFuncPressureSensor::calibrateDepth0()
{
    // use current pressure value as offset
    setParamLPressureOffset(m_paramDPressureB);
}

QString VectorFuncPressureSensor::getDepthStrValue() const
{
    QString sRet = "0.00 cm";
    if( 0 == m_paramLPressureOffset ) {
        sRet = QString("%1 bar").arg(QString::number(m_paramDPressureB / 1000, 'f', 3));
    } else {
        if( abs(m_paramLDepthValueM)<100 )  {
            sRet = QString("%1 cm").arg(QString::number(m_paramLDepthValueM, 'f', 0));
        } else {
            sRet = QString("%1 m").arg(QString::number(m_paramLDepthValueM/100, 'f', 2));
        }
    }
    return sRet;
}

QString VectorFuncPressureSensor::getDepthTypeStr() const
{
    if(m_bIsWaterProduct) {
        return tr("Profondeur");
    }
    return tr("Altitude");
}

void VectorFuncPressureSensor::onDPressureBChanged()
{
    computeDepth();
}

void VectorFuncPressureSensor::onLPressureOffset()
{
    computeDepth();
}

void VectorFuncPressureSensor::onLDepthValueMChanged()
{
    emit depthStrValueChanged(getDepthStrValue());
}


// -- Inherited from XmlProject
bool VectorFuncPressureSensor::customLoadParam(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramDPressureB, 0);
    bRet &= XML_READ_UNIQUE_DEFAULT(qreal, m_paramLDepthValueM, 0.0);
    bRet &= XML_READ_UNIQUE_DEFAULT(quint16, m_paramLPressureOffset, 0);
    return bRet;
}

bool VectorFuncPressureSensor::customLoadDefaultParam()
{
    bool bRet = true;
    m_paramDPressureB = 0;
    m_paramLDepthValueM = 0;
    m_paramLPressureOffset = 0;
    return bRet;
}

bool VectorFuncPressureSensor::customApplyParam()
{
    bool bRet = true;
    emit paramDPressureBChanged(m_paramDPressureB);
    emit paramLDepthValueMChanged(m_paramLDepthValueM);
    emit paramLPressureOffsetChanged(m_paramLPressureOffset);
    return bRet;
}

bool VectorFuncPressureSensor::customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_WRITE_UNIQUE(m_paramDPressureB);
    bRet &= XML_WRITE_UNIQUE(m_paramLDepthValueM);
    bRet &= XML_WRITE_UNIQUE(m_paramLPressureOffset);
    return bRet;
}


void VectorFuncPressureSensor::onDeviceConnected()
{

}

void VectorFuncPressureSensor::computeDepth()
{
    qreal rCoefficient = PRESSURE_TO_DEPTH_AIR_COEFF;
    if(m_bIsWaterProduct) {
        rCoefficient = PRESSURE_TO_DEPTH_WATER_COEFF;
    }
    qreal d = (qreal(m_paramDPressureB)-qreal(m_paramLPressureOffset))*rCoefficient;
    setParamLDepthValueM(d);
}

