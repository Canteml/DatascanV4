/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : VectorAltiscan.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/vector/VectorLinear.h>

// -- Qt includes


// ----------------------------------------------------------------------------
// -- Class VectorAltiscanParam

/**
 * @brief the VectorAltiscanParam class
 *
 */
class VectorAltiscanParam : public VectorLinearParam
{
public :
    explicit VectorAltiscanParam();
    explicit VectorAltiscanParam(const VectorAltiscanParam & p_value);   ///< copy constructor
    virtual ~VectorAltiscanParam();

    virtual VectorParam* copy() const override;

    quint32 getMotorReduction(void) const;
    void setMotorReduction(quint32 p_value);

    virtual quint32 getVectorType(void) const override;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    VectorAltiscanParam & operator= (const VectorAltiscanParam & p_value);
    bool operator== (const VectorAltiscanParam & p_value);

protected :

    // -- Param
    quint32 m_motorReduction;
};

// ----------------------------------------------------------------------------
// -- Class VectorAltiscanCalibrate

// \todo

class VectorAltiscanCalibrate : public VectorLinearCalibrate
{
public :
    explicit VectorAltiscanCalibrate();
    virtual ~VectorAltiscanCalibrate();

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
};

// ----------------------------------------------------------------------------
// -- Class VectorAltiscan

class VectorAltiscan: public VectorLinear
{
public :
    explicit VectorAltiscan();
    virtual ~VectorAltiscan();

    VectorAltiscanParam* getParamAltiscan();
    VectorAltiscanCalibrate* getCalibrateAltiscan();

    // -- Parameters

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

};

