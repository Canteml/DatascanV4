/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPump.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncPump

/**
 * @brief The VectorFuncPump class
 */
class VectorFuncPump : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT    

    FUNC_PARAM(DPump, quint8, 1, FUNC_PUMP_PARAM_D_PUMP, 0)              ///< 0 to 100%, set device pump level
    FUNC_PARAM(DTankLevel, quint8, 1, FUNC_PUMP_PARAM_D_TANK_LVL, 0)     ///< 0 to 100%, indicates remaining water
    FUNC_PARAM(LPumpEnable, quint8, 1, FUNC_PUMP_PARAM_L_PUMP_ENABLE, 0)      ///< 0/1
    FUNC_PARAM(LPumpLevel, quint8, 1, FUNC_PUMP_PARAM_L_PUMP_LEVEL, 0)       ///< 0 to 100%, send to pump if L_PUMP_ENABLE = 1
public :

    VectorFuncPump(VectorStd *p_pVectorStd);
    ~VectorFuncPump();

    // -- PARAM

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

private slots :
    void onLPumpEnableChanged(quint8 p_value);
    void onLPumpLevelChanged(quint8 p_value);

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
    void setPumpWithLogical();

};
