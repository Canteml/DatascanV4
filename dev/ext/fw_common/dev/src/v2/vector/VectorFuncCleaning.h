/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncCleaning.h
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

#pragma once

#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncCleaning

/**
 * @brief The VectorFuncCleaning class
 */
class VectorFuncCleaning : public GenVectorFunc
{
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DBrush, quint8, 1, FUNC_CLEANING_PARAM_D_CLEANING_BRUSH, 0)
    FUNC_PARAM(DPump, quint8, 1, FUNC_CLEANING_PARAM_D_CLEANING_PUMP, 0)
    FUNC_PARAM(DTankLevel, quint8, 1, FUNC_CLEANING_PARAM_D_TANK_LVL, 0)
    FUNC_PARAM(LBrushEnable, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_ENABLE, 0)
    FUNC_PARAM(LBrushLevel, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_BRUSH_LEVEL, 0)
    FUNC_PARAM(LPumpEnable, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_PUMP_ENABLE, 0)
    FUNC_PARAM(LPumpLevel, quint8, 1, FUNC_CLEANING_PARAM_L_CLEANING_PUMP_LEVEL, 0)

public:

    VectorFuncCleaning(VectorStd *p_pVectorStd);
    ~VectorFuncCleaning();

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

private slots:

    void onLBrushEnableChanged(quint8 p_value);
    void onLBrushLevelChanged(quint8 p_value);
    void onLPumpEnableChanged(quint8 p_value);
    void onLPumpLevelChanged(quint8 p_value);

protected:

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
    void setBrushWithLogical();
    void setPumpWithLogical();

};

