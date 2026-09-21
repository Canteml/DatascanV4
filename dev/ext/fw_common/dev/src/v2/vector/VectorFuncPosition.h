/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : VectorFuncPosition.h
# Author      : Thierry Lechevallier
# Date        : 08/07/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <v2/vector/VectorFunc.h>
#include <v2/prot/ProtMsg.h>
class ComDevice;
class GenComLink;

// -- Qt includes
#include <qqml.h>

// ----------------------------------------------------------------------------
// -- VectorFuncPosition

/**
 * @brief The VectorFuncPosition class
 */
class VectorFuncPosition : public GenVectorFunc {
    Q_OBJECT

    QML_ELEMENT

    FUNC_PARAM(DOdoPosMmX, qint32, 4, FUNC_POSITION_PARAM_D_ODO_POS_MM_X, 0)
    FUNC_PARAM(DOdoDiamMmX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_DIAM_MM_X, 0)
    FUNC_PARAM(DOdoResX, quint16, 2, FUNC_POSITION_PARAM_D_ODO_RES_X, 1024)
    FUNC_PARAM(DOdoFactorX, qreal, 8, FUNC_POSITION_PARAM_D_ODO_FACTOR_X, 0)
    FUNC_PARAM(DWheelSpacingMm, quint16, 2, FUNC_POSITION_PARAM_D_WHEEL_SPACING_MM, 0)
    FUNC_PARAM(DMinTurningRadius, quint16, 2, FUNC_POSITION_PARAM_D_MIN_TURNING_RADIUS_MM, 0)
    FUNC_PARAM(LBscanPosMmX, qreal, 8, FUNC_POSITION_PARAM_L_BSCAN_POS_MM_X, 0)
    FUNC_PARAM(LVectorDirX, quint8, 1, FUNC_POSITION_PARAM_L_VECTOR_DIR_X, 0) ///< 0 : incrément, 1 = décrément from last m_refBscanPosMmX


public :

    VectorFuncPosition(VectorStd *p_pVectorStd);
    ~VectorFuncPosition();

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    // -- BscanPosMmX
    Q_INVOKABLE void bscanPosMmXAddMm(qint32 p_value);  ///< Offset added to m_refBscanPosMmX to compute m_paramLBscanPosMmX
    Q_INVOKABLE QString getBScanPosMeterWith2Decimals() const;
    Q_INVOKABLE void setParamLBscanPosMmXFromString(const QString& p_value);

private slots :
    void onDOdoPosMmXChanged(qint32 p_value);
    void onLBscanPosMmXChanged(qreal p_value);


protected :
    // -- Inherited from XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- inherited
    virtual void onDeviceConnected() override; ///< read device parameters when a device is just connected (called only for Ground Station)


private :
    void computeBScanPosMmX();


    // -- CONF
    quint8 m_confPosX;  ///< 0 : m_paramLBscanPosMmX computed from odometry

    // -- Behavior
    qreal m_refOdoMmX;      ///< reference of Odometry for current m_refPosX
    qreal m_refBscanPosMmX; ///< value of m_refBscanPosMmX when m_refOdoMmX was changed
    bool m_refOdoDone;      ///< if false, the next received m_paramDOdoPosMmX will be the ref of the current m_refBscanPosMmX
    bool m_firstConnected;

};

Q_DECLARE_OPAQUE_POINTER(VectorFuncPosition*)
#ifndef METATYPE_VECTORFUNCPRESSURESENSOR
#define METATYPE_VECTORFUNCPRESSURESENSOR
Q_DECLARE_METATYPE(VectorFuncPosition*)
#endif
