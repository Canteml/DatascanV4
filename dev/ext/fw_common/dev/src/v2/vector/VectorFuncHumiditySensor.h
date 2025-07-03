/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncHumiditySensor.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncHumiditySensor

/**
 * @brief The VectorFuncHumiditySensor class
 */
class VectorFuncHumiditySensor : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DHumidityC, quint16, 2, FUNC_HUMIDITY_SENSOR_PARAM_D_HUMIDITY_P, 0)   ///< Valeur du pourcentage d’humidité 16 bits non signés : 1 pour-cent = 10 (942 = 94.2%). Payload : 2 bytes

public :

    VectorFuncHumiditySensor(VectorStd *p_pVectorStd, quint32 p_instance); ///< p_instance in 0-2
    ~VectorFuncHumiditySensor();

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
