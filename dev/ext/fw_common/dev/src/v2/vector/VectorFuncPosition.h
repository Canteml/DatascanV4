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
    FUNC_PARAM(LVectorDirX, quint8, 1, FUNC_POSITION_PARAM_L_VECTOR_DIR_X, 0)


public :

    VectorFuncPosition(VectorStd *p_pVectorStd);
    ~VectorFuncPosition();

    virtual void processReceivedMsg(ProtMsg* p_pMsg) override;
    virtual void configureFunction(quint32 p_prodId) override;

    // -- BscanPosMmX
    Q_INVOKABLE void bscanPosMmXAddMm(qint32 p_value);
    Q_INVOKABLE QString getBScanPosMeterWith2Decimals() const;
    Q_INVOKABLE void setParamLBscanPosMmXFromString(const QString& p_value);

    void injectEncoderPositionMm(qreal mm);

    static const QString BSCAN_SOURCE_ALL;
    static const QString BSCAN_SOURCE_SERIAL;
    void setBScanSource(const QString& p_value);
    QString getBScanSource() const { return m_bscanSource; }

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
    virtual void onDeviceConnected() override;


private :
    void computeBScanPosMmX();


    // -- CONF
    quint8 m_confPosX;

    // -- Behavior
    qreal m_refOdoMmX;
    qreal m_refBscanPosMmX;
    bool m_refOdoDone;
    bool m_firstConnected;
    QString m_bscanSource;

    // -- DEBUG : trace of the BScan position origin
    QString m_posOrigin;            // origin of the update being processed (empty = direct set, QML...)
    qreal m_lastTracedPosMmX = 0;
    quint32 m_ignoredPosMsgCount = 0;

};

Q_DECLARE_OPAQUE_POINTER(VectorFuncPosition*)
#ifndef METATYPE_VECTORFUNCPRESSURESENSOR
#define METATYPE_VECTORFUNCPRESSURESENSOR
Q_DECLARE_METATYPE(VectorFuncPosition*)
#endif
