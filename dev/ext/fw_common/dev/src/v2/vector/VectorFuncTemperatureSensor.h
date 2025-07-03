/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncTemperatureSensor.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncTemperatureSensor

/**
 * @brief The VectorFuncTemperatureSensor class
 */
class VectorFuncTemperatureSensor : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DTemperatureC, qint16, 2, FUNC_TEMPERATURE_SENSOR_PARAM_D_TEMPERATURE_C, 0)   ///< Valeur de la température 16 bits signés : 1 degré = 10 (exemple : 165 = 16.5 degrés). Payload : 2 bytes

public :

    VectorFuncTemperatureSensor(VectorStd *p_pVectorStd, quint32 p_instance); ///< p_instance in 0-2
    ~VectorFuncTemperatureSensor();

    // -- PARAM

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

protected  :

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
};
