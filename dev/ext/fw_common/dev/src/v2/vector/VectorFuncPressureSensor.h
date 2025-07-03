/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPressureSensor.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncPressureSensor

/**
 * @brief The VectorFuncPressureSensor class
 */
class VectorFuncPressureSensor : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DPressureB, quint16, 2, FUNC_PRESSURE_SENSOR_PARAM_D_PRESSURE_B, 0)              ///< Valeur de la pression 16 bits non signés : 1 Bar = 1000 (exemple : 1.008Bar = 1008)
    FUNC_PARAM(LDepthValueM, qreal, 8, FUNC_PRESSURE_SENSOR_PARAM_L_DEPTH_VALUE_M, 0)
    FUNC_PARAM(LPressureOffset, quint16, 2, FUNC_PRESSURE_SENSOR_PARAM_L_PRESSURE_OFFSET, 0)

    Q_PROPERTY(QString depthStrValue READ getDepthStrValue NOTIFY depthStrValueChanged)
    Q_PROPERTY(QString depthType READ getDepthTypeStr NOTIFY depthTypeChanged)

public :

    VectorFuncPressureSensor(VectorStd *p_pVectorStd, quint32 p_instance); ///< p_instance in 0-2
    ~VectorFuncPressureSensor();

    // -- PARAM

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    Q_INVOKABLE void calibrateDepth0(); ///< use current pressure value as offset
    QString getDepthStrValue(void) const;
    QString getDepthTypeStr(void) const;

private slots :
    void onDPressureBChanged();
    void onLPressureOffset();
    void onLDepthValueMChanged();

signals :
    void depthStrValueChanged(QString);
    void depthTypeChanged(QString);

protected  :
    bool m_bIsWaterProduct = false;

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
    void computeDepth();
};
