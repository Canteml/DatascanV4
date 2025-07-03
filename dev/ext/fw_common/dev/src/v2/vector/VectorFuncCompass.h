/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncCompass.h
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncCompass

/**
 * @brief The VectorFuncCompass class
 */
class VectorFuncCompass : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(DCompassDegr, quint16, 2, FUNC_COMPASS_PARAM_D_COMPASS_DEGR, 0)

public :

    VectorFuncCompass(VectorStd *p_pVectorStd);
    ~VectorFuncCompass();

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
