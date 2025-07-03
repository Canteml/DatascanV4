/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : VectorLinear.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/vector/Vector.h>

// -- Qt includes


// ----------------------------------------------------------------------------
// -- Class VectorLinearParam

/**
 * @brief the VectorLinearParam class
 *
 */
class VectorLinearParam : public VectorParam
{
public :
    explicit VectorLinearParam();
    explicit VectorLinearParam(const VectorLinearParam & p_value);   ///< copy constructor
    virtual ~VectorLinearParam();

    virtual VectorParam* copy() const override;

    quint32 getCoderResolution(void) const;
    void setCoderResolution(quint32 p_value);
    qreal getWheelDiameter(void) const;
    void setWheelDiameter(qreal p_value);

    virtual quint32 getVectorType(void) const override;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    VectorLinearParam & operator= (const VectorLinearParam & p_value);
    bool operator== (const VectorLinearParam & p_value);



protected :

    // -- Param
    quint32 m_coderResolution;
    qreal m_wheelDiameter;
};

// ----------------------------------------------------------------------------
// -- Class VectorLinearCalibrate

// \todo

class VectorLinearCalibrate : public VectorCalibrate
{
public :
    explicit VectorLinearCalibrate();
    virtual ~VectorLinearCalibrate();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- Class VectorLinear

class VectorLinear : public Vector
{
public :
    explicit VectorLinear();
    explicit VectorLinear(VectorLinearParam * m_pVectorParam, VectorLinearCalibrate * m_pVectorCalibrate);
    virtual ~VectorLinear();

    VectorLinearParam* getParamLinear();
    VectorLinearCalibrate* getCalibrateLinear();        

    // -- Parameters

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private :

    VectorLinearParam p_vectorLinearParam;
    VectorLinearCalibrate p_vectorLinearCalibrate;
};

