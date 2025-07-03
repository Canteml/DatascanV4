/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncEndStop.h
# Author      : Tanguy Dubois
# Date        : 16/08/2022
#
# ==========================================================================*/

#pragma once

#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncEndStop

/**
 * @brief The VectorFuncEndStop class
 */
class VectorFuncEndStop : public GenVectorFunc
{
    Q_OBJECT
    QML_ELEMENT

    FUNC_PARAM(LCollision, quint8, 1, FUNC_ENDSTOP_PARAM_L_COLLISION, 0)

public:

    VectorFuncEndStop(VectorStd *p_pVectorStd);
    ~VectorFuncEndStop();

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

signals:


protected:

    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override;

};

