/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncLight.h
# Author      : Thierry Lechevallier
# Date        : 14/10/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncLight

/**
 * @brief The VectorFuncLight class
 */
class VectorFuncLight : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT    

    FUNC_PARAM(DLight, quint8, 1, FUNC_LIGHT_PARAM_D_LIGHT, 0)              ///< 0 to 100%, set device pump level
    FUNC_PARAM(LLightEnable, quint8, 1, FUNC_LIGHT_PARAM_L_LIGHT_ENABLE, 0)      ///< 0/1
    FUNC_PARAM(LLightLevel, quint8, 1, FUNC_LIGHT_PARAM_L_LIGHT_LEVEL, 0)       ///< 0 to 100%, send to pump if L_LIGHT_ENABLE = 1
public :

    VectorFuncLight(VectorStd *p_pVectorStd);
    ~VectorFuncLight();

    // -- PARAM

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

private slots :
    void onLLightEnableChanged(quint8 p_value);
    void onLLightLevelChanged(quint8 p_value);

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
    void setLightWithLogical();
};

Q_DECLARE_OPAQUE_POINTER(VectorFuncLight*)
#ifndef METATYPE_VECTORFUNCPRESSURESENSOR
#define METATYPE_VECTORFUNCPRESSURESENSOR
Q_DECLARE_METATYPE(VectorFuncLight*)
#endif
