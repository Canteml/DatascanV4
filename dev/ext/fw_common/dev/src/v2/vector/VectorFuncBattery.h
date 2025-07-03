/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncBattery.h
# Author      : Thierry Lechevallier
# Date        : 02/11/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>

// ----------------------------------------------------------------------------
// -- VectorFuncBattery

/**
 * @brief The VectorFuncBattery class
 *
 * Manage General I/O. The class VectorFuncBatteryAyth manage data for authentifications
 *
 */
class VectorFuncBattery : public GenVectorFunc {
    Q_OBJECT
    QML_ELEMENT    

    FUNC_PARAM(DBatteryLvlP, quint8, 1, FUNC_BATTERY_PARAM_D_BATTERY_LVL_P, 0)              ///< 1 byte : percent of battery : 0 - 100%. 255 = no battery

public :

    VectorFuncBattery(VectorStd *p_pVectorStd);
    ~VectorFuncBattery();

    // -- PARAM
    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    static const quint8 NO_BATTETY = 255;

protected  :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- Behaviour
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)
};
